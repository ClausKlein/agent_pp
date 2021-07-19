.PHONY: all build install clean distclean
all: build
	ninja -C $< $@

install: all
	ninja -C build $@

build:
	mkdir -p $@
	cmake -B $@ -S . -G Ninja -D OPTION_PROXY=NO -D OPTION_PROXY_FORWARDER=NO -D OPTION_EXAMPLES=NO -D OPTION_TOOLS=YES

clean: build
	rm -f include/agent_pp/agent++.h
	-ninja -C $< clean

distclean: clean
	rm -rf build
#
