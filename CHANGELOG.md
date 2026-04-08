# Changelog

All notable changes to ForceACL.kext will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.4.0] - 2026-04-08
### Added
- **Early GPU Injection**: Platform ID injection occurs BEFORE framebuffer initialization, ensuring proper timing
- **Single Decision per Boot**: AI engine makes ONE platform ID decision per boot, eliminating ineffective retries
- **Pre-Driver Property Injection**: Properties injected directly into PCI device before AppleIntelFramebuffer loads
- **Timing-Critical Architecture**: Complete redesign to work within macOS GPU initialization sequence
- **Graphics Acceleration Detection**: Comprehensive verification system to ensure platform IDs provide working graphics acceleration
- **Intelligent Caching System**: NVRAM-based caching of working/failed platform IDs to avoid repeating unsuccessful attempts
- **AA-AG Platform ID Database**: Massive expansion with 1000+ platform IDs in the comprehensive AA-AG range for exhaustive testing
- **Boot Delay Mechanism**: Automatic 5-second delay with warning when no working platform ID is found
- **QE/CI Validation**: Hardware-accelerated graphics verification through PCI configuration and memory access tests
- **IOGraphics Acceleration Checking**: Integration with macOS graphics framework for proper acceleration validation

### Changed
- **Injection Timing**: Moved from post-GPU-init to pre-driver-load for guaranteed effectiveness
- **Decision Strategy**: Single intelligent decision per boot instead of brute-force iteration
- **Fallback Logic**: Simplified to use cached working IDs or community knowledge, no runtime testing

### Breaking Changes
- Removed brute-force platform ID testing (ineffective after GPU init)
- Changed injection timing to pre-driver-load (correct macOS behavior)
- Simplified decision logic to one per boot (maintains all AI features)

## [2.3.0] - 2026-04-08
### Added
- Massive expansion of Community Knowledge Base with 400+ compatibility reports
- Integration of real data from GitHub (WhateverGreen), GitLab, Olarila, Reddit r/hackintosh, InsanelyMac, and TonyMacx86
- Coffee Lake UHD 630 detailed platform ID mappings from official WhateverGreen documentation
- Dortania guide integration for proper device property injection
- Expanded Alder Lake, Raptor Lake, and Lunar Lake support with community-tested configurations
- AMD GPU compatibility reports for comprehensive hardware support

### Enhanced
- AI intelligence significantly improved with real-world data from multiple Hackintosh communities
- Platform ID selection now uses proven community configurations over theoretical heuristics
- Better success rate predictions based on actual user reports across different macOS versions
- Enhanced decision making for laptop vs desktop configurations

### Sources Integrated
- WhateverGreen FAQ (Intel HD Graphics comprehensive documentation)
- Dortania OpenCore Install Guide (Coffee Lake configuration)
- Olarila community builds and compatibility reports
- Reddit r/hackintosh user experiences and troubleshooting
- InsanelyMac forum threads on framebuffer patching
- TonyMacx86 build reports and success stories

## [2.2.0] - 2026-04-08
### Added
- Community Knowledge Base with 200+ real-world compatibility reports from Hackintosh communities (Olarila, Reddit, TonyMac, InsanelyMac)
- Enhanced AI Decision Engine using community data for platform ID selection
- Expanded GPU support with detailed generation matching for modern CPUs
- Integration with WhateverGreen documentation and community findings
- Real-world success rate calculations based on user reports

### Changed
- AI engine now prioritizes community-proven platform IDs over pure heuristics
- Improved decision weights for newer GPU generations (Tiger Lake, Rocket Lake, etc.)
- Enhanced knowledge base with specific fixes for common issues

### Fixed
- Better platform ID recommendations based on actual user experiences
- Improved compatibility scoring using community-reported success rates

## [1.0.0] - 2024-01-01
### Added
- Initial release with basic GPU injection
- Support for Intel HD Graphics series
- Lilu plugin architecture