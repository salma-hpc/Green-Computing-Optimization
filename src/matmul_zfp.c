#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "zfp.h"
#include "bitstream.h" // <--- Indispensable pour stream_open

// --- Fonctions utilitaires ---
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void allocate_matrix(double **M, int n) {
    *M = (double *)malloc(n * n * sizeof(double));
}

void free_matrix(double *M) {
    free(M);
}

// Initialisation (SDRBench ou Aléatoire)
void init_matrix(double *M, int n) {
    char filename[100];
    sprintf(filename, "../data/%d.csv", n);
    FILE *f = fopen(filename, "r");
    if (f) {
        for (int i = 0; i < n * n; i++) {
            // Lecture robuste pour éviter les warnings
            if (fscanf(f, "%lf,", &M[i]) != 1) {
                if(fscanf(f, "%lf", &M[i])) {} 
            }
        }
        fclose(f);
    } else {
        for (int i = 0; i < n * n; i++) M[i] = ((double)rand() / RAND_MAX);
    }
}

// --- Simulation ZFP + Matmul ---
void matmul_with_zfp_overhead(double *A, double *B, double *C, int n) {
    
    // Configuration ZFP
    zfp_type type = zfp_type_double;
    zfp_field* field = zfp_field_2d(A, type, n, n);
    zfp_stream* zfp = zfp_stream_open(NULL);
    
    // Calcul de la taille max du buffer
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    void* buffer = malloc(bufsize);
    
    // CORRECTION ICI : stream_open au lieu de stream_open_buffer
    bitstream* stream = stream_open(buffer, bufsize);
    
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_set_rate(zfp, 16.0, type, 2, 0); // Compression rate=16

    // 1. Compression
    zfp_field_set_pointer(field, A);
    zfp_compress(zfp, field);
    
    // 2. Décompression
    zfp_stream_rewind(zfp); // Important : revenir au début du flux
    zfp_field_set_pointer(field, A); 
    zfp_decompress(zfp, field);

    // Nettoyage ZFP
    free(buffer); // Ne pas oublier de libérer le buffer
    stream_close(stream);
    zfp_stream_close(zfp);
    zfp_field_free(field);

    // 3. Calcul Matmul Classique
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    int n = atoi(argv[1]);
    
    double *A, *B, *C;
    allocate_matrix(&A, n);
    allocate_matrix(&B, n);
    allocate_matrix(&C, n);
    init_matrix(A, n);
    init_matrix(B, n);

    // --- REPETITION POUR POWERJOULAR ---
    int repeats = 1;
    if (n <= 64)        repeats = 2000;
    else if (n <= 128)  repeats = 200;
    else if (n <= 256)  repeats = 50;
    else if (n <= 512)  repeats = 10;
    else                repeats = 2;

    double start = get_time();
    for (int r = 0; r < repeats; r++) {
        matmul_with_zfp_overhead(A, B, C, n);
    }
    double end = get_time();
    
    printf("Algo:Matmul_ZFP Size:%d Time:%f\n", n, (end - start) / repeats);

    free_matrix(A); free_matrix(B); free_matrix(C);
    return 0;
}
