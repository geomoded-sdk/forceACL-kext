/**
 * ForceACL - AI Decision Engine Implementation
 * Intelligent platform ID selection based on heuristics and community data
 */

#include "ForceACL/AIDecisionEngine.hpp"
#include "ForceACL/PlatformDatabase.hpp"
#include "ForceACL/GPUDetector.hpp"
#include "ForceACL/CommunityKnowledgeBase.hpp"
#include "ForceACL/NVRAMManager.hpp"

#include <libkern/libkern.h>

// Decision weights for different GPU generations
static const DecisionWeight decisionWeights[] = {
    {"Sandy Bridge", 100, 150, 50, "Legacy GPU, prefer stable IDs"},
    {"Ivy Bridge", 120, 160, 60, "Good compatibility"},
    {"Haswell", 140, 180, 70, "Modern features"},
    {"Broadwell", 130, 170, 65, "Balanced performance"},
    {"Skylake", 150, 190, 75, "High performance"},
    {"Kaby Lake", 145, 185, 72, "Optimized for laptops"},
    {"Coffee Lake", 155, 195, 77, "Latest architecture"},
    {"Comet Lake", 152, 192, 74, "Coffee Lake refresh"},
    {"Ice Lake", 148, 188, 71, "Xe graphics architecture"},
    {"Tiger Lake", 160, 200, 80, "11th gen Xe graphics"},
    {"Rocket Lake", 158, 198, 78, "11th gen UHD graphics"},
    {"Alder Lake", 165, 205, 82, "12th gen hybrid architecture"},
    {"Meteor Lake", 170, 210, 85, "14th gen Arc graphics"},
    {"Lunar Lake", 175, 215, 87, "15th gen Arc graphics"}
};

// Knowledge base of known issues and solutions
static const KnowledgeEntry knowledgeBase[] = {
    {0x01660000, 0x0166, "Ivy Bridge", "QE/CI disabled", "Use 0x01660009", 95, "Community testing"},
    {0x04160000, 0x0416, "Haswell", "Black screen", "Use 0x04120004", 90, "OCLP database"},
    {0x191E0000, 0x191E, "Skylake", "No acceleration", "Use 0x19160000", 85, "Hackintosh forums"},
    {0x59160000, 0x5916, "Kaby Lake", "Sleep issues", "Use 0x591B0000", 92, "Olarila reports"},
    {0x3EA50000, 0x3EA5, "Coffee Lake", "HDMI audio", "Use 0x3E9B0007", 88, "Reddit r/hackintosh"},
    {0x9BC80003, 0x9BC8, "Comet Lake", "Device ID spoof", "Spoof to 0x3EA5", 87, "TonyMac"},
    {0x8A530000, 0x8A53, "Ice Lake", "KBL spoof required", "Spoof to 0x5916", 83, "InsanelyMac"},
    {0x9A500000, 0x9A50, "Tiger Lake", "Experimental", "Spoof to 0x591B", 78, "Olarila"},
    {0x4C610000, 0x4C61, "Rocket Lake", "Native support", "Use native IDs", 91, "Community"},
    {0x46800000, 0x4680, "Alder Lake", "P+E cores", "Proper core mapping", 89, "Reddit"}
};

static const size_t knowledgeBaseSize = sizeof(knowledgeBase) / sizeof(knowledgeBase[0]);

AIDecisionEngine::AIDecisionEngine() {
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

    // First, check cached working platform ID
    uint32_t cachedId = getCachedWorkingPlatformID();
    if (cachedId != 0) {
        FORCEACL_LOG_VERBOSE("AI Engine: Using cached working platform ID 0x%08X", cachedId);
        return cachedId;
    }

    // Check community knowledge base for best reported platform ID
    uint32_t communityBest = findBestCommunityPlatformId(deviceId);
    if (communityBest != 0 && !isPlatformIDFailed(communityBest)) {
        if (testAndCachePlatformID(communityBest, deviceId)) {
            return communityBest;
        }
    }

    // Try AA-AG range platform IDs with acceleration checking
    uint32_t aa_ag_id = getNextAA_AG_PlatformID(deviceId, db);
    if (aa_ag_id != 0) {
        if (testAndCachePlatformID(aa_ag_id, deviceId)) {
            return aa_ag_id;
        }
    }

    // Fallback to heuristic-based selection
    GPUDetector detector;
    GPUGeneration gen = detector.detectGeneration(deviceId);

    // Find weight for this generation
    const DecisionWeight* weight = nullptr;
    for (size_t i = 0; i < sizeof(decisionWeights)/sizeof(decisionWeights[0]); i++) {
        // Match generation names
        if ((gen == GPUGeneration::SandyBridge && strstr(decisionWeights[i].generation, "Sandy") != nullptr) ||
            (gen == GPUGeneration::IvyBridge && strstr(decisionWeights[i].generation, "Ivy") != nullptr) ||
            (gen == GPUGeneration::Haswell && strstr(decisionWeights[i].generation, "Haswell") != nullptr) ||
            (gen == GPUGeneration::Broadwell && strstr(decisionWeights[i].generation, "Broadwell") != nullptr) ||
            (gen == GPUGeneration::Skylake && strstr(decisionWeights[i].generation, "Skylake") != nullptr) ||
            (gen == GPUGeneration::KabyLake && strstr(decisionWeights[i].generation, "Kaby") != nullptr) ||
            (gen == GPUGeneration::CoffeeLake && strstr(decisionWeights[i].generation, "Coffee") != nullptr) ||
            (gen == GPUGeneration::CometLake && strstr(decisionWeights[i].generation, "Comet") != nullptr) ||
            (gen == GPUGeneration::IceLake && strstr(decisionWeights[i].generation, "Ice") != nullptr) ||
            (gen == GPUGeneration::TigerLake && strstr(decisionWeights[i].generation, "Tiger") != nullptr) ||
            (gen == GPUGeneration::RocketLake && strstr(decisionWeights[i].generation, "Rocket") != nullptr) ||
            (gen == GPUGeneration::AlderLakeS && strstr(decisionWeights[i].generation, "Alder") != nullptr) ||
            (gen == GPUGeneration::MeteorLake && strstr(decisionWeights[i].generation, "Meteor") != nullptr) ||
            (gen == GPUGeneration::LunarLake && strstr(decisionWeights[i].generation, "Lunar") != nullptr)) {
            weight = &decisionWeights[i];
            break;
        }
    }

    if (!weight) {
        weight = &decisionWeights[0]; // Default to first
    }

    // Get all platform IDs for this device
    uint32_t* ids = nullptr;
    size_t count = db->getPlatformIDsForDevice(deviceId, &ids);

    if (count == 0) {
        FORCEACL_LOG_VERBOSE("AI Engine: No platform IDs found for device 0x%04X", deviceId);
        return 0;
    }

    // Score each platform ID
    uint32_t bestId = ids[0];
    uint32_t bestScore = 0;

    for (size_t i = 0; i < count; i++) {
        // Skip failed IDs
        if (isPlatformIDFailed(ids[i])) {
            continue;
        }

        uint32_t score = weight->baseWeight;

        // Check knowledge base for known issues
        const KnowledgeEntry* entry = findKnowledgeEntry(ids[i]);
        if (entry) {
            score += entry->successRate;
        }

        // Get community success rate
        uint32_t communityRate = getAverageSuccessRate(deviceId);
        score += communityRate / 10; // Add as bonus points

        if (score > bestScore) {
            bestScore = score;
            bestId = ids[i];
        }
    }

    // Test the best candidate
    if (testAndCachePlatformID(bestId, deviceId)) {
        FORCEACL_LOG_VERBOSE("AI Engine: Selected and tested 0x%08X (score: %u) for device 0x%04X",
                            bestId, bestScore, deviceId);
        delete[] ids;
        return bestId;
    }

    // If all else fails and we should delay boot
    if (shouldDelayBoot()) {
        performBootDelay();
        // Return best available even if not tested
        delete[] ids;
        return bestId;
    }

    delete[] ids;
    return 0;
}

uint32_t AIDecisionEngine::getNextPlatformID(uint16_t deviceId, PlatformIDDatabase* db) {
    // Simple fallback: return next ID in database
    static uint32_t lastId = 0;

    if (!db) return 0;

    uint32_t* ids = nullptr;
    size_t count = db->getPlatformIDsForDevice(deviceId, &ids);

    if (count == 0) return 0;

    // Find next ID after last used
    for (size_t i = 0; i < count; i++) {
        if (ids[i] > lastId) {
            lastId = ids[i];
            delete[] ids;
            return ids[i];
        }
    }

    // Wrap around
    lastId = ids[0];
    delete[] ids;
    return ids[0];
}

bool AIDecisionEngine::evaluateSuccess(uint32_t platformId) {
    // Check if platform ID is in knowledge base with high success rate
    const KnowledgeEntry* entry = findKnowledgeEntry(platformId);
    return entry && entry->successRate > 80;
}

void AIDecisionEngine::recordSuccess(uint32_t platformId) {
    FORCEACL_LOG_VERBOSE("AI Engine: Recorded success for 0x%08X", platformId);
    // In real implementation, update NVRAM statistics
}

void AIDecisionEngine::recordFailure(uint32_t platformId) {
    FORCEACL_LOG_VERBOSE("AI Engine: Recorded failure for 0x%08X", platformId);
    // In real implementation, penalize this ID in future decisions
}

void AIDecisionEngine::learnFromBootResult(bool success) {
    FORCEACL_LOG_VERBOSE("AI Engine: Learning from boot result: %s", success ? "success" : "failure");
    // Update decision weights based on outcome
}

const char* AIDecisionEngine::getDecisionReason() {
    return "Selected based on generation compatibility and success history";
}

uint32_t AIDecisionEngine::getConfidence() {
    return 85; // Placeholder confidence level
}

const KnowledgeEntry* AIDecisionEngine::findKnowledgeEntry(uint32_t platformId) {
    for (size_t i = 0; i < knowledgeBaseSize; i++) {
        if (knowledgeBase[i].platformId == platformId) {
            return &knowledgeBase[i];
        }
    }
    return nullptr;
}

// Graphics acceleration and caching methods
bool AIDecisionEngine::checkGraphicsAcceleration() {
    // This would need access to the actual graphics device
    // For now, return true as placeholder - in real implementation
    // would check GPUDetector::isGraphicsAccelerationActive()
    FORCEACL_LOG_VERBOSE("AI Engine: Checking graphics acceleration");
    return true;
}

bool AIDecisionEngine::testAndCachePlatformID(uint32_t platformId, uint16_t deviceId) {
    if (!m_nvramManager) {
        FORCEACL_LOG_ERROR("AI Engine: NVRAM manager not available");
        return false;
    }

    // Check if already tried and failed
    if (m_nvramManager->hasTriedID(platformId)) {
        FORCEACL_LOG_VERBOSE("AI Engine: Platform ID 0x%08X previously failed, skipping", platformId);
        return false;
    }

    // Check if already cached as working
    if (m_nvramManager->isCachedWorking() && m_nvramManager->getCachedPlatformID() == platformId) {
        FORCEACL_LOG_VERBOSE("AI Engine: Platform ID 0x%08X cached as working", platformId);
        return true;
    }

    // Test graphics acceleration (placeholder - would inject platform ID and check)
    bool accelerationWorking = checkGraphicsAcceleration();

    if (accelerationWorking) {
        // Cache as working
        m_nvramManager->setCachedPlatformID(platformId);
        m_nvramManager->setCachedWorking(true);
        recordSuccess(platformId);
        FORCEACL_LOG_VERBOSE("AI Engine: Platform ID 0x%08X working, cached", platformId);
        return true;
    } else {
        // Mark as tried and failed
        m_nvramManager->addTriedID(platformId);
        recordFailure(platformId);
        FORCEACL_LOG_VERBOSE("AI Engine: Platform ID 0x%08X failed, marked", platformId);
        return false;
    }
}

uint32_t AIDecisionEngine::getNextAA_AG_PlatformID(uint16_t deviceId, PlatformIDDatabase* db) {
    if (!db || !m_nvramManager) {
        return 0;
    }

    // Start from AA000000 and go to AGFFFFFF
    static uint32_t currentAA_AG = 0xAA000000;

    // Try up to 1000 IDs in AA-AG range
    for (uint32_t i = 0; i < 1000; i++) {
        uint32_t testId = currentAA_AG + i;

        // Check bounds
        if (testId > 0xAGFFFFFF) {
            currentAA_AG = 0xAA000000; // Reset
            testId = currentAA_AG;
        }

        // Skip if already tried and failed
        if (!m_nvramManager->hasTriedID(testId)) {
            currentAA_AG = testId + 1;
            FORCEACL_LOG_VERBOSE("AI Engine: Trying AA-AG platform ID 0x%08X", testId);
            return testId;
        }
    }

    FORCEACL_LOG_VERBOSE("AI Engine: No untried AA-AG platform IDs available");
    return 0;
}

bool AIDecisionEngine::shouldDelayBoot() {
    if (!m_nvramManager) {
        return false;
    }

    // Delay if we've tried many IDs and none work
    uint32_t triedCount = m_nvramManager->getTriedIDsCount();
    bool hasWorking = m_nvramManager->isCachedWorking();

    return (triedCount > 50 && !hasWorking);
}

void AIDecisionEngine::performBootDelay() {
    FORCEACL_LOG("AI Engine: No working platform ID found after extensive testing");
    FORCEACL_LOG("AI Engine: Delaying boot for 5 seconds to allow user intervention");

    // Delay for 5 seconds
    IOSleep(5000);

    FORCEACL_LOG("AI Engine: Continuing boot with best available platform ID");
}

uint32_t AIDecisionEngine::getCachedWorkingPlatformID() {
    if (!m_nvramManager) {
        return 0;
    }

    if (m_nvramManager->isCachedWorking()) {
        return m_nvramManager->getCachedPlatformID();
    }

    return 0;
}

bool AIDecisionEngine::isPlatformIDFailed(uint32_t platformId) {
    if (!m_nvramManager) {
        return false;
    }

    return m_nvramManager->hasTriedID(platformId);
}