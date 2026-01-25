#!/bin/bash

# --- CONFIGURATION ---
SIZES=(1024 2048) # Tailles assez grandes pour avoir le temps de mesurer
ALGOS=("matmul_plain" "gauss_plain" "cholesky_plain")
RESULT_DIR="../results"

# Création du dossier
mkdir -p $RESULT_DIR

echo "=== Lancement des mesures (Méthode PJ First) ==="

for SIZE in "${SIZES[@]}"; do
    for ALGO in "${ALGOS[@]}"; do
        echo "--> Mesure : $ALGO avec taille $SIZE"
        
        CSV_FILE="$RESULT_DIR/res_${ALGO}_${SIZE}.csv"
        
        # 1. DÉMARRAGE DE POWERJOULAR (En arrière-plan)
        # On ne cible pas de PID spécifique pour éviter les erreurs, on log tout.
        powerjoular -l -f "$CSV_FILE" > /dev/null 2>&1 &
        PJ_PID=$!
        
        # On laisse 1 seconde à PJ pour s'initialiser
        sleep 1
        
        # 2. EXÉCUTION DU CALCUL
        ../src/$ALGO $SIZE
        
        # 3. ARRÊT DE POWERJOULAR
        # On attend 1 seconde après le calcul pour avoir la fin de courbe
        sleep 1
        kill $PJ_PID 2>/dev/null
        
        # Petite pause pour nettoyer
        sleep 1
    done
done

echo "=== Terminé. Vérifie le dossier results ==="
ls -l $RESULT_DIR
