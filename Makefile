#
# ForceACL.kext Makefile (FIXED - Kernel Safe)
#

PRODUCT_NAME = ForceACL
KEXT_NAME = ForceACL
LILU_PATH ?= $(PWD)/Lilu

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

CPPFLAGS = -DKERNEL -DKERNEL_DEBUG \
	-fno-common \
	-I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers \
	-I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers/libkern \
	-I$(SDKROOT)/usr/include \
	-I$(PWD)/include \
	-I$(LILU_PATH)/Lilu/Headers

# Additional flags from workflow
OTHER_CFLAGS ?=
OTHER_LDFLAGS ?=

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

$(OBJDIR_X86):
	mkdir -p $(OBJDIR_X86)

$(OBJDIR_ARM):
	mkdir -p $(OBJDIR_ARM)

# =========================
# COMPILATION
# =========================

build_x86_64: | $(OBJDIR_X86)
	@echo "Building x86_64..."
	@for src in $(SOURCES); do \
		obj="$(OBJDIR_X86)/$$(basename $$src .cpp).o"; \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(KEXT_FLAGS) $(OTHER_CFLAGS) \
			-target x86_64-apple-macosx$(DEPLOYMENT_TARGET_X86_64) \
			-isysroot $(SDKROOT) \
			-c $$src -o $$obj || exit 1; \
	done

build_arm64: | $(OBJDIR_ARM)
	@echo "Building arm64..."
	@for src in $(SOURCES); do \
		obj="$(OBJDIR_ARM)/$$(basename $$src .cpp).o"; \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(KEXT_FLAGS) $(OTHER_CFLAGS) \
			-target arm64-apple-macosx$(DEPLOYMENT_TARGET_ARM64) \
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
