/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：07_powerMonitoring.ino
# Function：dual2s電壓之使用範例, 
  Examples of dual2S's application for voltage detection.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include "dual2s.h"

#define NUM_LEDS 2 // 定義硬體腳位與 LED 數量

Power pwr(DUAL2S_HW::BATTERY);    // 建立物件 - dual2s控制器電壓偵測
stateLED myLED(DUAL2S_HW::WSLED, NUM_LEDS); // 參數1：控制腳位, 參數2：燈數

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  myLED.begin();
  
  Serial.println("=== 電壓監控測試開始 ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // 實驗因素, 是以每秒讀取一次電壓
  if (currentMillis - lastUpdate >= 1000) {
     lastUpdate = currentMillis;

     float v = pwr.read();
     
     // 根據 2S 鋰電池特性微調的安全閾值
     if (v < 6.4) {  
       myLED.fillColor(stateLED::RED);    // 危險低電量 (< 6.4V)：顯示紅色 (建議立即斷電)
     } 
     else if (v < 7.4) { 
       myLED.fillColor(stateLED::YELLOW); // 電量偏低 (< 7.4V)：顯示黃色 (建議準備充電)
     } 
     else {
       myLED.fillColor(stateLED::GREEN);  // 電量健康 (>= 7.4V)：顯示綠色
     }

     Serial.printf("Battery Voltage: %.2fV\n", v);
  }
}