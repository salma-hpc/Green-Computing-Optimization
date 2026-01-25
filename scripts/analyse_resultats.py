import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# CONFIGURATION
REPEATS = {
    "matmul_plain": {64: 10000, 128: 1000, 256: 100, 512: 20},
    "matmul_zfp":   {64: 2000,  128: 200,  256: 50,  512: 10},
    "matmul_sz":    {64: 2000,  128: 200,  256: 50,  512: 10}
}

# 1. Chargement & Normalisation
try:
    df = pd.read_csv("resultats_sdrbench_final.csv")
except:
    print("Erreur CSV")
    exit()

def normalize(row):
    reps = REPEATS.get(row['Algo'], {}).get(row['Size'], 1)
    row['Energy_Unit'] = row['Energy_J'] / reps
    row['Time_Unit'] = row['Time_s'] / reps
    return row

df_clean = df.apply(normalize, axis=1)

# Style & Couleurs
sns.set_theme(style="whitegrid")
pal = {"matmul_plain": "#1f77b4", "matmul_zfp": "#2ca02c", "matmul_sz": "#ff7f0e"}
pal_ratio = {"Ratio_ZFP": "#2ca02c", "Ratio_SZ": "#ff7f0e"}

print("Génération des 5 graphiques...")

# --- 1. ENERGIE ---
plt.figure(figsize=(10,6))
sns.lineplot(data=df_clean, x='Size', y='Energy_Unit', hue='Algo', style='Algo', palette=pal, markers=True, linewidth=2.5)
plt.yscale('log')
plt.title('Consommation Énergétique Unitaire (Log)', fontsize=14)
plt.ylabel('Joules / Opération')
plt.grid(True, which="minor", ls=":", alpha=0.5)
plt.savefig('graphe_energie.png', dpi=300)
print("1/5 graphe_energie.png OK")

# --- 2. TEMPS ---
plt.figure(figsize=(10,6))
sns.lineplot(data=df_clean, x='Size', y='Time_Unit', hue='Algo', style='Algo', palette=pal, markers=True, linewidth=2.5)
plt.yscale('log')
plt.title('Temps d\'Exécution Unitaire (Log)', fontsize=14)
plt.ylabel('Secondes / Opération')
plt.grid(True, which="minor", ls=":", alpha=0.5)
plt.savefig('graphe_temps.png', dpi=300)
print("2/5 graphe_temps.png OK")

# --- 3. BAR CHART (Compromis) ---
df_sub = df_clean[df_clean['Size'].isin([64, 512])]
plt.figure(figsize=(10,6))
sns.barplot(data=df_sub, x='Size', y='Energy_Unit', hue='Algo', palette=pal)
plt.yscale('log')
plt.title('Compromis Énergétique : Petites vs Grandes Matrices', fontsize=14)
plt.ylabel('Joules (Log)')
plt.savefig('graphe_barres.png', dpi=300)
print("3/5 graphe_barres.png OK")

# --- 4. OVERHEAD (Ratio) ---
df_mean = df_clean.groupby(['Algo', 'Size'])[['Energy_Unit']].mean().reset_index()
df_pivot = df_mean.pivot(index='Size', columns='Algo', values='Energy_Unit')
df_ratio = df_pivot.copy()
df_ratio['Ratio_ZFP'] = df_ratio['matmul_zfp'] / df_ratio['matmul_plain']
df_ratio['Ratio_SZ'] = df_ratio['matmul_sz'] / df_ratio['matmul_plain']
df_ratio_plot = df_ratio[['Ratio_ZFP', 'Ratio_SZ']].reset_index().melt(id_vars='Size', var_name='Algo', value_name='Overhead')

plt.figure(figsize=(10,6))
sns.lineplot(data=df_ratio_plot, x='Size', y='Overhead', hue='Algo', style='Algo', palette=pal_ratio, markers=True, linewidth=2.5)
plt.axhline(1.0, color='black', linestyle='--', label="Référence (Plain)")
plt.title('Surcoût Énergétique Relatif (Overhead)', fontsize=14)
plt.ylabel('Facteur de Surcoût (x fois Plain)')
plt.grid(True, which="minor", ls=":")
plt.savefig('graphe_overhead.png', dpi=300)
print("4/5 graphe_overhead.png OK")

# --- 5. EFFICACITÉ ---
df_clean['Efficiency'] = 1.0 / df_clean['Energy_Unit']
plt.figure(figsize=(10,6))
sns.barplot(data=df_clean, x='Size', y='Efficiency', hue='Algo', palette=pal)
plt.yscale('log')
plt.title('Efficacité Énergétique (Opérations par Joule)', fontsize=14)
plt.ylabel('Op/J (Plus haut est mieux)')
plt.savefig('graphe_efficacite.png', dpi=300)
print("5/5 graphe_efficacite.png OK")

print("\nTerminé ! Tout est là.")
