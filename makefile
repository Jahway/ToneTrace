EXEC = tonetrace
SRCDIR = src
BINDIR = bin
# INSTALL_DIR = $(HOME)/local
INSTALL_DIR = $(abspath .)/lib
# CLIB = -I$(HOME)/local/include -L$(HOME)/local/lib -Ilib/libsndfile-1.2.2/include -Llib/libsndfile-1.2.2/CMakeBuild -pthread 

INCS 			+= -I$(INSTALL_DIR)/fftw-install/include -I$(INSTALL_DIR)/libsndfile-install/include
CFLAGS			+= -L$(INSTALL_DIR)/fftw-install/lib -L$(INSTALL_DIR)/libsndfile-install/lib64
LIBS 			+= -lsndfile -lfftw3

all: $(BINDIR)/$(EXEC)

$(BINDIR)/$(EXEC): $(SRCDIR)/main.cpp
	mkdir -p $(BINDIR)
	g++ -o $@ $^ $(INCS) $(CFLAGS) $(LIBS)

dep_inst:
	mkdir -p lib/libsndfile-install lib/fftw-install
	curl -L http://web.eecs.utk.edu/~jplank/plank/jgraph/2024-02-15-Jgraph.tar | tar -xvf - -C lib
	curl -L https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2.tar.xz | xzcat | tar -xvf - -C lib
	curl -L https://fftw.org/fftw-3.3.10.tar.gz | tar -xzvf - -C lib

	cd lib/jgraph/jgraph && \
	make

# Build libsndfile
	cd lib/libsndfile-1.2.2 && \
	mkdir -p build && \
	cd build && \
	cmake -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR)/libsndfile-install .. && \
	make && \
	make install

# Build FFTW
	cd lib/fftw-3.3.10 && \
	./configure --prefix=$(INSTALL_DIR)/fftw-install --enable-shared=yes --with-gcc --enable-type-prefix && \
	make && \
	make install

.PHONY: dep_inst

uninstall-deps:
	rm -rf lib

.PHONY: uninstall-deps

pull_audio:
	mkdir -p audio
	wget -r -nd -A.wav -P audio https://www2.cs.uic.edu/~i101/SoundFiles/

.PHONY: pull_audio

clean:
	rm -f $(BINDIR)/$(EXEC)
	rm -f $(BINDIR)/*.wav

.PHONY: clean
