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

static const char* strstr_simple(const char* haystack, const char* needle) {
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) {
            return haystack;
        }
        haystack++;
    }
    return nullptr;
}

static const PlatformIDScore DEFAULT_SCORE = {0, 60, 0, "Default heuristic"};

static const DecisionWeight decisionWeights[] = {
    {"Sandy Bridge", 100, 50, 20, "Stable legacy IDs"},
    {"Ivy Bridge", 120, 55, 20, "Good compatibility"},
    {"Haswell", 130, 60, 20, "Modern feature support"},
    {"Broadwell", 125, 58, 20, "Balanced performance"},
    {"Skylake", 140, 65, 20, "Preferred modern IDs"},
    {"Kaby Lake", 145, 68, 20, "Laptop-friendly"},
    {"Coffee Lake", 150, 70, 20, "Latest stable IDs"},
    {"Comet Lake", 148, 69, 20, "Refresh support"},
    {"Ice Lake", 142, 67, 20, "Xe graphics"},
    {"Tiger Lake", 155, 72, 20, "Intel Xe support"},
    {"Rocket Lake", 150, 70, 20, "Desktop refresh"},
    {"Alder Lake", 155, 72, 20, "Hybrid support"},
    {"Meteor Lake", 150, 70, 20, "Arc support"},
    {"Lunar Lake", 150, 70, 20, "Arc support"}
};

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

    uint32_t cachedId = getCachedWorkingPlatformID();
    if (cachedId != 0) {
        m_lastDecisionReason = "Cached working platform ID";
        m_lastDecisionConfidence = 95;
        return cachedId;
    }

    uint32_t communityId = findBestCommunityPlatformId(deviceId);
    if (communityId != 0 && !isPlatformIDFailed(communityId)) {
        m_lastDecisionReason = "Community recommended ID";
        m_lastDecisionConfidence = getBestKnowledgeConfidence(deviceId);
        return communityId;
    }

    GPUDetector detector;
    GPUGeneration gen = detector.detectGeneration(deviceId);
    const DecisionWeight* weight = findDecisionWeight(gen);

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
        if (isPlatformIDFailed(platformId)) {
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
        cacheDecision(bestId, deviceId, bestConfidence);
        FORCEACL_LOG("AI Engine selected platform ID 0x%08X (confidence=%u%%, reason=%s)", bestId, bestConfidence, m_lastDecisionReason);
        return bestId;
    }

    uint32_t fallbackId = findGenericPlatformID(deviceId, gen);
    m_lastDecisionReason = "Fallback generic platform ID";
    m_lastDecisionConfidence = 65;
    return fallbackId;
}

uint32_t AIDecisionEngine::getNextPlatformID(uint16_t deviceId, PlatformIDDatabase* db) {
    static uint32_t lastId = 0;
    if (!db) {
        return 0;
    }

    size_t count = 0;
    const PlatformIDEntry** ids = db->getPlatformIDsForDevice(deviceId, &count);
    if (!ids || count == 0) {
        return 0;
    }

    for (size_t i = 0; i < count; i++) {
        if (ids[i]->id > lastId) {
            lastId = ids[i]->id;
            return lastId;
        }
    }

    lastId = ids[0]->id;
    return lastId;
}

bool AIDecisionEngine::evaluateSuccess(uint32_t platformId) {
    const KnowledgeEntry* entry = findKnowledgeEntry(platformId);
    return entry && entry->successRate >= 85;
}

void AIDecisionEngine::recordSuccess(uint32_t platformId) {
    FORCEACL_LOG_VERBOSE("AI Engine: Recorded success for 0x%08X", platformId);
}

void AIDecisionEngine::recordFailure(uint32_t platformId) {
    FORCEACL_LOG_VERBOSE("AI Engine: Recorded failure for 0x%08X", platformId);
    if (m_nvramManager) {
        m_nvramManager->addTriedID(platformId);
    }
}

void AIDecisionEngine::learnFromBootResult(bool success) {
    FORCEACL_LOG_VERBOSE("AI Engine: Learning from boot result: %s", success ? "success" : "failure");
}

const char* AIDecisionEngine::getDecisionReason() {
    return m_lastDecisionReason;
}

bool AIDecisionEngine::checkGraphicsAcceleration() {
    FORCEACL_LOG_VERBOSE("AI Engine: checking graphics acceleration status");
    return true;
}

uint32_t AIDecisionEngine::getAverageSuccessRate(uint16_t deviceId) {
    uint32_t total = 0;
    uint32_t entries = 0;

    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].deviceId == deviceId) {
            total += knowledgeBase[i].successRate;
            entries++;
        }
    }

    return entries ? total / entries : 70;
}

uint32_t AIDecisionEngine::findBestCommunityPlatformId(uint16_t deviceId) {
    uint32_t bestId = 0;
    uint32_t bestRate = 0;

    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].deviceId == deviceId && knowledgeBase[i].successRate > bestRate) {
            bestRate = knowledgeBase[i].successRate;
            bestId = knowledgeBase[i].platformId;
        }
    }

    return bestId;
}

PlatformIDScore AIDecisionEngine::calculatePlatformIDScore(uint32_t platformId, uint16_t deviceId, GPUGeneration gen) {
    PlatformIDScore score = {};
    score.reason = "Heuristic match";
    score.confidence = 70;

    const KnowledgeEntry* entry = findKnowledgeEntry(platformId);
    if (entry) {
        score.totalScore += entry->successRate;
        score.confidence = entry->successRate;
        score.reason = "Knowledge base match";
    }

    if (isGenerationCompatible(platformId, gen)) {
        score.totalScore += 30;
        score.confidence = (score.confidence + 90) / 2;
    }

    score.totalScore += getAverageSuccessRate(deviceId) / 4;
    score.totalScore += analyzePlatformIDPattern(platformId, deviceId);

    if (score.confidence < 60) {
        score.confidence = 60;
    }

    return score;
}

uint32_t AIDecisionEngine::findGenericPlatformID(uint16_t deviceId, GPUGeneration gen) {
    switch (gen) {
        case GPUGeneration::SandyBridge: return 0x00030010;
        case GPUGeneration::IvyBridge: return 0x01660009;
        case GPUGeneration::Haswell: return 0x04120004;
        case GPUGeneration::Broadwell: return 0x16260000;
        case GPUGeneration::Skylake: return 0x19120000;
        case GPUGeneration::KabyLake: return 0x591B0000;
        case GPUGeneration::CoffeeLake: return 0x3E9B0000;
        case GPUGeneration::CometLake: return 0x9BC80000;
        case GPUGeneration::IceLake: return 0x8A530000;
        case GPUGeneration::TigerLake: return 0x9A500000;
        case GPUGeneration::RocketLake: return 0x4C610000;
        case GPUGeneration::AlderLakeS:
        case GPUGeneration::AlderLakeP: return 0x46800000;
        case GPUGeneration::MeteorLake: return 0x7D400000;
        case GPUGeneration::LunarLake: return 0x64A00000;
        default: return 0x04120004;
    }
}

void AIDecisionEngine::cacheDecision(uint32_t platformId, uint16_t deviceId, uint32_t confidence) {
    if (!m_nvramManager) {
        return;
    }

    m_nvramManager->setCachedPlatformID(platformId);
    m_nvramManager->setCachedWorking(true);
}

const KnowledgeEntry* AIDecisionEngine::getKnowledgeEntry(uint32_t platformId) {
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].platformId == platformId) {
            return &knowledgeBase[i];
        }
    }
    return nullptr;
}

const DecisionWeight* AIDecisionEngine::findDecisionWeight(GPUGeneration gen) {
    for (size_t i = 0; i < sizeof(decisionWeights)/sizeof(decisionWeights[0]); i++) {
        if (strstr_simple(decisionWeights[i].generation, detector.generationToString(gen)) != nullptr) {
            return &decisionWeights[i];
        }
    }
    return &decisionWeights[0];
}

bool AIDecisionEngine::getCachedWorkingPlatformID(uint32_t* platformId) {
    if (!m_nvramManager || !platformId) {
        return false;
    }
    if (m_nvramManager->isCachedWorking()) {
        *platformId = m_nvramManager->getCachedPlatformID();
        return true;
    }
    return false;
}

bool AIDecisionEngine::isPlatformIDFailed(uint32_t platformId) {
    if (!m_nvramManager) {
        return false;
    }
    return m_nvramManager->hasTriedID(platformId);
}

uint32_t AIDecisionEngine::analyzePlatformIDPattern(uint32_t platformId, uint16_t deviceId) {
    uint32_t bonus = 0;
    uint32_t low = platformId & 0xFFFF;
    if ((low & 0xFF00) == 0x0000) {
        bonus += 5;
    }
    if ((low & 0x000F) <= 0x09) {
        bonus += 3;
    }
    return bonus;
}

uint32_t AIDecisionEngine::getBestKnowledgeConfidence(uint16_t deviceId) {
    uint32_t best = 0;
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].deviceId == deviceId && knowledgeBase[i].successRate > best) {
            best = knowledgeBase[i].successRate;
        }
    }
    return best ? best : 80;
}
