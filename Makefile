default: all

%:
	make -C engine $@
	make -C game $@
