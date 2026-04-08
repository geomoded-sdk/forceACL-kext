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

# Build object files
$(BUILDDIR)/%.o: source/%.cpp $(HEADERS) $(BUILDDIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
		-I$(KEXT_INC) \
		-I$(LILU_INC) \
		-I$(PROJECT_INC) \
		-target kext \
		-isysroot $(SYSROOT) \
		-c $< -o $@

# Link all objects
$(BUILDDIR)/$(PRODUCT_NAME)_all.o: $(SOURCES:source/%.cpp=$(BUILDDIR)/%.o) $(BUILDDIR)
	@echo "Building ForceACL.kext..."

# Build kext
$(TARGET): $(SOURCES:source/%.cpp=$(BUILDDIR)/%.o) Info.plist $(BUILDDIR)
	@echo "Building ForceACL.kext..."
	@for arch in $(ARCHS); do \
		echo "  Building for $$arch..."; \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-I$(KEXT_INC) -I$(LILU_INC) -I$(PROJECT_INC) \
			-target kext$${arch#x86_64} \
			-isysroot $(SYSROOT) \
			-c source/plugin_main.cpp -o $(BUILDDIR)/plugin_main_$${arch}.o && \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-I$(KEXT_INC) -I$(LILU_INC) -I$(PROJECT_INC) \
			-target kext$${arch#x86_64} \
			-isysroot $(SYSROOT) \
			-c source/ForceACL.cpp -o $(BUILDDIR)/ForceACL_$${arch}.o && \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-I$(KEXT_INC) -I$(LILU_INC) -I$(PROJECT_INC) \
			-target kext$${arch#x86_64} \
			-isysroot $(SYSROOT) \
			-c source/GPUDetector.cpp -o $(BUILDDIR)/GPUDetector_$${arch}.o && \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) \
			-I$(KEXT_INC) -I$(LILU_INC) -I$(PROJECT_INC) \
			-target kext$${arch#x86_64} \
			-isysroot $(SYSROOT) \
			-c source/PlatformDatabase.cpp -o $(BUILDDIR)/PlatformDatabase_$${arch}.o; \
	done
	@echo "  Linking..."
	@if [ -f $(BUILDDIR)/plugin_main_x86_64.o ] && [ -f $(BUILDDIR)/plugin_main_arm64.o ]; then \
		lipo -create \
			$(BUILDDIR)/plugin_main_x86_64.o \
			$(BUILDDIR)/ForceACL_x86_64.o \
			$(BUILDDIR)/GPUDetector_x86_64.o \
			$(BUILDDIR)/PlatformDatabase_x86_64.o \
			$(BUILDDIR)/plugin_main_arm64.o \
			$(BUILDDIR)/ForceACL_arm64.o \
			$(BUILDDIR)/GPUDetector_arm64.o \
			$(BUILDDIR)/PlatformDatabase_arm64.o \
			-output $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
	elif [ -f $(BUILDDIR)/plugin_main_x86_64.o ]; then \
		lipo -create \
			$(BUILDDIR)/plugin_main_x86_64.o \
			$(BUILDDIR)/ForceACL_x86_64.o \
			$(BUILDDIR)/GPUDetector_x86_64.o \
			$(BUILDDIR)/PlatformDatabase_x86_64.o \
			-output $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
	elif [ -f $(BUILDDIR)/plugin_main_arm64.o ]; then \
		lipo -create \
			$(BUILDDIR)/plugin_main_arm64.o \
			$(BUILDDIR)/ForceACL_arm64.o \
			$(BUILDDIR)/GPUDetector_arm64.o \
			$(BUILDDIR)/PlatformDatabase_arm64.o \
			-output $(BUILDDIR)/$(KEXT_NAME).kext/Contents/MacOS/$(PRODUCT_NAME); \
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
