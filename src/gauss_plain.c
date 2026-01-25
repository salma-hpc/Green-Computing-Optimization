#include "utils.h"

// Pivot de Gauss simple (Résout Ax = b)
void gauss_solve(double *A, double *b, double *x, int N) {
    // 1. Élimination (Triangularisation)
    for (int k = 0; k < N - 1; k++) {
        for (int i = k + 1; i < N; i++) {
            double factor = A[i * N + k] / A[k * N + k];
            for (int j = k; j < N; j++) {
                A[i * N + j] -= factor * A[k * N + j];
            }
            b[i] -= factor * b[k];
        }
    }
    // 2. Remontée (Substitution arrière)
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
    double *x = (double *)malloc(N * sizeof(double)); // La solution qu'on cherche
    double *x_ref = (double *)malloc(N * sizeof(double)); // La solution cible (tout à 1)

    // 1. Matrice A issue de SDRBench
    init_matrix(A, N); 

    // 2. Vecteur solution cible : x = [1, 1, ... 1]
    for(int i=0; i<N; i++) x_ref[i] = 1.0;

    // 3. Génération du second membre : b = A * x_ref
    // Comme ça on est sûr qu'une solution existe et que c'est x_ref
    for(int i=0; i<N; i++) {
        b[i] = 0.0;
        for(int j=0; j<N; j++) {
            b[i] += A[i*N + j] * x_ref[j];
        }
    }

    double start = get_time();
    // 4. Résolution Ax = b
    gauss_solve(A, b, x, N);
    double end = get_time();

    printf("Algo:Gauss Size:%d Time:%.6f\n", N, end - start);

    free(A); free(b); free(x); free(x_ref);
    return 0;
}
