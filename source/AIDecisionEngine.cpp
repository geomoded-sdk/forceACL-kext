/**
 * ForceACL - AI Decision Engine Implementation
 * Intelligent platform ID selection based on heuristics and community data
 */

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <IOKit/IOService.h>

#include "ForceACL/AIDecisionEngine.hpp"
#include "ForceACL/PlatformDatabase.hpp"
#include "ForceACL/GPUDetector.hpp"
#include "ForceACL/NVRAMManager.hpp"

static const KnowledgeEntry knowledgeBase[] = {
    {0x01660009, 0x0166, "Ivy Bridge", "Stable HD 4000 ID", "Use 0x01660009", 98, "OCLP"},
    {0x04120004, 0x0412, "Haswell", "Best HD 4600 ID", "Use 0x04120004", 96, "OCLP"},
    {0x19120000, 0x1912, "Skylake", "Best HD 530 ID", "Use 0x19120000", 92, "OCLP"},
    {0x591B0000, 0x591B, "Kaby Lake", "Best HD 630 ID", "Use 0x591B0000", 96, "OCLP"},
    {0x3E9B0000, 0x3E9B, "Coffee Lake", "Best UHD 630 ID", "Use 0x3E9B0000", 97, "OCLP"},
    {0x9BC80000, 0x9BC8, "Comet Lake", "Stable UHD 630 ID", "Use 0x9BC80000", 93, "Community"},
    {0x8A530000, 0x8A53, "Ice Lake", "Preferred Xe ID", "Use 0x8A530000", 88, "OCLP"},
    {0x9A500000, 0x9A50, "Tiger Lake", "Preferred Xe ID", "Use 0x9A500000", 90, "OCLP"},
    {0x4C610000, 0x4C61, "Rocket Lake", "Native Rocket Lake ID", "Use 0x4C610000", 91, "Community"},
    {0x46800000, 0x4680, "Alder Lake", "Native Alder Lake ID", "Use 0x46800000", 90, "Community"}
};

static const size_t knowledgeBaseSize = sizeof(knowledgeBase) / sizeof(knowledgeBase[0]);

AIDecisionEngine::AIDecisionEngine()
    : m_nvramManager(nullptr)
    , m_lastDecisionConfidence(0)
    , m_lastDecisionReason("No decision yet")
    , m_currentPlatformIndex(0)
    , m_confidence(60)
    , m_successCount(0)
    , m_failureCount(0)
{
    FORCEACL_LOG_VERBOSE("AI Decision Engine initialized");
    m_nvramManager = new NVRAMManager();
}

AIDecisionEngine::~AIDecisionEngine() {
    if (m_nvramManager) {
        delete m_nvramManager;
    }
    FORCEACL_LOG_VERBOSE("AI Decision Engine destroyed");
}

uint32_t AIDecisionEngine::decidePlatformID(uint16_t deviceId, PlatformIDDatabase* db) {
    if (!db) {
        FORCEACL_LOG_ERROR("AI Engine: null database");
        return 0;
    }

    FORCEACL_LOG("AI Engine: deciding platform ID for device 0x%04X", deviceId);

    uint32_t cachedId = 0;
    if (m_nvramManager && m_nvramManager->isCachedWorking()) {
        cachedId = m_nvramManager->getCachedPlatformID();
        if (cachedId != 0) {
            m_lastDecisionReason = "Cached working platform ID";
            m_lastDecisionConfidence = 95;
            return cachedId;
        }
    }

    uint32_t communityId = findBestCommunityPlatformId(deviceId);
    if (communityId != 0) {
        if (!m_nvramManager || !m_nvramManager->hasTriedID(communityId)) {
            m_lastDecisionReason = "Community recommended ID";
            m_lastDecisionConfidence = 85;
            return communityId;
        }
    }

    GPUDetector detector;
    GPUGeneration gen = detector.detectGeneration(deviceId);

    size_t count = 0;
    const PlatformIDEntry** ids = db->getPlatformIDsForDevice(deviceId, &count);
    if (!ids || count == 0) {
        m_lastDecisionReason = "No platform IDs in database";
        m_lastDecisionConfidence = 55;
        return findGenericPlatformID(deviceId, gen);
    }

    uint32_t bestId = 0;
    uint32_t bestScore = 0;
    uint32_t bestConfidence = 0;

    for (size_t i = 0; i < count; i++) {
        uint32_t platformId = ids[i]->id;
        if (m_nvramManager && m_nvramManager->hasTriedID(platformId)) {
            continue;
        }

        PlatformIDScore score = calculatePlatformIDScore(platformId, deviceId, gen);
        if (score.totalScore > bestScore) {
            bestScore = score.totalScore;
            bestId = platformId;
            bestConfidence = score.confidence;
            m_lastDecisionReason = score.reason;
        }
    }

    if (bestId != 0) {
        m_lastDecisionConfidence = bestConfidence;
        if (m_nvramManager) {
            m_nvramManager->setCachedPlatformID(bestId);
        }
    }

    return bestId;
}

uint32_t AIDecisionEngine::getNextPlatformID(uint16_t deviceId, PlatformIDDatabase* db) {
    if (!db) return 0;
    
    m_currentPlatformIndex++;
    
    size_t count = 0;
    const PlatformIDEntry** ids = db->getPlatformIDsForDevice(deviceId, &count);
    if (!ids || count == 0) {
        return findGenericPlatformID(deviceId, GPUGeneration::Unknown);
    }
    
    if (m_currentPlatformIndex >= count) {
        m_currentPlatformIndex = 0;
    }
    
    return ids[m_currentPlatformIndex]->id;
}

bool AIDecisionEngine::evaluateSuccess(uint32_t platformId) {
    return m_lastDecisionConfidence >= 70;
}

void AIDecisionEngine::recordSuccess(uint32_t platformId) {
    m_successCount++;
    FORCEACL_LOG("AI: Recorded success for 0x%08X", platformId);
    if (m_nvramManager) {
        m_nvramManager->setCachedPlatformID(platformId);
        m_nvramManager->setCachedWorking(true);
    }
}

void AIDecisionEngine::recordFailure(uint32_t platformId) {
    m_failureCount++;
    FORCEACL_LOG("AI: Recorded failure for 0x%08X", platformId);
    if (m_nvramManager) {
        m_nvramManager->setCachedWorking(false);
    }
}

void AIDecisionEngine::learnFromBootResult(bool success) {
    if (success) {
        recordSuccess(m_currentPlatformIndex > 0 ? m_currentPlatformIndex : 0);
    } else {
        recordFailure(m_currentPlatformIndex > 0 ? m_currentPlatformIndex : 0);
    }
    updateConfidence();
}

const char* AIDecisionEngine::getDecisionReason() {
    return m_lastDecisionReason;
}

bool AIDecisionEngine::checkGraphicsAcceleration() {
    return m_lastDecisionConfidence >= 70;
}

uint32_t AIDecisionEngine::getConfidence() const {
    return m_confidence;
}

uint32_t AIDecisionEngine::getAverageSuccessRate(uint16_t deviceId) {
    uint32_t total = m_successCount + m_failureCount;
    if (total == 0) return 50;
    return (m_successCount * 100) / total;
}

uint32_t AIDecisionEngine::findBestCommunityPlatformId(uint16_t deviceId) {
    uint32_t best = 0;
    uint32_t bestRate = 0;
    
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].deviceId == deviceId && 
            knowledgeBase[i].successRate > bestRate) {
            best = knowledgeBase[i].platformId;
            bestRate = knowledgeBase[i].successRate;
        }
    }
    
    return best;
}

PlatformIDScore AIDecisionEngine::calculatePlatformIDScore(uint32_t platformId, uint16_t deviceId, GPUGeneration gen) {
    PlatformIDScore score = {};
    score.totalScore = 100;
    score.confidence = 60;
    score.factors = 0;
    score.reason = "Default heuristic";
    
    uint32_t low = platformId & 0xFFFF;
    if ((low & 0xFF00) == 0x0000) {
        score.totalScore += 5;
        score.factors++;
    }
    if ((low & 0x000F) <= 0x09) {
        score.totalScore += 3;
        score.factors++;
    }
    
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].platformId == platformId) {
            score.totalScore += (knowledgeBase[i].successRate / 10);
            score.confidence = knowledgeBase[i].successRate;
            score.reason = knowledgeBase[i].solution;
            break;
        }
    }
    
    return score;
}

uint32_t AIDecisionEngine::findGenericPlatformID(uint16_t deviceId, GPUGeneration gen) {
    switch (gen) {
        case GPUGeneration::SandyBridge: return 0x00030010;
        case GPUGeneration::IvyBridge: return 0x01660009;
        case GPUGeneration::Haswell: return 0x0D220003;
        case GPUGeneration::Broadwell: return 0x16060000;
        case GPUGeneration::Skylake: return 0x19160000;
        case GPUGeneration::KabyLake: return 0x59160000;
        case GPUGeneration::CoffeeLake: return 0x3EA50000;
        case GPUGeneration::CometLake: return 0x9BC80003;
        case GPUGeneration::IceLake: return 0x8A530000;
        case GPUGeneration::TigerLake: return 0x9A500000;
        case GPUGeneration::RocketLake: return 0x4C610000;
        case GPUGeneration::AlderLakeS: return 0x46800000;
        case GPUGeneration::MeteorLake: return 0x7D450000;
        default: return 0x0D220003;
    }
}

void AIDecisionEngine::cacheDecision(uint32_t platformId, uint16_t deviceId, uint32_t confidence) {
    if (m_nvramManager) {
        m_nvramManager->setCachedPlatformID(platformId);
        m_nvramManager->setCachedWorking(confidence >= 70);
    }
    m_confidence = confidence;
}

bool AIDecisionEngine::shouldDelayBoot() {
    return m_failureCount > 3 && m_successCount == 0;
}

void AIDecisionEngine::performBootDelay() {
    FORCEACL_LOG("AI: Performing boot delay due to repeated failures");
}

const KnowledgeEntry* AIDecisionEngine::findKnowledgeEntry(uint32_t platformId) {
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].platformId == platformId) {
            return &knowledgeBase[i];
        }
    }
    return nullptr;
}

const char** AIDecisionEngine::getRecommendedFixes(const char* issue) {
    static const char* noFixes[] = {nullptr};
    return noFixes;
}

void AIDecisionEngine::updateConfidence() {
    uint32_t total = m_successCount + m_failureCount;
    if (total > 0) {
        m_confidence = (m_successCount * 100) / total;
    }
}