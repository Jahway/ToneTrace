EXEC = tonetrace
SRCDIR = src
BINDIR = bin
OUTDIR = output
INSTALL_DIR = $(abspath .)/lib

INCS += -I$(INSTALL_DIR)/fftw-install/include -I$(INSTALL_DIR)/libsndfile-install/include
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L$(INSTALL_DIR)/fftw-install/lib -L$(INSTALL_DIR)/libsndfile-install/lib64
LIBS += -lsndfile -lfftw3

all: $(BINDIR)/$(EXEC)

$(BINDIR)/$(EXEC): $(SRCDIR)/main.cpp
	mkdir -p $(BINDIR)
	g++ $(CXXFLAGS) -o $@ $^ $(INCS) $(LDFLAGS) $(LIBS)

inst_dep: 
	mkdir -p $(INSTALL_DIR)/libsndfile-install $(INSTALL_DIR)/fftw-install
	curl -L http://web.eecs.utk.edu/~jplank/plank/jgraph/2024-02-15-Jgraph.tar | tar -xvf - -C lib
	curl -L https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2.tar.xz | xzcat | tar -xvf - -C lib
	curl -L https://fftw.org/fftw-3.3.10.tar.gz | tar -xzvf - -C lib

# Build jgraph
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
	./configure --prefix=$(INSTALL_DIR)/fftw-install --enable-shared=yes --enable-type-prefix && \
	make && \
	make install

.PHONY: inst_dep

uninst_dep:
	rm -rf $(INSTALL_DIR)

.PHONY: uninst_dep

pull_audio:
	mkdir -p audio
	wget -r -nd -A.wav -P audio https://www2.cs.uic.edu/~i101/SoundFiles/
	rm -f audio/CantinaBand3.wav audio/taunt.wav audio/StarWars3.wav
.PHONY: pull_audio

clean:
	rm -rf $(BINDIR)
	rm -rf $(OUTDIR)
	rm -r $(SRCDIR)/*.jgr
.PHONY: clean