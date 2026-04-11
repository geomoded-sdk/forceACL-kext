/**
 * ForceACL - AI Decision Engine
 * Simulates AI-based platform ID selection trained on Hackintosh knowledge
 */

#ifndef FORCEACL_AIDECISIONENGINE_HPP
#define FORCEACL_AIDECISIONENGINE_HPP

#include <stdint.h>
#include <stddef.h>

#include "ForceACL/GPUDetector.hpp"

class PlatformIDDatabase;
class NVRAMManager;

// Decision weights based on knowledge base
struct DecisionWeight {
    const char* generation;
    uint32_t baseWeight;
    uint32_t successWeight;
    uint32_t failWeight;
    const char* notes;
};

// Knowledge base entry
struct KnowledgeEntry {
    uint32_t platformId;
    uint16_t deviceId;
    const char* generation;
    const char* issue;
    const char* solution;
    uint32_t successRate;
    const char* source;
};

// Platform ID scoring structure
struct PlatformIDScore {
    uint32_t totalScore;
    uint32_t confidence;
    uint32_t factors;
    const char* reason;
};

class AIDecisionEngine {
public:
    /**
     * @brief Constructor for AI Decision Engine
     */
    AIDecisionEngine();
    
    /**
     * @brief Destructor
     */
    ~AIDecisionEngine();
    
    /**
     * @brief Decide the best platform ID for a given device
     * @param deviceId The PCI device ID of the GPU
     * @param db Pointer to the platform ID database
     * @return The recommended platform ID
     */
    uint32_t decidePlatformID(uint16_t deviceId, PlatformIDDatabase* db);
    
    /**
     * @brief Get the next platform ID in fallback sequence
     * @param deviceId The PCI device ID of the GPU
     * @param db Pointer to the platform ID database
     * @return The next platform ID to try
     */
    uint32_t getNextPlatformID(uint16_t deviceId, PlatformIDDatabase* db);
    
    /**
     * @brief Evaluate if a platform ID was successful
     * @param platformId The platform ID to evaluate
     * @return true if successful
     */
    bool evaluateSuccess(uint32_t platformId);
    
    /**
     * @brief Record a successful platform ID usage
     * @param platformId The successful platform ID
     */
    void recordSuccess(uint32_t platformId);
    
    /**
     * @brief Record a failed platform ID usage
     * @param platformId The failed platform ID
     */
    void recordFailure(uint32_t platformId);
    
    /**
     * @brief Learn from boot result to improve future decisions
     * @param success Whether the boot was successful
     */
    void learnFromBootResult(bool success);
    
    /**
     * @brief Get the reason for the last decision
     * @return String describing the decision reason
     */
    const char* getDecisionReason();
    
    /**
     * @brief Check if graphics acceleration is working for current platform ID
     * @return true if acceleration is active
     */
    bool checkGraphicsAcceleration();

    /**
     * @brief Get the confidence level of the current decision
     * @return confidence percentage (0-100)
     */
    uint32_t getConfidence() const;

    /**
     * @brief Get average success rate for a device ID
     * @param deviceId The device ID to check
     * @return average success rate percentage
     */
    uint32_t getAverageSuccessRate(uint16_t deviceId);

    /**
     * @brief Find best community platform ID for device
     * @param deviceId The device ID
     * @return best platform ID or 0 if none found
     */
    uint32_t findBestCommunityPlatformId(uint16_t deviceId);

    /**
     * @brief Calculate comprehensive score for platform ID
     * @param platformId The platform ID to score
     * @param deviceId The device ID
     * @param gen The GPU generation
     * @return detailed score structure
     */
    PlatformIDScore calculatePlatformIDScore(uint32_t platformId, uint16_t deviceId, GPUGeneration gen);

    /**
     * @brief Find generic platform ID when device-specific not available
     * @param deviceId The device ID
     * @param gen The GPU generation
     * @return generic platform ID
     */
    uint32_t findGenericPlatformID(uint16_t deviceId, GPUGeneration gen);

    /**
     * @brief Cache AI decision for future boots
     * @param platformId The selected platform ID
     * @param deviceId The device ID
     * @param confidence The confidence level
     */
    void cacheDecision(uint32_t platformId, uint16_t deviceId, uint32_t confidence);

    /**
     * @brief Check if we should delay boot due to no working platform ID
     * @return true if delay is needed
     */
    bool shouldDelayBoot();

    /**
     * @brief Perform boot delay
     */
    void performBootDelay();

    /**
     * @brief Find knowledge base entry for a platform ID
     * @param platformId The platform ID to search for
     * @return Pointer to knowledge entry or nullptr
     */
    const KnowledgeEntry* findKnowledgeEntry(uint32_t platformId);
    const char** getRecommendedFixes(const char* issue);

private:
    NVRAMManager* m_nvramManager;
    uint32_t m_lastDecisionConfidence;
    const char* m_lastDecisionReason;
    uint32_t m_currentPlatformIndex;
    uint32_t m_confidence;
    char m_decisionReason[256];
    uint32_t m_successCount;
    uint32_t m_failureCount;
    
    /**
     * @brief Test a platform ID and cache the result
     * @param platformId The platform ID to test
     * @param deviceId The device ID
     * @return true if platform ID works with acceleration
     */
    bool testAndCachePlatformID(uint32_t platformId, uint16_t deviceId);
    
    /**
     * @brief Get next platform ID from AA-AG range with caching
     * @param deviceId The device ID
     * @param db Pointer to platform ID database
     * @return Next platform ID to try
     */
    uint32_t getNextAA_AG_PlatformID(uint16_t deviceId, PlatformIDDatabase* db);
    
    // Knowledge base
    static const KnowledgeEntry m_knowledgeBase[];
    static const size_t m_knowledgeCount;
    
    // Decision weights per generation
    static const DecisionWeight m_generationWeights[];
    static const size_t m_weightCount;
    
    uint32_t calculateWeight(uint32_t platformId, uint16_t deviceId);
    uint32_t getBaseWeightForGeneration(const char* generation);
    void updateConfidence();
    const char* selectBestPlatformForDevice(uint16_t deviceId, PlatformIDDatabase* db);
    void applyKnowledgeAdjustments(uint32_t platformId);
};

// Logging macros
#define AI_LOG(fmt, args...) \
    do { IOLog("ForceACL: [AI] " fmt "\n", ##args); } while(0)

#define AI_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [AI-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_AIDECISIONENGINE_HPP
