/*=====================================================================================
yesio.net / 2026.03.01 / by nick
# Filename：dcMotors.ino
# Function：4通道直流(DC)馬達與GoSUMO車體運動之使用範例, 
  Examples of dual2S's application in 4 DC-Motors & GoSUMO motion control.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include "dual2s.h"

// 引用 M1 ~ M4 的預設 A/B 腳位
Motor m1(DUAL2S_HW::M1A, DUAL2S_HW::M1B);
Motor m2(DUAL2S_HW::M2A, DUAL2S_HW::M2B);
Motor m3(DUAL2S_HW::M3A, DUAL2S_HW::M3B);
Motor m4(DUAL2S_HW::M4A, DUAL2S_HW::M4B);

//GoSUMO為四驅運動模式(抓地胎 or 麥克納姆倫)
//將宣告完成的4個DC-Motor物件指向GoSUMO物件中使用。
GoSUMO GoSUMO(&m1, &m2, &m3, &m4);

void setup() {
  // 停止所有馬達確保安全
  GoSUMO.stop();
}

void loop() {
  //馬達轉速以PWM控制, PWM數值範圍 0-1023
  
  //參數順序：轉向, PWM數值
  GoSUMO.act(GoSUMO::FORWARD, 600);   // 以 600 速度前進 2 秒
  delay(2000);
  
  // 停止
  GoSUMO.stop();
  delay(1000);

  //指定單顆DC-Motor轉動
  //參數順序：轉向, PWM數值
  m1.act(Motor::CW, 900);  //Motor-1 PWM 800 順時針旋轉
  m3.act(Motor::CCW, 200); //Motor-3 PWM 200 逆時針旋轉
  delay(3000);
  
  //旋轉3秒後停止
  m1.stop();
  m3.stop();

  while(1); 
}