#include <Adafruit_LIS3DH.h>
#include <list>
#define LIS3DH_ADDRESS 0x19
Adafruit_LIS3DH accel = Adafruit_LIS3DH();

//加速度の構造体
struct Accel {
    double x;
    double y;
    double z;
};
//直近の加速度のリスト
std::list<Accel> recentAccelList;
//リストの最大長
const int MAX_RECENT_ACCEL = 20;

//加速度センサ初期化関数
void accelSetUp() {
    accel.begin(LIS3DH_ADDRESS);
    accel.setClick(0, 0);                      // Disable Interrupt
    accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
    accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
}

//加速度を表示する関数
void printAccel(Accel accel) {
    Serial.print("X: " + String(accel.x));
    Serial.print(", ");
    Serial.print("Y: " + String(accel.y));
    Serial.print(", ");
    Serial.print("Z: " + String(accel.z));
    Serial.println("");
}

//現在の加速度を取得する関数
Accel getNowAccel() {
    accel.read();
    return {(double)accel.x, (double)accel.y, (double)accel.z};
}

//加速度をリストに追加する関数
void addAccelToList(Accel accel) {
    recentAccelList.push_back(accel);
    if (recentAccelList.size() > MAX_RECENT_ACCEL) {
        recentAccelList.pop_front();
    }
}

//加速度リストの平均値を計算して返す関数
Accel getAvgAccel() {
    Accel avgAccel = {0, 0, 0};
    for (Accel accel : recentAccelList) {
        avgAccel.x += accel.x;
        avgAccel.y += accel.y;
        avgAccel.z += accel.z;
    }
    avgAccel.x /= recentAccelList.size();
    avgAccel.y /= recentAccelList.size();
    avgAccel.z /= recentAccelList.size();
    return avgAccel;
}

//現在の加速度と平均値の標準偏差を計算して返す関数
double getAccelStd(Accel avgAccel) {
    double var = 0;
    for (Accel accel : recentAccelList) {
        var += pow(accel.x - avgAccel.x, 2);
        var += pow(accel.y - avgAccel.y, 2);
        var += pow(accel.z - avgAccel.z, 2);
    }
    var /= recentAccelList.size();
    return sqrt(var);
}


