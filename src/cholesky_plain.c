#include "utils.h"
#include <math.h>

void cholesky(double *A, double *L, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            for (int k = 0; k < j; k++) {
                sum += L[i * N + k] * L[j * N + k];
            }
            if (i == j) {
                double val = A[i * N + i] - sum;
                if (val <= 0) {
                    // printf("Erreur: Matrice non définie positive à l'index %d (val=%.2f)\n", i, val);
                    // On force une valeur positive pour éviter le crash sqrt(-1)
                    val = 1.0; 
                }
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
    double *L = (double *)calloc(N * N, sizeof(double)); // calloc met à 0

    // Chargement SDRBench + Transformation SDP
    init_matrix_from_file(A, N);
    make_symmetric_positive(A, N);

    double start = get_time();
    cholesky(A, L, N);
    double end = get_time();

    printf("Algo:Cholesky Size:%d Time:%.6f\n", N, end - start);

    free(A); free(L);
    return 0;
}
