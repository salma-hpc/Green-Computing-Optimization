# 🌿 Green Computing : Efficacité Énergétique & Compression avec Perte

**Auteur :** Salma BENSMAIL  
**Contexte :** Master 2 CHPS - Module Green Computing (Université de Perpignan)  
**Encadrant :** M. Matthieu MARTEL

📄 **[Lire le rapport complet du projet (PDF)](./Rapport_Green_Computing_Salma_BENSMAIL-2.pdf)**

## 📖 Vue d'ensemble
Le Calcul Haute Performance (HPC) fait face à un défi critique : la consommation énergétique. Ce projet étudie le compromis entre **Précision Numérique** et **Efficacité Énergétique** en intégrant des algorithmes de compression avec perte (**ZFP**, **SZ3**) dans des solveurs d'algèbre linéaire standards (Gauss, Cholesky).

L'objectif est de déterminer si le surcoût de calcul (*overhead*) induit par la compression est compensé par la réduction de l'utilisation de la bande passante mémoire.

## 📂 Structure du dépôt
- **`src/`** : Code source des solveurs (Gauss, Cholesky, Matmul) et wrappers de compression.
- **`include/`** : Fichiers d'en-tête pour l'intégration de ZFP/SZ3.
- **`scripts/`** : Scripts d'automatisation (Bash pour PowerJoular, Python pour l'analyse).
- **`results/`** : Logs de consommation énergétique (CSV) et courbes de performance.

## 🛠️ Stack Technique & Méthodologie
- **Algorithmes** : Multiplication Matricielle, Élimination de Gauss, Décomposition de Cholesky.
- **Compression** : 
  - **ZFP** (Mode précision fixe).
  - **SZ3** (Borné par l'erreur).
- **Monitoring** : **PowerJoular** (Mesure énergétique basée sur RAPL).
- **Jeux de données** : **SDRBench** (Scientific Data Reduction Benchmark).

## 🚀 Résultats Clés & Performance
Les tests ont été effectués sur des matrices de taille $N=64$ à $N=512$.

| Algorithme | Compression | Impact Énergétique | Observation |
|------------|:-----------:|:------------------:|-------------|
| **Plain**  | Aucune      | Référence (1.0x)   | Plus efficace pour $N < 512$ (Memory-bound) |
| **ZFP**    | Fixed-Acc   | Overhead CPU élevé | Meilleure stabilité numérique que SZ3 sur Cholesky |
| **SZ3**    | Error-Bound | Overhead CPU élevé | Taux de compression élevés mais coût énergétique important |

### 📌 Conclusion du rapport
Pour ces tailles de matrices, le coût de calcul de la compression (*overhead*) dépasse souvent le gain énergétique obtenu par la réduction des transferts mémoire. L'approche devient pertinente pour des volumes de données massifs où la bande passante est le goulot d'étranglement principal.

---
*Projet réalisé dans le cadre du cursus Master 2 CHPS (2025).*

