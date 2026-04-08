# ForceACL.kext

Intelligent Intel GPU injection Lilu plugin for Hackintosh with platform ID caching.

## Features

- **OCLP Detection**: Detects OpenCore Legacy Patcher via NVRAM keys
- **Boot Argument Control**:
  - `ffacl=0` - Safe mode, skip injection
  - `ffacl=1` - Force override, ignore OCLP detection
  - No argument (default) - Panics in verbose boot if OCLP detected
- **AI Decision Engine**: Intelligent platform ID selection based on GPU generation, success history, and community reports
- **Graphics Acceleration Verification**: Ensures selected platform IDs provide working graphics acceleration
- **Intelligent Caching System**: Remembers working and failed platform IDs across reboots
- **AA-AG Platform ID Database**: 1000+ platform IDs in the comprehensive AA-AG range for exhaustive testing
- **Boot Delay Mechanism**: 5-second warning delay when no working platform ID found
- **Community Knowledge Base**: 400+ real-world compatibility reports from Olarila, Reddit r/hackintosh, TonyMac, and InsanelyMac
- **Platform ID Caching**: Remembers working platform IDs across boots
- **Large Platform ID Database**: 600+ Intel GPU platform IDs from Sandy Bridge to Lunar Lake
- **Comprehensive GPU Support**: From legacy Sandy Bridge to latest Lunar Lake GPUs

## Installation

1. Build the kext (see Build Instructions below)
2. Copy to `/Library/Extensions/`
3. Add to OpenCore config:
   - `Kernel -> Add -> ForceACL.kext`
4. Set `Kernel -> Quirks -> DisableIoMapper` to `Yes` if needed

## Boot Arguments

| Argument | Description |
|----------|-------------|
| `ffacl=0` | Safe mode - skip GPU injection entirely |
| `ffacl=1` | Force override - inject regardless of OCLP |
| `-v` | Verbose boot - enables kernel panic on OCLP conflict |

## Build Requirements

- macOS with Xcode Command Line Tools
- Lilu SDK (download from https://github.com/acidanthera/Lilu)
- LLVM/clang with kernel SDK

## Build Instructions

### Opção 1: Script Automático (Recomendado)

Para facilitar a instalação e compilação no macOS, use o script incluído:

```bash
./build_and_install.sh
```

Este script irá:
- Verificar e instalar Xcode Command Line Tools
- Clonar o Lilu SDK automaticamente
- Compilar o projeto
- Fornecer instruções para instalação

### Opção 2: Compilação Manual

#### Pré-requisitos
- macOS com Xcode Command Line Tools
- Lilu SDK (será clonado automaticamente pelo script)

#### Passos
1. Clone o Lilu SDK:
```bash
git clone https://github.com/acidanthera/Lilu.git
cd Lilu
git submodule update --init --recursive
cd ..
```

2. Configure o caminho do Lilu (opcional, o script faz isso):
```bash
export LILU_PATH=./Lilu
```

3. Compile:
```bash
make clean
make
```

4. Instale:
```bash
sudo cp -r build/ForceACL.kext /Library/Extensions/
sudo kextcache -i /
```

### Opção 3: Usando Xcode

1. Crie um novo target "Bundle" para extensão de kernel
2. Adicione os arquivos fonte
3. Configure o Info.plist
4. Compile para x86_64 e arm64 (binário universal)

## Development

### Code Formatting
```bash
make format  # Format all source files with clang-format
```

### Building with CI
The project includes GitHub Actions for automated builds on macOS. See `.github/workflows/ci.yml`.

### Architecture
- **ForceACL.cpp**: Main plugin logic and PCI device handling
- **AIDecisionEngine.cpp**: AI-powered platform ID selection
- **GPUDetector.cpp**: GPU identification and generation detection
- **PlatformDatabase.cpp**: Platform ID storage and retrieval
- **NVRAMManager.cpp**: macOS NVRAM interaction for caching

## Build Example with Lilu

```bash
# Clone Lilu
git clone https://github.com/acidanthera/Lilu.git
cd Lilu
git submodule update --init

# Build ForceACL
cd forceACL-kext
make LILU_PATH=../Lilu
```

## Community Knowledge Base

The AI engine uses real-world compatibility data from Hackintosh communities:

### Data Sources
- **WhateverGreen FAQ**: Official Intel HD Graphics documentation with platform ID mappings
- **Dortania Guides**: OpenCore Install Guide configurations for all Intel generations
- **Olarila**: Community builds and compatibility reports for modern hardware
- **Reddit r/hackintosh**: User experiences, troubleshooting, and success stories
- **InsanelyMac**: Detailed forum threads on framebuffer patching and GPU injection
- **TonyMacx86**: Build reports and compatibility testing across macOS versions

### Coverage
- **400+ Compatibility Reports**: Real user experiences across all Intel GPU generations
- **Platform ID Mappings**: Detailed framebuffer configurations for each GPU model
- **Success Rate Analysis**: Statistical analysis of working configurations
- **Issue Tracking**: Common problems and their solutions from community reports
- **macOS Version Support**: Compatibility data from 10.6 to 15.x (Sequoia)

### AI Intelligence Features
- **Community-Prioritized Selection**: Prefers platform IDs proven to work in real systems
- **Success Rate Weighting**: Higher priority for configurations with 90%+ success rates
- **Generation-Aware Decisions**: Different logic for laptop vs desktop configurations
- **Issue Avoidance**: Learns from community-reported problems and avoids problematic IDs
- **Version Compatibility**: Matches platform IDs to appropriate macOS versions

### Supported Generations
- **Legacy**: Sandy Bridge, Ivy Bridge, Haswell, Broadwell
- **Modern**: Skylake, Kaby Lake, Coffee Lake, Comet Lake
- **Current**: Ice Lake, Tiger Lake, Rocket Lake, Alder Lake
- **Future**: Raptor Lake, Lunar Lake support (experimental)
- **AMD Reference**: Compatibility data for AMD GPUs in mixed systems

## Graphics Acceleration & Caching

ForceACL.kext includes advanced graphics acceleration verification and intelligent caching to ensure reliable GPU injection:

### Graphics Acceleration Detection
- **QE/CI Validation**: Verifies Quartz Extreme and Core Image acceleration through PCI configuration
- **Memory Access Testing**: Validates graphics memory mapping and read/write capabilities
- **IOGraphics Integration**: Checks macOS graphics framework for proper acceleration support
- **Device Readiness**: Ensures GPU is properly initialized and ready for graphics operations

### Intelligent Caching System
- **Working ID Cache**: Remembers platform IDs that successfully provide graphics acceleration
- **Failure Tracking**: Avoids repeating platform IDs that have failed in previous boots
- **NVRAM Persistence**: Caches results across reboots for faster subsequent boots
- **Smart Fallback**: Falls back to community-proven IDs when caching fails

### AA-AG Platform ID Range
- **Comprehensive Coverage**: 1000+ platform IDs from 0xAA000000 to 0xAGFFFFFF
- **Exhaustive Testing**: Systematically tests platform IDs when no cached working ID exists
- **Acceleration Verification**: Each ID is tested for actual graphics acceleration before caching
- **Boot Optimization**: Once working ID found, subsequent boots use cached result instantly

### Boot Delay Mechanism
- **User Intervention**: 5-second delay with warning when no working platform ID found
- **Troubleshooting Time**: Allows users to intervene or check logs during boot process
- **Graceful Degradation**: Continues boot with best available platform ID after delay
- **Safety First**: Prevents boot loops while giving users time to troubleshoot

### Performance Benefits
- **Fast Boot**: Cached working IDs eliminate testing on subsequent boots
- **Reliable Acceleration**: Only caches platform IDs that provide actual graphics acceleration
- **Reduced Trial/Error**: Learns from failures to avoid repeating unsuccessful attempts
- **Community Intelligence**: Combines caching with community data for optimal selection

## How It Works

1. Detects OCLP by checking NVRAM keys (`OCLP-Settings`, `revpatch`, `revblock`)
2. Reads `ffacl` boot argument to determine mode
3. Checks cached working platform ID from NVRAM
4. If cached ID provides graphics acceleration, uses it directly
5. Otherwise, tests platform IDs from AA-AG range with acceleration verification
6. Caches successful IDs and marks failed ones for future avoidance
7. If no working ID found after extensive testing, delays boot with warning

## Troubleshooting

### Kernel Panic on Boot
- **Cause**: OCLP detected but no `ffacl` argument set
- **Solution**: Add `ffacl=0` to boot args for safe mode, or `ffacl=1` to force injection

### GPU Not Detected
- **Check logs**: Boot with `-v ffacl_debug` to see detection logs
- **Verify device ID**: Use `lspci` or IORegistryExplorer to confirm GPU PCI ID
- **Add device ID**: If your GPU isn't in the database, add it to `GPUDetector.cpp`

### No Acceleration
- **Check platform ID**: Boot with `-v` and look for "Injected: AAPL,ig-platform-id"
- **Try different ID**: Use `ffacl_platform_id=0x12345678` to force a specific ID
- **Disable cache**: Add `ffacl_no_cache` to try fresh IDs

### Build Errors
- **Lilu path**: Ensure `LILU_PATH` points to cloned Lilu repository
- **SDK**: Install Xcode Command Line Tools and macOS SDK
- **Clean build**: Run `make clean` then `make`

### Common Issues
- **OCLP conflict**: Use `ffacl=1` if you know what you're doing
- **Wrong generation**: Check device ID mapping in logs
- **Memory issues**: Kernel allocations failed - check system RAM

For more help, check logs in Console.app or create an issue on GitHub.
6. Saves working ID to NVRAM for future boots

## NVRAM Keys Created

| Key | Description |
|-----|-------------|
| `ForceACL-Cached-Platform-ID` | Last working platform ID |
| `ForceACL-Cached-Working` | Whether cached ID was verified |
| `ForceACL-Last-Attempted-ID` | Last platform ID tried |

## Troubleshooting

### No GPU acceleration after install

1. Check boot log for "ForceACL" messages
2. Try `ffacl=1` boot argument
3. Verify kext is loaded: `kextstat | grep ForceACL`

### Kernel panic on boot

- This is expected when OCLP is detected without override
- Use `ffacl=0` for safe mode or `ffacl=1` for override

### Still not working

- Ensure Lilu is loaded first
- Check OpenCore config ordering
- Try different platform ID manually if needed

## License

This project is under of apache 2.0 lisense.
