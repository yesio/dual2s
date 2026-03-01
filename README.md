dual2s Library for ESP32
dual2s 是一個專為 ESP32 機器人設計的高整合度函數庫，特別適用於 Sumo Robot（相撲機器人） 或 四輪驅動避障車。本庫整合了電機驅動、舵機控制、超音波測距及感測器讀取，並支持**非阻塞（Non-blocking）**架構，讓您的機器人反應更加靈敏。

核心功能
多路馬達控制：支援 4 路 DC 馬達，並提供前進、後退、橫移（Mecanum）等抽象化指令。

精準舵機控制：支援 MG995 等伺服馬達，內建物理角度限制保護與平滑移動功能。

非阻塞音效與感測：蜂鳴器旋律播放與循跡感測器採樣皆不佔用 CPU 等待時間。

硬體狀態指示：整合 WS2812B RGB LED，一鍵切換「搜尋」、「攻擊」、「錯誤」等狀態燈效。

安裝
將此儲存庫下載為 .zip 並透過 Arduino IDE 的 匯入 .zip 程式庫 功能安裝。

依賴項：請確保您已安裝 Adafruit NeoPixel 函數庫。

快速開始
1. 使用預設腳位 (Default Pinout)
本庫內建了 DUAL2S 專用開發板的 GPIO 定義，您可以直接引用 DUAL2S_HW 命名空間。

C++
#include <dual2s.h>

// 初始化馬達與蜂鳴器 (使用預設腳位)
Motor m1(DUAL2S_HW::M1A, DUAL2S_HW::M1B);
Buzzer bz(DUAL2S_HW::BUZZER);

void setup() {
    bz.soundBoot(); // 播放開機音
}

void loop() {
    bz.update(); // 必須呼叫以維持非阻塞音效播放
}
2. 舵機控制範例 (MG995)
C++
ServoMotor myservo(DUAL2S_HW::MG18);

void setup() {
    // 限制在 0~180 度，開機回到 90 度
    myservo.begin(0, 180, 90); 
}

void loop() {
    myservo.write(45); // 立即轉動到 45 度
    delay(1000);
}
硬體定義參考
以下為本庫內建的 DUAL2S_HW 部分預設腳位：

馬達：M1(14, 32), M2(12, 33), M3(25, 26), M4(23, 22)

蜂鳴器：15

MG995 舵機：18, 19, 2

循跡感測器：34 (L), 35 (M), 39 (R)

電池偵測：36

授權資訊
本專案採用 MIT 授權協議。
