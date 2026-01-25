CC = gcc
CXX = g++
# CFLAGS pour le C (ZFP)
CFLAGS = -O3 -Wall -I../zfp/include -I../SZ3/include
# CXXFLAGS pour le C++ (SZ3) -> On force C++17 ici
CXXFLAGS = -O3 -Wall -I../SZ3/include -std=c++17

# Chemins des libs
LDFLAGS_ZFP = -L../zfp/lib -lzfp -lm
# Pour SZ: on inclut zstd et on lie la librairie C++
LDFLAGS_SZ = -L../SZ3/build/lib -lSZ3c -lstdc++ -lm -lzstd

all: matmul_plain gauss_plain cholesky_plain matmul_zfp gauss_zfp cholesky_zfp matmul_sz

# --- VERSIONS PLAIN ---
matmul_plain: matmul_plain.c utils.h
	$(CC) $(CFLAGS) -o matmul_plain matmul_plain.c -lm

gauss_plain: gauss_plain.c utils.h
	$(CC) $(CFLAGS) -o gauss_plain gauss_plain.c -lm

cholesky_plain: cholesky_plain.c utils.h
	$(CC) $(CFLAGS) -o cholesky_plain cholesky_plain.c -lm

# --- VERSIONS ZFP ---
matmul_zfp: matmul_zfp.c utils.h
	$(CC) $(CFLAGS) -o matmul_zfp matmul_zfp.c $(LDFLAGS_ZFP)

gauss_zfp: gauss_zfp.c utils.h
	$(CC) $(CFLAGS) -o gauss_zfp gauss_zfp.c $(LDFLAGS_ZFP)

cholesky_zfp: cholesky_zfp.c utils.h
	$(CC) $(CFLAGS) -o cholesky_zfp cholesky_zfp.c $(LDFLAGS_ZFP)

# --- VERSION SZ (C++) ---
sz_wrapper.o: sz_wrapper.cpp
	$(CXX) $(CXXFLAGS) -c sz_wrapper.cpp -o sz_wrapper.o

matmul_sz: matmul_sz.c sz_wrapper.o utils.h
	$(CC) $(CFLAGS) -o matmul_sz matmul_sz.c sz_wrapper.o $(LDFLAGS_SZ)

clean:
	rm -f *_plain *_zfp *_sz *.o
