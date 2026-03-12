/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：04_IR3CH.ino
# Function：紅外線以三角佈署邊界感測 (IR3CH 類別) 使用範例
  Test Code of dual2S's IR3CH CLASS.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

const uint16_t IR_THRESHOLD = 2000;

// 宣告 IR3CH 物件，使用 dual2s.h 預設的類比輸入腳位 (34, 35, 39)
IR3CH ir(DUAL2S_HW::IR_L, DUAL2S_HW::IR_M, DUAL2S_HW::IR_R);

void setup() {
  Serial.begin(115200);

  Serial.println("=== IR3CH 紅外線感測器陣列測試開始 ===");
  Serial.println("準備讀取數值...");
}

void loop() {
  //---------------------------------------
  //參數(1):判斷門檻值
  //參數(2):判斷目標線：BLACK_LINE(黑線) / WHITE_LINE(白線)
  //參數(3):是否開啟DEBUG以輸出IR讀值
  //---------------------------------------
  ir.update(IR_THRESHOLD, IR3CH::BLACK_LINE, false);

  byte state = ir.getState();
  
  //IR are deployed in TRIANGLE pattern.
  //三角佈署感測器 
  switch(state) {
    case 0:
      Serial.println("自由運動");
      break;
    case 2:
      Serial.println("車頭壓線:快速後退");
      break;
    case 5:
      Serial.println("車尾壓線:全力前進");
      break;
    case 7:
      Serial.println("全車壓線:停止運動");
      break;  
    case 1:
    case 3:
      Serial.println("右後/右側大壓線:小左旋+小前進");
      break;
    case 4:
    case 6:
      Serial.println("左後/左側大壓線:小右旋+小前進");
      break;  
  }

}