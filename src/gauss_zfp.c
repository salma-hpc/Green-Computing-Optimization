#include "utils.h"
#include "zfp.h"

// Fonction pour compresser/décompresser un tableau 2D (Matrice)
void apply_zfp_2d(double *data, int N, double rate) {
    zfp_type type = zfp_type_double;
    zfp_field* field = zfp_field_2d(data, type, N, N);
    zfp_stream* zfp = zfp_stream_open(NULL);
    zfp_stream_set_rate(zfp, rate, type, 2, 0); 
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    void* buffer = malloc(bufsize);
    bitstream* stream = stream_open(buffer, bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
    zfp_compress(zfp, field);
    zfp_stream_rewind(zfp);
    zfp_decompress(zfp, field);
    free(buffer);
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
}

// Fonction pour compresser/décompresser un tableau 1D (Vecteur)
void apply_zfp_1d(double *data, int N, double rate) {
    zfp_type type = zfp_type_double;
    zfp_field* field = zfp_field_1d(data, type, N);
    zfp_stream* zfp = zfp_stream_open(NULL);
    zfp_stream_set_rate(zfp, rate, type, 1, 0); 
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    void* buffer = malloc(bufsize);
    bitstream* stream = stream_open(buffer, bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
    zfp_compress(zfp, field);
    zfp_stream_rewind(zfp);
    zfp_decompress(zfp, field);
    free(buffer);
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
}

void gauss_solve(double *A, double *b, double *x, int N) {
    // Élimination
    for (int k = 0; k < N - 1; k++) {
        for (int i = k + 1; i < N; i++) {
            double factor = A[i * N + k] / A[k * N + k];
            for (int j = k; j < N; j++) {
                A[i * N + j] -= factor * A[k * N + j];
            }
            b[i] -= factor * b[k];
        }
    }
    // Remontée
    for (int i = N - 1; i >= 0; i--) {
        double sum = b[i];
        for (int j = i + 1; j < N; j++) {
            sum -= A[i * N + j] * x[j];
        }
        x[i] = sum / A[i * N + i];
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    int N = atoi(argv[1]);

    double *A = (double *)malloc(N * N * sizeof(double));
    double *b = (double *)malloc(N * sizeof(double));
    double *x = (double *)malloc(N * sizeof(double));
    double *x_ref = (double *)malloc(N * sizeof(double));

    init_matrix(A, N); 
    for(int i=0; i<N; i++) x_ref[i] = 1.0;

    // Génération b
    for(int i=0; i<N; i++) {
        b[i] = 0.0;
        for(int j=0; j<N; j++) b[i] += A[i*N + j] * x_ref[j];
    }

    double start = get_time();
    
    // COMPRESSION ZFP (Taux fixe 16 bits)
    // On altère A et b avant le calcul
    apply_zfp_2d(A, N, 16.0);
    apply_zfp_1d(b, N, 16.0);
    
    gauss_solve(A, b, x, N);
    
    double end = get_time();
    printf("Algo:Gauss_ZFP Size:%d Time:%.6f\n", N, end - start);

    free(A); free(b); free(x); free(x_ref);
    return 0;
}
