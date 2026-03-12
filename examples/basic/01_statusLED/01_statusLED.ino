/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：01_statusLED.ino
# Function：WS2812b全採LED(statusLED類別)使用範例
  Test Code of dual2S's statusLED CLASS for ws2812b.
# dual2s：Buzzer Pin G2 & number 2. You can extend ws2812b number in G2.
# dual2s mini：Buzzer Pin G2 & number 1. You can extend ws2812b number in G2.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s, Adafruit_NeoPixel.h
======================================================================================*/
#include <dual2s.h>

// 定義硬體腳位與 LED 數量
#define NUM_LEDS 2

// 宣告 stateLED 物件
// dual2s G2控制 ws2812b, DUAL2S_HW::WSLED
// 參數1：控制腳位, 參數2：燈數
stateLED myLED(DUAL2S_HW::WSLED, NUM_LEDS);

void setup() {
  Serial.begin(115200);
  Serial.println("=== stateLED WS2812B 測試開始 ===");

  // 1. 初始化 LED (必須呼叫)
  myLED.begin();
}

void loop() {
  Serial.println("\n--- 測試 1：進階控制 (fillColor 一鍵全亮 Enum 顏色) ---");
  myLED.fillColor(stateLED::BLUE);   delay(1000); // 全亮藍光 (代表連線)
  myLED.fillColor(stateLED::YELLOW); delay(1000); // 全亮黃光 (代表警告)
  myLED.fillColor(stateLED::RED);    delay(1000); // 全亮紅光 (代表攻擊)

  Serial.println("--- 測試 2：進階控制 (setColor 單顆控制 Enum 顏色) ---");
  myLED.clear(); // 先清除畫面
  
  // 讓第 0 顆 (左邊) 亮綠色，第 1 顆 (右邊) 亮紫色
  myLED.setColor(0, stateLED::GREEN);
  delay(500);
  myLED.setColor(1, stateLED::PURPLE);
  delay(1500);

  Serial.println("--- 測試 3：基礎控制 (setPixel 自訂 RGB 單顆顏色) ---");
  myLED.clear();
  // 自訂特殊的粉紅色 (R:255, G:20, B:147) 給第 0 顆
  myLED.setPixel(0, 255, 20, 147);
  // 自訂特殊的湖水綠 (R:32, G:178, B:170) 給第 1 顆
  myLED.setPixel(1, 32, 178, 170);
  delay(2000);

  Serial.println("--- 測試 4：基礎控制 (fill 自訂 RGB 全亮) ---");
  // 全亮自訂的暖白色 (R:255, G:200, B:100)
  myLED.fill(255, 200, 100);
  delay(1000);

  Serial.println("--- 測試 5：動態亮度調整 (setBrightness) ---");
  // 測試呼吸燈效果 (由亮變暗)
  Serial.println("--- 測試 5-1：由亮變暗 ---");
  for (int b = 255; b > 5; b -= 5) {
    myLED.setBrightness(b); // 動態改變亮度
    delay(100);
  }
  // 測試呼吸燈效果 (由暗變亮)
  Serial.println("--- 測試 5-2：由暗變亮 ---");
  for (int b = 5; b <= 255; b += 5) {
    myLED.setBrightness(b);
    delay(100);
  }
  delay(1000);

  Serial.println("--- 測試 6：清除畫面 (clear) ---");
  myLED.clear(); // 關閉所有燈光
  delay(2000); // 停頓 2 秒後，重新開始新一輪測試
}