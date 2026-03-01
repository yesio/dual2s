# dual2s Library for GoSUMO相撲機器人 | esp32
這是一個支援esp32 / dual2s控制器 / GoSUMO相撲擂台 / AIoT微專題的多工多用途函數庫，提供了：
* **指示元件**: ws2812b全彩LED、蜂鳴器Buzzer
* **環境感測**: 三路超音波、三通道循線(近紅外線)、供電電壓偵測
* **制動元件**: MG995/SG90s伺服馬達、四路獨立正反轉可調速直流電機控制
* **遙控通訊**: PS3 遙控器整合

---

## 🚀 快速開始
### 1. 硬體需求
* **核心板**: ESP32 (建議使用 Node32s 腳位定義)
* **驅動板**: dual2s多功能運動控制器，詳細資訊：[dual2s/mini多功能運動控制器](https://yesio.net/srl/dual2s-v2_2025/)。
* **電源**: 2S 鋰電池 (7.4V - 8.4V)

### 2. 安裝方式
1. 下載此儲存庫並解壓縮。
2. 將資料夾放入您的 Arduino `libraries` 目錄。
3. 重新啟動 Arduino IDE。

---

## 🛠️ 核心功能教學
### [全彩LED (ws2812b)](https://yesio.net/srl/dual2s-v2_2025/)。
### 蜂鳴器 (Buzzer)
### 電壓監測 (Power)
### 超音波 (HCSR04)
### 三通道循線 (IR3CH)
### MG995/SG90s伺服馬達 (ServoMotor)
### DC電機控制 (Motor)
### GoSUMO車體 (GoSUMO)
