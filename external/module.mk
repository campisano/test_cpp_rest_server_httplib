.PHONY: all
all:
	mkdir -p build
	cd build && cmake ..
	$(MAKE) -C build

.PHONY: clean
clean:
	test ! -d build && true || $(MAKE) -C build clean
