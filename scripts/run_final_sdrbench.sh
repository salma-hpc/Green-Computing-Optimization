#!/bin/bash

# Fichier de sortie
CSV="resultats_sdrbench_final.csv"
echo "Algo,Size,Run,Energy_J,Time_s" > $CSV

# Paramètres stricts de l'énoncé
SIZES=(64 128 256 512)
RUNS=10
ALGOS=("matmul_plain" "matmul_zfp" "matmul_sz")

# Config SZ (chemin vers la lib)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../SZ3/build/lib

echo "================================================="
echo "Démarrage du protocole expérimental SDRBench"
echo "Tailles : ${SIZES[*]}"
echo "Runs par taille : $RUNS"
echo "================================================="

for algo in "${ALGOS[@]}"; do
    echo ">> Test de l'algorithme : $algo"
    
    for N in "${SIZES[@]}"; do
        echo "   -> Taille matrice : $N x $N"
        
        for (( r=1; r<=RUNS; r++ )); do
            # Fichier temporaire pour ce run
            PJ_LOG="pj_temp.log"
            rm -f $PJ_LOG
            
            # 1. Démarrer PowerJoular en tâche de fond
            # On redirige tout vers le log
            sudo powerjoular > $PJ_LOG 2>&1 &
            PJ_PID=$!
            
            # Petite pause pour laisser PowerJoular s'initialiser
            sleep 0.5
            
            # 2. Mesurer le temps exact et lancer le calcul
            START=$(date +%s.%N)
            ./$algo $N > /dev/null
            END=$(date +%s.%N)
            
            # 3. Arrêter PowerJoular immédiatement
            sudo kill -SIGINT $PJ_PID
            wait $PJ_PID 2>/dev/null
            
            # 4. Calculs
            DURATION=$(echo "$END - $START" | bc)
            
            # Récupérer l'énergie totale depuis le log de PowerJoular
            # Format attendu: "Total energy: 12.34 Joules"
            ENERGY=$(grep "Total energy" $PJ_LOG | awk '{print $3}')
            
            # Sécurité si le calcul est trop rapide et que PJ n'a rien capté
            if [ -z "$ENERGY" ]; then 
                ENERGY="0.00"
            fi
            
            echo "      Run $r/$RUNS : Time=${DURATION}s, Energy=${ENERGY}J"
            echo "$algo,$N,$r,$ENERGY,$DURATION" >> $CSV
            
            # Petite pause entre les runs pour laisser le CPU refroidir
            sleep 0.5
        done
    done
done

echo "================================================="
echo "Terminé ! Données sauvegardées dans $CSV"
cat $CSV
