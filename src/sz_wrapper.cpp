#include "SZ3/api/sz.hpp"
#include <vector>
#include <iostream>

extern "C" {
    
    void compress_decompress_sz(double* data, int N, double error_bound) {
        
        SZ3::Config conf;
        conf.absErrorBound = error_bound;
        // On force 1D pour être sûr que ça passe sans assertion
        conf.dims = {static_cast<size_t>(N * N)}; 
        conf.num = N * N;
        conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO; // Algo par défaut robuste

        size_t outSize = 0;
        
        // Appel standard (le compilateur déduit les types)
        char* compressedData = SZ_compress(conf, data, outSize);
        
        // Décompression
        double* decData = SZ_decompress<double>(conf, compressedData, outSize);
        
        // Recopie
        for(size_t i=0; i<conf.num; i++) {
            data[i] = decData[i];
        }
        
        delete[] compressedData;
        delete[] decData;
    }
}
