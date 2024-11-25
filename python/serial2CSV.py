import serial
import csv

# シリアルポートを開く
ser = serial.Serial('/dev/cu.ESP32_Test', 9600)

# CSVファイルを開く
with open('/Users/k22089kk/githum/BathGuardDuck/python/ToSearchShockWave.csv', 'w', newline='') as f:
    writer = csv.writer(f)

    # 継続してシリアルから読み取る
    while True:
        try:
            # シリアルから1行読み取る
            line = ser.readline().decode('utf-8').strip()  # デコードして改行を除去
            print(line)  # デバッグ用に出力
            writer.writerow([line])  # CSVに書き込み

        except KeyboardInterrupt:
            print("プログラムを終了します")
            break  # Ctrl+Cでプログラムを停止

# シリアルポートを閉じる
ser.close()