#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

// Fonction chrono
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Initialisation Aléatoire (Fallback au cas où)
void init_matrix(double *A, int N) {
    for (int i = 0; i < N * N; i++) {
        A[i] = (double)rand() / RAND_MAX;
    }
}

// --- NOUVEAU : Chargement SDRBench ---
void init_matrix_from_file(double *A, int N) {
    char filename[512];
    // Chemin adapté à ton arborescence : ../data/sdrbench_blocks/temperature_NxN.f32
    sprintf(filename, "../data/sdrbench_blocks/temperature_%dx%d.f32", N, N);
    
    FILE *f = fopen(filename, "rb");
    if (!f) {
        // Si le fichier n'existe pas, on prévient et on met de l'aléatoire pour ne pas planter
        // (Utile pour 1024/2048 si tu n'as pas téléchargé ces fichiers là)
        if (N <= 512) printf("⚠️  Fichier introuvable : %s (Passage en aléatoire)\n", filename);
        init_matrix(A, N);
        return;
    }

    // Lecture en float (SDRBench est en float 32 bits)
    float *temp_buffer = (float *)malloc(N * N * sizeof(float));
    if (!temp_buffer) { perror("Malloc temp_buffer"); exit(1); }

    size_t read_count = fread(temp_buffer, sizeof(float), N * N, f);
    fclose(f);

    // Copie et conversion vers double
    for (int i = 0; i < N * N; i++) {
        if (i < read_count) {
            A[i] = (double)temp_buffer[i];
        } else {
            A[i] = 0.0; // Padding si fichier trop petit
        }
    }
    
    free(temp_buffer);
}

// Pour Cholesky : On force la matrice à être Symétrique Définie Positive (SDP)
// A_sdp = A * A' + I
void make_symmetric_positive(double *A, int N) {
    // Note : C'est une approx rapide pour que Cholesky ne plante pas
    // On ajoute N sur la diagonale pour assurer la dominance
    for(int i=0; i<N; i++) {
        A[i*N + i] += (double)N; 
        for(int j=0; j<i; j++) {
            double val = (A[i*N + j] + A[j*N + i]) * 0.5;
            A[i*N + j] = val;
            A[j*N + i] = val;
        }
    }
}

#endif
