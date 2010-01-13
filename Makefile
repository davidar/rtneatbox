all:
	git submodule update --init
	cd thirdparty/librtneat && $(MAKE) all
	cd src && $(MAKE) all

clean:
	cd thirdparty/librtneat && $(MAKE) clean
	cd src && $(MAKE) clean
