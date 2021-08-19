BUILD_TYPE?=Debug

export CMAKE_BUILD_TYPE=$(BUILD_TYPE)
export CPM_USE_LOCAL_PACKAGES=0
export CPM_SOURCE_CACHE=${HOME}/.cache/CPM

MACHINE:=$(shell uname -m)
PROJECT_NAME:=$(shell basename $(CURDIR))

MACHINE:=$(shell uname -m)
PROJECT_NAME:=$(shell basename $(CURDIR))
BUILD_DIR?=../build-$(PROJECT_NAME)-$(MACHINE)-$(BUILD_TYPE)


.PHONY: all build test install check clean distclean format
all: build
	ninja -C $(BUILD_DIR) $@

test: all
	cd $(BUILD_DIR) && ctest --verbose --timeout 25 # --output-on-failure

install: test
	ninja -C $(BUILD_DIR) $@

build: $(BUILD_DIR)
build: $(BUILD_DIR)/compile_commands.json
$(BUILD_DIR)/compile_commands.json:
	cmake -B $(BUILD_DIR) -S . -G Ninja -D CMAKE_CXX_COMPILER_LAUNCHER=ccache
	perl -i.bak -p -e 's#-W[-\w]+\b##g;' -e 's#-I(${CPM_SOURCE_CACHE})#-isystem $$1#g;' $(BUILD_DIR)/compile_commands.json

$(BUILD_DIR):
	mkdir -p $@

check: $(BUILD_DIR)/compile_commands.json
	# run-clang-tidy.py -p $(BUILD_DIR) -checks='-*,cppcoreguidelines-init-variables' -j1 -fix src
	# clang-tidy -p $(BUILD_DIR) --checks='-*,cppcoreguidelines-explicit-virtual-functions' --fix include/agent_pp/*.h
	run-clang-tidy.py -p $(BUILD_DIR) -checks='-clang-analyzer-optin.*' src/*.cpp examples

clean:
	rm -f include/agent_pp/agent++.h
	rm -f $(BUILD_DIR)/compile_commands.json
	rm -f $(BUILD_DIR)/*.h
	-ninja -C $(BUILD_DIR) clean

distclean: clean
	rm -rf $(BUILD_DIR) build

format: distclean
	find . -name CMakeLists.txt | xargs cmake-format -i
	find . -type f -name '*.cmake' | xargs cmake-format -i
	find . -type f -name '*.cpp' | xargs clang-format -i
	find . -type f -name '*.h' | xargs clang-format -i
	find . -type f \( -name '*.cpp' -o -name '*.h' \) | xargs grep  --color '\/\/ BEGIN=' || echo OK

