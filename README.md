# dual2s Library for ESP32 GoSUMO
這是一個專為 ESP32 開發的機器人控制函數庫，支援馬達驅動、超音波、WS2812B 燈號及 PS3 遙控器整合。

---

## 🚀 快速開始
### 1. 硬體需求
* **核心板**: ESP32 (建議使用 Node32s 腳位定義)
* **驅動板**: dual2s 客製化控制器
* **電源**: 2S 鋰電池 (7.4V - 8.4V)

### 2. 安裝方式
1. 下載此儲存庫並解壓縮。
2. 將資料夾放入您的 Arduino `libraries` 目錄。
3. 重新啟動 Arduino IDE。

---

## 🛠️ 核心功能說明

### 電壓監測 (Power)
修正後的電壓公式，提供更精準的電池數據。
```cpp
Power pwr(DUAL2S_HW::BATTERY);
float v = pwr.read(); // 取得當前電壓
