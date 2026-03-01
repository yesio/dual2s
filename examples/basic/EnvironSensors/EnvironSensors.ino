/*=====================================================================================
yesio.net / 2026.03.01 / by nick
# Filename：EnvironSensors.ino
# Function：dual2s電壓、超音波、3通道近距紅外線之使用範例, 
  Examples of dual2S's application in voltage, ultrasound, and 3-channel near-infrared.
# Toolchain & Libs：ESP32 Arduino Core v3.3.7 (ESP-IDF v5.1), dual2s-v2.0.1
======================================================================================*/
#include "dual2s.h"

Power pwr(DUAL2S_HW::BATTERY);                                 //建立物件 - dual2s控制器電壓偵測
HCSR04 sonar(DUAL2S_HW::USC_F_ECHO, DUAL2S_HW::USC_F_TRIG);    //建立物件 - 前側超音波
IR3CH line(DUAL2S_HW::IR_L, DUAL2S_HW::IR_M, DUAL2S_HW::IR_R); //建立物件 - 3通道循線感測

void setup() {
  Serial.begin(115200);
}

void loop() {
  // 等待 ADC 轉換或超音波回波
  float v = pwr.read();
  float d = sonar.ObjDistance(); 
  
  // 直接讀取原始類比值 (不呼叫需狀態機的 update)
  uint16_t L = line.getValL();
  uint16_t M = line.getValM();
  uint16_t R = line.getValR();

  //每500ms讀取一次並列印於串列埠視窗
  Serial.printf("V: %.2fV, Dist: %.2fcm, IR: %d, %d, %d\n", v, d, L, M, R);
  delay(500);
}