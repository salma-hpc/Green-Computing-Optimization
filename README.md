# Green Computing: Efficacité énergétique et compression avec perte

Ce projet étudie l’impact de la compression avec perte sur la consommation énergétique de solveurs numériques utilisés en calcul haute performance. L’objectif est d’évaluer si le coût de calcul introduit par la compression peut être compensé par la réduction des transferts mémoire.

## Contexte

Dans les applications HPC, la bande passante mémoire et la consommation énergétique sont deux contraintes importantes. Ce travail s’intéresse à l’utilisation de techniques de compression avec perte dans des algorithmes de calcul scientifique afin d’analyser le compromis entre précision, performance et énergie.

## Objectif

Le projet vise à comparer plusieurs approches de compression appliquées à des solveurs standards d’algèbre linéaire dense, en particulier pour mesurer leur influence sur :

- le temps d’exécution ;
- le coût énergétique ;
- la stabilité numérique ;
- l’intérêt réel de la compression selon la taille des données.

## Méthodologie

L’étude repose sur des solveurs classiques de calcul matriciel et sur l’intégration de schémas de compression avec perte dans les flux de données manipulés pendant l’exécution.

### Algorithmes étudiés

- multiplication matricielle ;
- élimination de Gauss ;
- décomposition de Cholesky.

### Méthodes de compression

- **ZFP** : compression à précision fixe ;
- **SZ3** : compression bornée par l’erreur.

### Outils utilisés

- **C / C++** pour les solveurs ;
- **Python** pour l’analyse ;
- **PowerJoular** pour les mesures énergétiques ;
- **SDRBench** comme jeu de données de référence.

## Résultats principaux

Les tests ont été réalisés sur des matrices de taille modérée. Le tableau ci-dessous résume les observations principales.

| Algorithme | Compression | Impact énergétique | Observation |
|---|---|---:|---|
| Plain | Aucune | Référence (1.0x) | Plus efficace pour \(N \leq 512\) (memory-bound). |
| ZFP | Fixed-Acc | Overhead CPU élevé | Meilleure stabilité numérique que SZ3 sur Cholesky. |
| SZ3 | Error-Bound | Overhead CPU élevé | Taux de compression élevés mais coût énergétique important. |

Ces résultats montrent que, pour les tailles testées, le coût de la compression dépasse souvent le gain obtenu sur les transferts mémoire. La compression devient surtout intéressante lorsque les volumes de données augmentent et que la bande passante mémoire est le principal goulot d’étranglement.

## Structure du dépôt

- `src/` : code source des solveurs et wrappers de compression ;
- `include/` : fichiers d’en-tête ;
- `scripts/` : scripts d’automatisation et d’analyse ;
- `results/` : mesures de consommation, sorties CSV et courbes de performance ;
- `README.md` : présentation du projet ;
- `Rapport Green Computing_*.pdf` : rapport complet.

## Conclusion

La compression avec perte n’est pas toujours avantageuse dans ce contexte. Pour les tailles de matrices considérées, l’overhead lié à la compression reste important. L’approche devient plus pertinente lorsque les données à traiter augmentent et que la bande passante mémoire devient limitant

