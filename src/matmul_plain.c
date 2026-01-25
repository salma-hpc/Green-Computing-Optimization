#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

// --- Fonctions utilitaires ---

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void allocate_matrix(double **M, int n) {
    *M = (double *)malloc(n * n * sizeof(double));
    if (*M == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(1);
    }
}

void free_matrix(double *M) {
    free(M);
}

// Essaie de lire SDRBench (../data/N.csv), sinon remplit aléatoirement
void init_matrix(double *M, int n) {
    char filename[100];
    sprintf(filename, "../data/%d.csv", n); // Chemin supposé des données SDRBench

    FILE *f = fopen(filename, "r");
    if (f) {
        // Lecture du fichier CSV (suppose des valeurs séparées par virgule ou espace)
        for (int i = 0; i < n * n; i++) {
            if (fscanf(f, "%lf,", &M[i]) != 1) {
                if (fscanf(f, "%lf", &M[i]) != 1) M[i] = ((double)rand() / RAND_MAX);
            }
        }
        fclose(f);
    } else {
        // Fallback : Génération aléatoire si fichier non trouvé
        // (Évite le crash "File not found")
        for (int i = 0; i < n * n; i++) {
            M[i] = ((double)rand() / RAND_MAX);
        }
    }
}

// --- Algorithme Matmul Classique (O^3) ---

void matmul(double *A, double *B, double *C, int n) {
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

    // --- LOGIQUE DE RÉPÉTITION POUR POWERJOULAR ---
    // Les petites matrices (64x64) se calculent en 0.0003s.
    // PowerJoular a besoin d'au moins 1 ou 2 secondes pour voir l'énergie.
    // On répète donc le calcul X fois selon la taille.
    
    int repeats = 1;
    if (n <= 64)        repeats = 10000; // Très petit -> beaucoup de répétitions
    else if (n <= 128)  repeats = 1000;
    else if (n <= 256)  repeats = 100;
    else if (n <= 512)  repeats = 20;    // Moyen -> quelques répétitions
    else                repeats = 5;     // Grand -> peu de répétitions

    // printf("Running size %d with %d repeats for energy measurement...\n", n, repeats);

    double start = get_time();

    for (int r = 0; r < repeats; r++) {
        matmul(A, B, C, n);
    }

    double end = get_time();
    
    // Temps TOTAL divisé par le nombre de répétitions pour avoir le temps unitaire
    double avg_time = (end - start) / repeats;

    // Format attendu par le script bash :
    printf("Algo:Matmul Size:%d Time:%f\n", n, avg_time);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    return 0;
}
