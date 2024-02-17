EXEC = tonetrace

CLIB = -I./lib/libsndfile-1.2.2/include -pthread

$(EXEC): main.cpp
	g++ -o $@ $^ $(CLIB)

install-deps:
	mkdir -p lib
	curl -L https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2.tar.xz | tar -xvf - -C lib --use-compress-program xz
	curl -L https://fftw.org/fftw-3.3.10.tar.gz | tar -xzvf - -C lib
	
	# Build libsndfile
	cd lib/libsndfile-1.2.2 && \
	cmake . && \
	make
	
	# Build FFTW
	cd lib/fftw-3.3.10 && \
	./configure && \
	make

.PHONY: install-deps

uninstall-deps:
	cd lib/portaudio && $(MAKE) uninstall
	rm -rf lib/portaudio
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC)
.PHONY: clean