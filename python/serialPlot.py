import serial
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt

# シリアルポートの設定
port = "/dev/cu.ESP32_Test"  # Macのポート名に合わせて変更
baudrate = 9600
ser = serial.Serial(port, baudrate)

# ローパスフィルタの設定
def butter_lowpass(cutoff, fs, order=5):
    nyquist = 0.5 * fs
    normal_cutoff = cutoff / nyquist
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def apply_lowpass(data, b, a):
    return filtfilt(b, a, data)

# フィルタのパラメータ
cutoff = 2.0  # カットオフ周波数（Hz）
fs = 50.0     # サンプリング周波数（Hz）
order = 5       # フィルタの次数
b, a = butter_lowpass(cutoff, fs, order)

# プロットの準備
fig1, ax1 = plt.subplots()  # 生データ用のウィンドウ
fig2, ax2 = plt.subplots()  # フィルタ後データ用のウィンドウ

x_data, y_data, z_data = [], [], []
x_filtered, y_filtered, z_filtered = [], [], []
time_data = []

# データ差分の閾値（ノイズ除去用）
DIFF_THRESHOLD = 10000  # 差分の閾値

try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        
        # データ解析
        try:
            x, y, z = map(float, line.split(','))
            
            # ノイズ除去：一個前のデータとの差分が閾値以内の場合のみ受け入れ
            if len(x_data) == 0 or (
                abs(x - x_data[-1]) < DIFF_THRESHOLD and
                abs(y - y_data[-1]) < DIFF_THRESHOLD and
                abs(z - z_data[-1]) < DIFF_THRESHOLD
            ):
                x_data.append(x)
                y_data.append(y)
                z_data.append(z)
                time_data.append(len(time_data))
                
                # ローパスフィルタを適用
                x_filtered = apply_lowpass(x_data, b, a)
                y_filtered = apply_lowpass(y_data, b, a)
                z_filtered = apply_lowpass(z_data, b, a)
                
                # 生データの更新（fig1）
                ax1.clear()
                ax1.plot(time_data, x_data, label='X (Raw)', color='red')
                ax1.plot(time_data, y_data, label='Y (Raw)', color='green')
                ax1.plot(time_data, z_data, label='Z (Raw)', color='blue')
                ax1.set_title('Raw Acceleration Data')
                ax1.set_xlabel('Time')
                ax1.set_ylabel('Acceleration (m/s^2)')
                ax1.legend(loc='upper right')
                
                # フィルタ後のデータの更新（fig2）
                ax2.clear()
                ax2.plot(time_data, x_filtered, label='X (Filtered)', color='red')
                ax2.plot(time_data, y_filtered, label='Y (Filtered)', color='green')
                ax2.plot(time_data, z_filtered, label='Z (Filtered)', color='blue')
                ax2.set_title('Filtered Acceleration Data')
                ax2.set_xlabel('Time')
                ax2.set_ylabel('Acceleration (m/s^2)')
                ax2.legend(loc='upper right')
                
                # プロットの更新
                plt.draw()
                plt.pause(0.1)
            
        except ValueError:
            pass

except KeyboardInterrupt:
    print("通信を終了しました。")
    ser.close()
