#include "utils.h"
#include "zfp.h"
#include <math.h>

void compress_decompress_zfp(double* data, int N, double tolerance) {
    zfp_type type = zfp_type_double;
    zfp_field* field = zfp_field_2d(data, type, N, N);
    zfp_stream* zfp = zfp_stream_open(NULL);
    zfp_stream_set_accuracy(zfp, tolerance);
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    void* buffer = malloc(bufsize);
    bitstream* stream = stream_open(buffer, bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
    zfp_compress(zfp, field);
    zfp_stream_rewind(zfp);
    zfp_decompress(zfp, field);
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
    free(buffer);
}

void cholesky(double *A, double *L, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            for (int k = 0; k < j; k++) {
                sum += L[i * N + k] * L[j * N + k];
            }
            if (i == j) {
                double val = A[i * N + i] - sum;
                if (val <= 0) val = 1.0; 
                L[i * N + j] = sqrt(val);
            } else {
                L[i * N + j] = (1.0 / L[j * N + j]) * (A[i * N + j] - sum);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    int N = atoi(argv[1]);

    double *A = (double *)malloc(N * N * sizeof(double));
    double *L = (double *)calloc(N * N, sizeof(double));

    // Chargement SDRBench + Transformation SDP
    init_matrix_from_file(A, N);
    make_symmetric_positive(A, N);

    double start = get_time();
    
    compress_decompress_zfp(A, N, 1e-4); // ZFP avant calcul
    cholesky(A, L, N);
    
    double end = get_time();
    printf("Algo:Cholesky_ZFP Size:%d Time:%.6f\n", N, end - start);

    free(A); free(L);
    return 0;
}
