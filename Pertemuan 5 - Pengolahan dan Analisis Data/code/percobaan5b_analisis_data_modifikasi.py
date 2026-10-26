# Modifikasi: moving average (jendela 5 data) pada data hasil CSV sebelum dihitung z-score,
# lalu membandingkan jumlah anomali data mentah dengan data hasil moving average.
# Jalankan setelah data_suhu.csv terbentuk (dari percobaan5b_analisis_data.py atau versi dummy).
import pandas as pd
import matplotlib.pyplot as plt

JENDELA = 5          # ukuran jendela moving average
BATAS_Z = 2          # batas |z| untuk anomali

# --- Baca data hasil CSV ---
df = pd.read_csv("data_suhu.csv")

# --- Z-score pada data mentah (pembanding) ---
rata2 = df["suhu"].mean()
std = df["suhu"].std()
df["zscore"] = 0.0 if std == 0 else (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > BATAS_Z

# --- BARU: moving average jendela 5 data ---
# min_periods=1 agar 4 data awal tetap dihitung (dengan jendela yang lebih pendek)
df["suhu_ma"] = df["suhu"].rolling(window=JENDELA, min_periods=1).mean()

# --- BARU: z-score pada data hasil moving average ---
rata2_ma = df["suhu_ma"].mean()
std_ma = df["suhu_ma"].std()
df["zscore_ma"] = 0.0 if std_ma == 0 else (df["suhu_ma"] - rata2_ma) / std_ma
df["anomali_ma"] = df["zscore_ma"].abs() > BATAS_Z

# --- BARU: bandingkan jumlah anomali ---
print(f"Rata-rata data mentah : {rata2:.2f} | Std: {std:.2f}")
print(f"Rata-rata data MA     : {rata2_ma:.2f} | Std: {std_ma:.2f}")
print(f"Jumlah anomali data mentah       : {df['anomali'].sum()}")
print(f"Jumlah anomali data moving avg   : {df['anomali_ma'].sum()}")

df.to_csv("data_suhu_ma.csv", index=False)
print("Hasil tersimpan di data_suhu_ma.csv")

# --- Visualisasi perbandingan ---
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", alpha=0.5, label="Suhu mentah")
plt.plot(df.index, df["suhu_ma"], marker="s", label=f"Moving average ({JENDELA})")
anom_mentah = df[df["anomali"]]
anom_ma = df[df["anomali_ma"]]
plt.scatter(anom_mentah.index, anom_mentah["suhu"], color="red", s=100,
            label="Anomali (mentah)", zorder=3)
plt.scatter(anom_ma.index, anom_ma["suhu_ma"], color="orange", s=100, marker="X",
            label="Anomali (MA)", zorder=4)
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Perbandingan Deteksi Anomali: Data Mentah vs Moving Average")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu_ma.png")
plt.show()
