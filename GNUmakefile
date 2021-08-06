.PHONY: all build test install check clean distclean
all: build
	ninja -C $< $@

test: all
	cd build && ctest --verbose --timeout 25 # --output-on-failure

install: test
	ninja -C build $@

build:
	mkdir -p $@
	cmake -B $@ -S . -G Ninja -D SNMP_PP_OPENSSL=YES -D CMAKE_CXX_COMPILER_LAUNCHER=ccache

check: build/compile_commands.json
	# run-clang-tidy.py -p build -checks='-*,cppcoreguidelines-init-variables' -j1 -fix src
	# run-clang-tidy.py -p build -checks='-*,cppcoreguidelines-explicit-virtual-functions' -j1 -fix src
	run-clang-tidy.py -p build -checks='-clang-analyzer-optin.*,-cert-dcl37-c,-cert-dcl51-cpp' src/*.cpp

clean: build
	rm -f include/agent_pp/agent++.h
	rm -f $</*.h
	-ninja -C $< clean

distclean: clean
	rm -rf build
#
