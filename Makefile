#
# ForceACL.kext Makefile
# Lilu plugin for Intel GPU injection with AI decision engine
# Build: make (debug) or make BUILD_TYPE=release
#

PRODUCT_NAME = ForceACL
KEXT_NAME = ForceACL
LILU_PATH ?= $(HOME)/Lilu

# Build type: debug or release
BUILD_TYPE ?= debug
ifeq ($(BUILD_TYPE),release)
    OPT_FLAGS = -O2
    STRIP_SYMBOLS = -Wl,-x
    VERSION_SUFFIX = 
    BUILD_NAME = Release
else
    OPT_FLAGS = -g -O0
    VERSION_SUFFIX = -debug
    BUILD_NAME = Debug
endif

# Source files
SOURCES = \
	source/plugin_main.cpp \
	source/ForceACL.cpp \
	source/AIDecisionEngine.cpp \
	source/CommunityKnowledgeBase.cpp \
	source/GPUDetector.cpp \
	source/PlatformDatabase.cpp \
	source/GPUInjector.cpp \
	source/NVRAMManager.cpp \
	source/ErrorHandler.cpp \
	source/HookManager.cpp \
	source/FramebufferPatcher.cpp \
	source/ConnectorFixer.cpp \
	source/GPUPropertiesManager.cpp

# Headers  
HEADERS = \
	include/ForceACL/ForceACL.hpp \
	include/ForceACL/PlatformDatabase.hpp \
	include/ForceACL/GPUDetector.hpp \
	include/ForceACL/GPUInjector.hpp \
	include/ForceACL/NVRAMManager.hpp \
	include/ForceACL/AIDecisionEngine.hpp \
	include/ForceACL/ErrorHandler.hpp \
	include/ForceACL/HookManager.hpp \
	include/ForceACL/FramebufferPatcher.hpp \
	include/ForceACL/ConnectorFixer.hpp \
	include/ForceACL/GPUPropertiesManager.hpp \
	include/ForceACL/CommunityKnowledgeBase.hpp \
	include/ForceACL/WGCompat.hpp

# macOS SDK detection
SDK_ROOT = /Library/Developer/CommandLineTools/SDKs
SYSROOT ?= $(shell ls -d $(SDK_ROOT)/MacOSX* 2>/dev/null | tail -1)
ifeq ($(SYSROOT),)
    $(error macOS SDK not found. Please install CommandLineTools: xcode-select --install)
endif

# Single or multiple architectures
ifeq ($(ARCHS),x86_64)
    DEPLOYMENT_TARGET = $(DEPLOYMENT_TARGET_X86_64)
else ifeq ($(ARCHS),arm64)
    DEPLOYMENT_TARGET = $(DEPLOYMENT_TARGET_ARM64)
else
    DEPLOYMENT_TARGET = $(DEPLOYMENT_TARGET_X86_64)
    ARCHS ?= x86_64 arm64
endif

DEPLOYMENT_TARGET_X86_64 = 10.15
DEPLOYMENT_TARGET_ARM64 = 11.0

# Directories
BUILDDIR = build/$(BUILD_NAME)
OBJDIR_X86 = $(BUILDDIR)/Objects_x86
OBJDIR_ARM = $(BUILDDIR)/Objects_arm
KEXTDIR = $(BUILDDIR)/$(KEXT_NAME).kext

# Compilation flags
CXX = clang++
CFLAGS = -Wall -Wextra -Wno-unused-parameter $(OPT_FLAGS)
CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c++17 $(OPT_FLAGS)
CPPFLAGS = -DKERNEL -DKERNEL_DEBUG -fno-builtin -fno-common -fapple-kext \
           -DKERNEL_EXTENSION=1 \
           -I$(SYSROOT)/System/Library/Frameworks/Kernel.framework/Headers \
           -I$(SYSROOT)/usr/include/c++/v1 \
           -I$(PWD)/include \
           -I$(PWD)/Lilu/Lilu/Headers

# Framework paths
KEXT_INC = $(SYSROOT)/System/Library/Frameworks/Kernel.framework/Headers

# Default target
all: info $(KEXTDIR)

info:
	@echo "========================================"
	@echo " ForceACL.kext Build System"
	@echo "========================================"
	@echo " Build Type: $(BUILD_NAME)"
	@echo " SDK: $(SYSROOT)"
	@echo " Architectures: $(ARCHS)"
	@echo "========================================"

# Create directories
$(BUILDDIR):
	mkdir -p $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS
	mkdir -p $(BUILDDIR)/$(KEXT_NAME).kext/Contents/Resources

$(OBJDIR_X86):
	mkdir -p $(OBJDIR_X86)

$(OBJDIR_ARM):
	mkdir -p $(OBJDIR_ARM)

# Build for x86_64
build_x86_64: | $(BUILDDIR) $(OBJDIR_X86)
	@echo "Building x86_64..."
	@for src in $(SOURCES); do \
		echo "  Compiling: $$src"; \
		obj="$(OBJDIR_X86)/$$(basename $$src .cpp).o"; \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-target x86_64-apple-macosx$(DEPLOYMENT_TARGET_X86_64) \
			-isysroot $(SYSROOT) \
			-I$(KEXT_INC) \
			-I$(KEXT_INC)/libkern/c++ \
			-c $$src -o $$obj || exit 1; \
	done

# Build for arm64
build_arm64: | $(BUILDDIR) $(OBJDIR_ARM)
	@echo "Building arm64..."
	@for src in $(SOURCES); do \
		echo "  Compiling: $$src"; \
		obj="$(OBJDIR_ARM)/$$(basename $$src .cpp).o"; \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-target arm64-apple-macosx$(DEPLOYMENT_TARGET_ARM64) \
			-isysroot $(SYSROOT) \
			-I$(KEXT_INC) \
			-I$(KEXT_INC)/libkern/c++ \
			-c $$src -o $$obj || exit 1; \
	done

# Link x86_64 - combine all .o files into single executable
link_x86_64: build_x86_64
	@echo "Linking x86_64..."
	@objs=""; \
	for src in $(SOURCES); do \
		obj="$(OBJDIR_X86)/$$(basename $$src .cpp).o"; \
		objs="$$objs $$obj"; \
	done; \
	ld -r $$objs -o $(BUILDDIR)/ForceACL_x86_64 -segsizepkg 0x1000

# Link arm64 - combine all .o files into single executable
link_arm64: build_arm64
	@echo "Linking arm64..."
	@objs=""; \
	for src in $(SOURCES); do \
		obj="$(OBJDIR_ARM)/$$(basename $$src .cpp).o"; \
		objs="$$objs $$obj"; \
	done; \
	ld -r $$objs -o $(BUILDDIR)/ForceACL_arm64 -segsizepkg 0x1000

# Create universal binary from linked arch-specific objects
create_universal: link_x86_64 link_arm64
	@echo "Creating universal binary..."
	@lipo -create $(BUILDDIR)/ForceACL_x86_64 $(BUILDDIR)/ForceACL_arm64 \
		-output $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)

# Create x86_64 only binary
create_x86_64: link_x86_64
	@echo "Creating x86_64 binary..."
	@cp $(BUILDDIR)/ForceACL_x86_64 $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)

# Create arm64 only binary
create_arm64: link_arm64
	@echo "Creating arm64 binary..."
	@cp $(BUILDDIR)/ForceACL_arm64 $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)

# Main build target
$(KEXTDIR): info
ifneq ($(findstring x86_64,$(ARCHS)),)
ifneq ($(findstring arm64,$(ARCHS)),)
	@$(MAKE) create_universal
else
	@$(MAKE) create_x86_64
endif
else
	@$(MAKE) create_arm64
endif
	@cp Info.plist $(KEXTDIR)/Contents/Info.plist
	@echo ""
	@echo "========================================"
	@echo " Build complete!"
	@echo " Output: $(KEXTDIR)"
	@echo "========================================"

# Convenience targets
debug:
	@$(MAKE) BUILD_TYPE=debug all

release:
	@$(MAKE) BUILD_TYPE=release all

x86_64:
	@$(MAKE) ARCHS=x86_64 all

arm64:
	@$(MAKE) ARCHS=arm64 all

clean:
	rm -rf build

distclean:
	rm -rf build*

install: $(KEXTDIR)
	sudo cp -R $(KEXTDIR) /Library/Extensions/
	sudo touch /Library/Extensions
	@echo "Installed. Reboot required."

uninstall:
	sudo rm -rf /Library/Extensions/$(KEXT_NAME).kext
	sudo touch /Library/Extensions

.PHONY: all info build_x86_64 build_arm64 link_x86_64 link_arm64 create_universal create_x86_64 create_arm64 debug release x86_64 arm64 clean distclean install uninstall