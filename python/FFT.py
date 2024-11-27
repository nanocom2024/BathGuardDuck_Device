import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt
import csv

# CSVファイルのパス
csv_file = "./python/csv/nyuusui.csv"

# フィルタリング用の関数（ローパスフィルター）
def butter_lowpass(cutoff, fs, order=5):
    nyquist = 0.5 * fs
    normal_cutoff = cutoff / nyquist
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order)
    y = filtfilt(b, a, data)
    return y

# データを読み込む
x_data, y_data, z_data = [], [], []

with open(csv_file, 'r', encoding='utf-8') as file:
    reader = csv.reader(file)
    for row in reader:
        if row:
            values = row[0].split(',')
            x, y, z = map(lambda val: float(val.strip()), values)
            x_data.append(x)
            y_data.append(y)
            z_data.append(z)

# サンプリングレートとカットオフ周波数（Hz）
sampling_rate = 100
cutoff_frequency = 2.0  # 低周波成分を取り除くカットオフ周波数

# ローパスフィルターを適用
x_data_filtered = butter_lowpass_filter(x_data, cutoff_frequency, sampling_rate)
y_data_filtered = butter_lowpass_filter(y_data, cutoff_frequency, sampling_rate)
z_data_filtered = butter_lowpass_filter(z_data, cutoff_frequency, sampling_rate)

# 時間軸
N = len(x_data)
time = np.arange(N) / sampling_rate

# プロット: 生データとフィルタリング後のデータ
plt.figure(figsize=(12, 12))

# 生データ: X軸
plt.subplot(6, 1, 1)
plt.plot(time, x_data, label='X axis (raw)', color='red')
plt.title('Raw X-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

# 生データ: Y軸
plt.subplot(6, 1, 2)
plt.plot(time, y_data, label='Y axis (raw)', color='orange')
plt.title('Raw Y-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

# 生データ: Z軸
plt.subplot(6, 1, 3)
plt.plot(time, z_data, label='Z axis (raw)', color='green')
plt.title('Raw Z-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

# フィルタリング後のデータ: X軸
plt.subplot(6, 1, 4)
plt.plot(time, x_data_filtered, label='X axis (filtered)', color='red')
plt.title('Filtered X-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

# フィルタリング後のデータ: Y軸
plt.subplot(6, 1, 5)
plt.plot(time, y_data_filtered, label='Y axis (filtered)', color='orange')
plt.title('Filtered Y-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

# フィルタリング後のデータ: Z軸
plt.subplot(6, 1, 6)
plt.plot(time, z_data_filtered, label='Z axis (filtered)', color='green')
plt.title('Filtered Z-axis Acceleration')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid(True)

plt.tight_layout()
plt.show()

# フーリエ変換を実行
x_fft = np.fft.fft(x_data_filtered)
y_fft = np.fft.fft(y_data_filtered)
z_fft = np.fft.fft(z_data_filtered)

# 周波数軸の作成
frequencies = np.fft.fftfreq(N, d=1/sampling_rate)

# フーリエ変換の結果の振幅
x_amplitude = np.abs(x_fft) / N
y_amplitude = np.abs(y_fft) / N
z_amplitude = np.abs(z_fft) / N

# 正の周波数のみをプロットするためのインデックス
positive_freqs = frequencies > 0

# フーリエ変換の結果のプロット
plt.figure(figsize=(12, 6))

# フーリエ変換: X軸
plt.subplot(3, 1, 1)
plt.plot(frequencies[positive_freqs], x_amplitude[positive_freqs], label='X axis (filtered)')
plt.title('Fourier Transform of X-axis (Filtered)')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.grid(True)

# フーリエ変換: Y軸
plt.subplot(3, 1, 2)
plt.plot(frequencies[positive_freqs], y_amplitude[positive_freqs], label='Y axis (filtered)', color='orange')
plt.title('Fourier Transform of Y-axis (Filtered)')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.grid(True)

# フーリエ変換: Z軸
plt.subplot(3, 1, 3)
plt.plot(frequencies[positive_freqs], z_amplitude[positive_freqs], label='Z axis (filtered)', color='green')
plt.title('Fourier Transform of Z-axis (Filtered)')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.grid(True)

plt.tight_layout()
plt.show()
