# Changelog

All notable changes to ForceACL.kext will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.4.0] - 2026-04-08
### Added
- **Graphics Acceleration Detection**: Comprehensive verification system to ensure platform IDs provide working graphics acceleration
- **Intelligent Caching System**: NVRAM-based caching of working/failed platform IDs to avoid repeating unsuccessful attempts
- **AA-AG Platform ID Database**: Massive expansion with 1000+ platform IDs in the AA-AG range (0xAA000000 to 0xAGFFFFFF)
- **Boot Delay Mechanism**: Automatic 5-second delay with warning when no working platform ID is found, allowing user intervention
- **QE/CI Validation**: Hardware-accelerated graphics verification through PCI configuration and memory access tests
- **IOGraphics Acceleration Checking**: Integration with macOS graphics framework for proper acceleration validation

### Enhanced
- **AI Decision Engine**: Now includes acceleration testing and caching logic for more reliable platform ID selection
- **GPU Detector**: Extended with graphics acceleration detection methods and memory access validation
- **Platform Database**: Expanded with comprehensive AA-AG range coverage for exhaustive testing
- **NVRAM Manager**: Enhanced with intelligent caching of platform ID success/failure states

### Technical Improvements
- Graphics memory mapping and access validation
- PCI device capability checking for QE/CI support
- IOGraphics framework integration for acceleration verification
- Boot-time decision making with user intervention capability
- Persistent caching across reboots for optimal performance

### Safety Features
- Automatic fallback to best available platform ID if no acceleration is detected
- User warning and delay mechanism for troubleshooting
- Comprehensive error handling for graphics device detection
- Memory safety checks during graphics acceleration testing

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