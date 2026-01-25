#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "sz3c.h" 

// --- Fonctions utilitaires ---
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void allocate_matrix(double **M, int n) {
    *M = (double *)malloc(n * n * sizeof(double));
    if (*M == NULL) { fprintf(stderr, "Erreur alloc\n"); exit(1); }
}

void free_matrix(double *M) {
    free(M);
}

void init_matrix(double *M, int n) {
    char filename[100];
    sprintf(filename, "../data/%d.csv", n);
    FILE *f = fopen(filename, "r");
    if (f) {
        for (int i = 0; i < n * n; i++) {
            if (fscanf(f, "%lf,", &M[i]) != 1) {
                if(fscanf(f, "%lf", &M[i])) {}
            }
        }
        fclose(f);
    } else {
        // Fallback aléatoire
        for (int i = 0; i < n * n; i++) M[i] = ((double)rand() / RAND_MAX);
    }
}

// --- Algorithme Matmul avec SZ3 ---
void matmul_with_sz_overhead(double *A, double *B, double *C, int n) {
    
    size_t outSize;
    
    // Paramètres d'erreur pour SZ3
    // Mode 1 = Error Bound Relative (REL)
    int errBoundMode = 1; 
    double absErrBound = 0.0;
    double relErrBound = 1E-3; // Précision 0.001
    double pwrBound = 0.0;     // Pas utilisé en REL
    
    // 1. COMPRESSION
    // Signature SZ3C : dataType, data, outSize, mode, abs, rel, pwr, r5, r4, r3, r2, r1
    // Pour une matrice nxn, on utilise r2=n et r1=n. Les autres sont 0.
    unsigned char *cmpData = SZ_compress_args(
        SZ_DOUBLE, 
        A, 
        &outSize, 
        errBoundMode, 
        absErrBound, 
        relErrBound, 
        pwrBound, 
        0, 0, 0, n, n
    );

    // 2. DECOMPRESSION
    // Signature SZ3C : dataType, bytes, size, r5, r4, r3, r2, r1
    double *decData = (double*)SZ_decompress(
        SZ_DOUBLE, 
        cmpData, 
        outSize, 
        0, 0, 0, n, n
    );

    // 3. CALCUL (Matmul classique sur données décompressées)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += decData[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }

    // Nettoyage
    free(cmpData); // SZ alloue avec malloc, donc free ok
    free(decData);
}

// --- Main ---
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    
    double *A, *B, *C;
    allocate_matrix(&A, n);
    allocate_matrix(&B, n);
    allocate_matrix(&C, n);

    init_matrix(A, n);
    init_matrix(B, n);

    // --- LOGIQUE DE RÉPÉTITION POUR MESURE ÉNERGIE ---
    // Ajusté pour que le calcul dure ~2s minimum
    int repeats = 1;
    if (n <= 64)        repeats = 2000;
    else if (n <= 128)  repeats = 200;
    else if (n <= 256)  repeats = 50;
    else if (n <= 512)  repeats = 10;
    else                repeats = 2;

    double start = get_time();

    for (int r = 0; r < repeats; r++) {
        matmul_with_sz_overhead(A, B, C, n);
    }

    double end = get_time();
    
    // Temps moyen par exécution
    printf("Algo:Matmul_SZ Size:%d Time:%f\n", n, (end - start) / repeats);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    return 0;
}
