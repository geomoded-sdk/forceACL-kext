#
# ForceACL.kext Makefile
# Lilu plugin for Intel GPU injection with AI decision engine
#

# Configuration
PRODUCT_NAME = ForceWACL
KEXT_NAME = ForceACL
LILU_PATH ?= $(HOME)/Lilu
DEVELOPER = $(shell xcode-select -p)

# Source files
SOURCES = \
	source/plugin_main.cpp \
	source/ForceACL.cpp \
	source/AIDecisionEngine.cpp \
	source/CommunityKnowledgeBase.cpp \
	source/GPUDetector.cpp \
	source/PlatformDatabase.cpp

# Headers
HEADERS = \
	include/ForceACL/ForceACL.hpp \
	include/ForceACL/PlatformDatabase.hpp \
	include/ForceACL/GPUDetector.hpp \
	include/ForceACL/GPUInjector.hpp \
	include/ForceACL/NVRAMManager.hpp \
	include/ForceACL/AIDecisionEngine.hpp \
	include/ForceACL/ErrorHandler.hpp \
	include/ForceACL/HookManager.hpp

# Build settings
SYSROOT ?= $(DEVELOPER)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
ARCHS = x86_64 arm64
VALID_ARCHS = x86_64 arm64

# Compilation flags
CFLAGS = -Os -Wall -Wextra -Wno-unused-parameter -MMD -MP
CXXFLAGS = -Os -Wall -Wextra -Wno-unused-parameter -std=c++17 -MMD -MP
CPPFLAGS = -DKERNEL -DKERNEL_DEBUG -D__i386__ -D__x86_64__ -fno-builtin -fno-common -fapple-kext

# Framework paths
KEXT_INC = $(SYSROOT)/System/Library/Frameworks/Kernel.framework/Headers
LILU_INC = $(LILU_PATH)/include
PROJECT_INC = $(PWD)/include

# Target
TARGET = $(PRODUCT_NAME).kext
BUILDDIR = build

# Default target
all: $(TARGET)

# Create directories
$(BUILDDIR):
	mkdir -p $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS
	mkdir -p $(BUILDDIR)/$(KEXT_NAME).kext/Contents/Resources

# Deployment targets per architecture
DEPLOYMENT_TARGET_X86_64 = 10.15
DEPLOYMENT_TARGET_ARM64 = 11.0

# Removed pattern rule - all compilation handled by $(TARGET) rule

# Build kext - compile each architecture separately then combine
$(TARGET): $(BUILDDIR)
	@echo "Building ForceACL.kext..."
	@for arch in $(ARCHS); do \
		target_triple=""; \
		deploy_target=""; \
		if [ "$$arch" = "x86_64" ]; then \
			target_triple="x86_64-apple-macosx$(DEPLOYMENT_TARGET_X86_64)"; \
		elif [ "$$arch" = "arm64" ]; then \
			target_triple="arm64-apple-macosx$(DEPLOYMENT_TARGET_ARM64)"; \
		fi; \
		echo "  Building for $$arch (target: $$target_triple)..."; \
		for src in $(SOURCES); do \
			obj="$(BUILDDIR)/$$(basename $$src .cpp)_$${arch}.o"; \
			echo "    Compiling $$src..."; \
			$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
				-I$(KEXT_INC) -I$(LILU_INC) -I$(PROJECT_INC) \
				-target $$target_triple \
				-isysroot $(SYSROOT) \
				-c $$src -o $$obj || exit 1; \
		done; \
	done
	@echo "  Linking..."
	@mkdir -p $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS
	@for arch in $(ARCHS); do \
		echo "  Linking $$arch..."; \
		objs=""; \
		for src in $(SOURCES); do \
			obj="$(BUILDDIR)/$$(basename $$src .cpp)_$${arch}.o"; \
			if [ -f "$$obj" ]; then \
				objs="$$objs $$obj"; \
			fi; \
		done; \
		if [ -n "$$objs" ]; then \
			ld -r -arch $$arch $$objs -o $(BUILDDIR)/ForceACL_$${arch}_linked.o; \
		fi; \
	done
	@echo "  Creating universal binary..."
	@if [ -f $(BUILDDIR)/ForceACL_x86_64_linked.o ] && [ -f $(BUILDDIR)/ForceACL_arm64_linked.o ]; then \
		lipo -create $(BUILDDIR)/ForceACL_x86_64_linked.o $(BUILDDIR)/ForceACL_arm64_linked.o \
			-output $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
	elif [ -f $(BUILDDIR)/ForceACL_x86_64_linked.o ]; then \
		cp $(BUILDDIR)/ForceACL_x86_64_linked.o $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
	elif [ -f $(BUILDDIR)/ForceACL_arm64_linked.o ]; then \
		cp $(BUILDDIR)/ForceACL_arm64_linked.o $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
	fi
	@cp Info.plist $(BUILDDIR)/$(KEXT_NAME).kext/Contents/Info.plist
	@echo "Build complete: $(BUILDDIR)/$(KEXT_NAME).kext"

clean:
	rm -rf $(BUILDDIR)

clean-deps:
	find . -name "*.d" -delete

format:
	clang-format -i source/*.cpp include/ForceACL/*.hpp

install: $(TARGET)
	sudo cp -R $(BUILDDIR)/$(KEXT_NAME).kext /Library/Extensions/
	sudo touch /Library/Extensions

.PHONY: all clean clean-deps format install
