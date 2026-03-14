# dual2s Library for GoSUMO相撲機器人 | esp32
這是一個在Arduino IDE環境下支援esp32 / dual2s控制器 / GoSUMO相撲擂台 / AIoT微專題的多工多用途函數庫，提供了：
* **指示元件**: ws2812b全彩LED、蜂鳴器Buzzer
* **環境感測**: 三路超音波、三通道循線(近紅外線)、供電電壓偵測
* **制動元件**: MG995/SG90s伺服馬達、四路獨立正反轉可調速直流電機控制
* **遙控通訊**: PS3 遙控器整合

---

## ✨ 最佳編譯環境
ESP32 Arduino Core v3.3.5

---

## 🚀 快速開始
### 1. 硬體需求
* **核心板**: ESP32 (建議使用 Node32s 腳位定義)
* **驅動板**: dual2s多功能運動控制器，詳細資訊：[dual2s/mini多功能運動控制器](https://yesio.net/srl/dual2s-v2_2025/)。
* **電源**: 2S 鋰電池 (7.4V - 8.4V)

### 2. 安裝方式
第一次玩ESP32的朋友，請直接參考【完整安裝教學】
####簡要說明 - 如圖：
1.開啟ArduinoIDE編輯器，實作版本ArduinoIDE v2.3.6以上。
2.點選左側【函數庫管理】，在搜尋窗中輸入【yesio】或【dual2s】。
3.安裝最新版dual2s函數。 
#### [完整安裝教學](https://yesio.net/srl/dual22-ide/)

---

## 🛠️ dual2s 核心功能教學
#### [【顯示】狀態燈使用 | ws2812b](https://yesio.net/srl/dual2sbasic-ws2812b/)
#### [【聲音】蜂鳴器 | buzzer](https://yesio.net/srl/dual2sbasic-buzzer/)
#### [【感測】超音波 | HCSR04](https://yesio.net/srl/dual2sbasic-hcsr04/)
#### [三通道循線 |IR3CH-line](https://yesio.net/srl/dual2sbasic-ir3ch-linefollowing/)
#### [三通道循線 | IR3CH-相撲](https://yesio.net/srl/dual2sbasic-ir3ch-triangle/)
#### [舵機 | ServoMotor)](https://yesio.net/srl/dual2sbasic-servo/)
#### [【制動】 DC馬達 | Motor](https://yesio.net/srl/dual22-2s-dcmotor/)
#### [電壓監測 (Power)](https://yesio.net/srl/dual2sbasic-power/)
#### [【通訊】 無線搖桿 | PS3GamePad](https://yesio.net/srl/dual2sbasic-ps3/)

