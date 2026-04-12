#
# ForceACL.kext Makefile (FIXED - Kernel Safe)
#

PRODUCT_NAME = ForceACL
MODULE_VERSION = 1.0.0
KEXT_NAME = ForceACL
LILU_PATH ?= $(PWD)/Lilu

# Use Lilu headers from submodule directly
LILU_HEADERS_PATH := $(PWD)/Lilu/Lilu

# Build type
BUILD_TYPE ?= debug

ifeq ($(BUILD_TYPE),release)
    OPT_FLAGS = -O2
    VERSION_SUFFIX =
    BUILD_NAME = Release
else
    OPT_FLAGS = -g -O0
    VERSION_SUFFIX = -debug
    BUILD_NAME = Debug
endif

# Sources
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

# SDK (CORRIGIDO)
SDKROOT := $(shell xcrun --sdk macosx --show-sdk-path)

# Deployment targets
DEPLOYMENT_TARGET_X86_64 = 10.15
DEPLOYMENT_TARGET_ARM64 = 11.0

# Archs
ARCHS ?= arm64 x86_64

# Output
BUILDDIR = build/$(BUILD_NAME)
OBJDIR_X86 = $(BUILDDIR)/obj_x86
OBJDIR_ARM = $(BUILDDIR)/obj_arm
KEXTDIR = $(BUILDDIR)/$(KEXT_NAME).kext

# Compiler
CXX = clang++

# 💣 FIX PRINCIPAL (KERNEL MODE FLAGS)
KEXT_FLAGS = -mkernel -nostdlib -fno-builtin -fno-stack-protector

CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c++17 $(OPT_FLAGS)

CPPFLAGS = -DKERNEL -DKERNEL_DEBUG -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT -D__ACIDANTHERA_MAC_SDK -DLILU_SUPPORTS_1_7=1 \
	-DPRODUCT_NAME=$(PRODUCT_NAME) \
	-DMODULE_VERSION=$(MODULE_VERSION) \
	-fno-common \
	-I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers \
	-I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers/libkern \
	-I$(SDKROOT)/usr/include \
	-I$(PWD)/include \
	-I$(LILU_HEADERS_PATH) \
	-I$(LILU_HEADERS_PATH)/PrivateHeaders \
	-I$(PWD)/include/ForceACL \
	-I$(PWD)/Lilu/Lilu/Headers \
	-I$(PWD)/Lilu/Lilu/PrivateHeaders \
	-DLILU_SUPPORTS_1_7=1

# Additional flags from workflow
OTHER_CFLAGS ?= -fapple-kext
OTHER_LDFLAGS ?= -fapple-kext

# Default
all: info $(KEXTDIR)

info:
	@echo "========================================"
	@echo " ForceACL.kext (FIXED BUILD)"
	@echo "========================================"
	@echo " Build: $(BUILD_NAME)"
	@echo " SDK: $(SDKROOT)"
	@echo " Archs: $(ARCHS)"
	@echo "========================================"

# Directories
$(BUILDDIR):
	mkdir -p $(KEXTDIR)/Contents/MacOS
	mkdir -p $(KEXTDIR)/Contents/Resources

.PHONY: create_universal

create_universal: link_x86_64 link_arm64
	@echo "Creating universal binary..."
	@echo "x86: $(BUILDDIR)/ForceACL_x86_64"
	@echo "arm: $(BUILDDIR)/ForceACL_arm64"
	@echo "out: $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)"
	@lipo -create \
		$(BUILDDIR)/ForceACL_x86_64 \
		$(BUILDDIR)/ForceACL_arm64 \
		-output $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)

$(OBJDIR_X86):
	mkdir -p $(OBJDIR_X86)

$(OBJDIR_ARM):
	mkdir -p $(OBJDIR_ARM)
	mkdir -p $(OBJDIR_X86)

$(OBJDIR_ARM):
	mkdir -p $(OBJDIR_ARM)

# =========================
# COMPILATION
# =========================

build_x86_64: | $(OBJDIR_X86)
	@echo "Building x86_64..."
	@test -f "$(LILU_HEADERS_PATH)/Headers/plugin_start.hpp" || echo "ERROR: plugin_start.hpp not found!"
	clang++ -E -D__test__=1 -c source/plugin_main.cpp -o /dev/null 2>&1 | head -1 || true
	@for src in $(SOURCES); do \
		obj="$(OBJDIR_X86)/$$(basename $$src .cpp).o"; \
		echo ">>> Compiling $$src with -D__x86_64__"; \
		clang++ -Wall -Wextra -Wno-unused-parameter -std=c++17 -g -O0 \
			-target x86_64-apple-macos10.6 \
			-DKERNEL -DKERNEL_DEBUG -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT -D__ACIDANTHERA_MAC_SDK \
			-DLILU_SUPPORTS_1_7=1 -DPRODUCT_NAME=ForceACL -DMODULE_VERSION=1.0.0 \
			-fno-common -fapple-kext \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/System/Library/Frameworks/Kernel.framework/Headers \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/System/Library/Frameworks/Kernel.framework/Headers/libkern \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/usr/include \
			-Iinclude \
			-I$(LILU_HEADERS_PATH) \
			-I$(LILU_HEADERS_PATH)/Headers \
			-I$(LILU_HEADERS_PATH)/PrivateHeaders \
			-Iinclude/ForceACL \
			-D__x86_64__=1 \
			-mkernel -nostdlib -fno-builtin -fno-stack-protector \
			-isysroot $(SDKROOT) \
			-c $$src -o $$obj || exit 1; \
	done

build_arm64: | $(OBJDIR_ARM)
	@echo "Building arm64..."
	@for src in $(SOURCES); do \
		obj="$(OBJDIR_ARM)/$$(basename $$src .cpp).o"; \
		clang++ -Wall -Wextra -Wno-unused-parameter -std=c++17 -g -O0 \
			-target arm64-apple-macos11.0 \
			-DKERNEL -DKERNEL_DEBUG -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT -D__ACIDANTHERA_MAC_SDK \
			-DLILU_SUPPORTS_1_7=1 -DPRODUCT_NAME=ForceACL -DMODULE_VERSION=1.0.0 \
			-fno-common -fapple-kext \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/System/Library/Frameworks/Kernel.framework/Headers \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/System/Library/Frameworks/Kernel.framework/Headers/libkern \
			-I/Applications/Xcode_16.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.5.sdk/usr/include \
			-Iinclude \
			-I$(LILU_HEADERS_PATH) \
			-I$(LILU_HEADERS_PATH)/Headers \
			-I$(LILU_HEADERS_PATH)/PrivateHeaders \
			-Iinclude/ForceACL \
			-D__arm64__ \
			-mkernel -nostdlib -fno-builtin -fno-stack-protector \
			-isysroot $(SDKROOT) \
			-c $$src -o $$obj || exit 1; \
	done

# =========================
# LINKING (FIXED CRT0 ISSUE)
# =========================

link_x86_64: build_x86_64
	@echo "Linking x86_64..."
	@objs=""; \
	for src in $(SOURCES); do \
		objs="$$objs $(OBJDIR_X86)/$$(basename $$src .cpp).o"; \
	done; \
	$(CXX) \
		-target x86_64-apple-macosx$(DEPLOYMENT_TARGET_X86_64) \
		-isysroot $(SDKROOT) \
		-nostdlib \
		-Wl,-undefined,dynamic_lookup \
		$(OTHER_LDFLAGS) \
		-o $(BUILDDIR)/ForceACL_x86_64 $$objs

link_arm64: build_arm64
	@echo "Linking arm64..."
	@objs=""; \
	for src in $(SOURCES); do \
		objs="$$objs $(OBJDIR_ARM)/$$(basename $$src .cpp).o"; \
	done; \
	$(CXX) \
		-target arm64-apple-macosx$(DEPLOYMENT_TARGET_ARM64) \
		-isysroot $(SDKROOT) \
		-nostdlib \
		-Wl,-undefined,dynamic_lookup \
		$(OTHER_LDFLAGS) \
		-o $(BUILDDIR)/ForceACL_arm64 $$objs

# =========================
# UNIVERSAL
# =========================

create_universal: link_x86_64 link_arm64
	@echo "Creating universal binary..."
	@lipo -create \
		$(BUILDDIR)/ForceACL_x86_64 \
		$(BUILDDIR)/ForceACL_arm64 \
		-output $(KEXTDIR)/Contents/MacOS/$(PRODUCT_NAME)

# =========================
# FINAL BUILD
# =========================

$(KEXTDIR): $(BUILDDIR)
	@$(MAKE) create_universal
	@cp Info.plist $(KEXTDIR)/Contents/Info.plist
	@echo ""
	@echo "========================================"
	@echo " BUILD SUCCESS (CRT0 FIXED)"
	@echo " Output: $(KEXTDIR)"
	@echo "========================================"

# =========================
# SHORTCUTS
# =========================

debug:
	@$(MAKE) BUILD_TYPE=debug all

release:
	@$(MAKE) BUILD_TYPE=release all

clean:
	rm -rf build

.PHONY: all debug release clean build_x86_64 build_arm64 link_x86_64 link_arm64 create_universal
