# Makefile for cross-compilation

.PHONY: all arm x86_64 clean clean-arm clean-x86_64

all: arm x86_64

arm:
	@echo "Building ARM version..."
	@mkdir -p build_arm
	@cd build_arm && cmake .. -DCMAKE_BUILD_TYPE=Release -DTARGET_ARCH=arm
	@cd build_arm && $(MAKE) -j$$(nproc)
	@echo "ARM build completed: build_arm/libs/arm/libjsapi_langningchen.so"

x86_64:
	@echo "Building x86_64 version..."
	@mkdir -p build_x86_64
	@cd build_x86_64 && cmake .. -DCMAKE_BUILD_TYPE=Release -DTARGET_ARCH=x86_64
	@cd build_x86_64 && $(MAKE) -j$$(nproc)
	@echo "x86_64 build completed: build_x86_64/libs/x86_64/libjsapi_langningchen.so"

clean: clean-arm clean-x86_64

clean-arm:
	@echo "Cleaning ARM build..."
	@rm -rf build_arm

clean-x86_64:
	@echo "Cleaning x86_64 build..."
	@rm -rf build_x86_64

# Debug builds
arm-debug:
	@echo "Building ARM debug version..."
	@mkdir -p build_arm_debug
	@cd build_arm_debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DTARGET_ARCH=arm
	@cd build_arm_debug && $(MAKE) -j$$(nproc)
	@cd build_arm_debug && $(MAKE) install

x86_64-debug:
	@echo "Building x86_64 debug version..."
	@mkdir -p build_x86_64_debug
	@cd build_x86_64_debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DTARGET_ARCH=x86_64
	@cd build_x86_64_debug && $(MAKE) -j$$(nproc)
	@cd build_x86_64_debug && $(MAKE) install
