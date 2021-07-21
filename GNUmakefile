.PHONY: all build install clean distclean
all: build
	ninja -C $< $@

install: all
	ninja -C build $@

build:
	mkdir -p $@
	cmake -B $@ -S . -G Ninja -D CMAKE_EXPORT_COMPILE_COMMANDS=1 -D CMAKE_CXX_COMPILER_LAUNCHER=ccache

clean: build
	rm -f include/agent_pp/agent++.h
	rm -f $</*.h
	-ninja -C $< clean

distclean: clean
	rm -rf build
#
