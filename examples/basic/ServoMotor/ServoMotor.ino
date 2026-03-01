/*=====================================================================================
yesio.net / 2026.03.01 / by nick
# Filename：ServoMotor.ino
# Function：伺服馬達(MG995/SG90s)之使用範例, 
  Examples of dual2S's application in Servo Motor(MG995/SG90s).
# Toolchain & Libs：ESP-IDF-v3.3.7, dual2s-v2.0.1
======================================================================================*/
#include "dual2s.h"

//支援MG995大扭矩伺服馬達, GPIO 18, 19, 2
//物件宣告使用內建namespace, 也可以直接填入指定GPIO腳位號
ServoMotor s1(DUAL2S_HW::MG18); 
ServoMotor s2(DUAL2S_HW::MG19);

void setup() {
  // 初始化並設定物理限制 0~180 度
  // 參數順序：最小限制、最大限制、開機預設角度
  s1.begin(0, 180, 0); 
  s2.begin(0, 180, 0);
}

void loop() {
  s1.write(180);  // 設定目標角度
  s2.write(180);
  delay(1000);    // 等待 1 秒讓舵機轉到位
  
  s1.write(0);
  s2.write(0);
  delay(1000);
}