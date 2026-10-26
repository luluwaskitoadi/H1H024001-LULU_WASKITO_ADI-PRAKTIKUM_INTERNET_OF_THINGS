# Versi dummy: tahap 1 tidak memakai MQTT, data suhu dibangkitkan langsung di Python.
# Tahap 2 sampai 4 (CSV, statistik, z-score, grafik) sama persis dengan versi original.
import random
import time
import pandas as pd
import matplotlib.pyplot as plt

JUMLAH_SAMPEL = 30  # jumlah data

random.seed()  # ganti ke random.seed(42) kalau mau hasil yang selalu sama

# --- Tahap 1: akuisisi data (DUMMY, pengganti MQTT) ---
data = []
for i in range(JUMLAH_SAMPEL):
    # Suhu dasar 28 °C dengan derau acak sekitar ±0.5 °C
    suhu = 28.0 + random.uniform(-0.5, 0.5)

    # Sekitar 1 dari 10 data dibuat lonjakan (meniru sumber panas / gangguan)
    if random.random() < 0.1:
        suhu += random.choice([-1, 1]) * random.uniform(3.0, 5.0)

    suhu = round(suhu, 2)
    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

# --- Tahap 2: simpan ke CSV ---
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# --- Tahap 3: analisis statistik dan deteksi anomali (z-score) ---
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata : {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum : {df['suhu'].min():.2f}")
print(f"Maksimum : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# --- Tahap 4: visualisasi ---
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
