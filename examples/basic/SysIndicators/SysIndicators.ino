/*=====================================================================================
yesio.net / 2026.03.01 / by nick
# Filename：SysIndicators.ino
# Function：系統指示器(ws2812b全彩LED / 蜂鳴器 Buzzer)之使用範例, 
  Examples of dual2S's application in ws2812b & Buzzer.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include "dual2s.h"

Buzzer bz(DUAL2S_HW::BUZZER); //建立物件 - Buzzer, GPIO 15
stateLED led(DUAL2S_HW::WSLED, 2);           //建立物件 - stateLED兩顆, GPIO 2

void setup() {
  led.begin();
}

void loop() {
  led.fillColor(stateLED::BLUE);
  bz.alarm(800);  // 聲響頻率800Hz持續200ms 
  
  delay(1000);    // 暫停 1 秒
  
  led.fillColor(stateLED::RED);
  bz.alarm(400);  // 聲響頻率400Hz持續200ms
  delay(1000);
  
  //參數順序：某顆, R, G, B
  led.setPixel(0, 255, 0, 0);
  led.setPixel(1, 0, 255, 0);

  while(1); //不再執行
}
