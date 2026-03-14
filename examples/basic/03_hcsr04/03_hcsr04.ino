/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：03_hcsr04.ino
# Function：超音波(HCSR04類別)使用範例
  Test Code of dual2S's HCSR04 CLASS.
# Buzzer Pin G15 in dual2s HW  
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

//使用dual2s-mini請開啟此定義, dual2s-mini硬體只預設前方超音波。當然自行外接不再此限喔。
//#define DUAL2S_MINI

// 宣告三個方向的超音波物件，直接使用 dual2s.h 內建的硬體腳位定義
// 參數：(Echo 腳位, Trig 腳位)
HCSR04 us_F(DUAL2S_HW::USC_F_ECHO, DUAL2S_HW::USC_F_TRIG); // 前方感測器

#ifndef DUAL2S_MINI
HCSR04 us_L(DUAL2S_HW::USC_L_ECHO, DUAL2S_HW::USC_L_TRIG); // 左方感測器
HCSR04 us_R(DUAL2S_HW::USC_R_ECHO, DUAL2S_HW::USC_R_TRIG); // 右方感測器
#endif

// 測試流程控制變數
unsigned long lastPrintTime = 0;
int testStep = 0;

// 設定攻擊觸發距離：當目標進入 20 公分內視為發現對手
const uint8_t ATTACK_RANGE = 20; 

void setup() {
  Serial.begin(115200);
  Serial.println("=== HCSR04 超音波陣列測試開始 ===");
  Serial.println("初始化完成，準備讀取數值...");
}

void loop() {
  // 使用 millis() 實現非阻斷式讀取，每 150 毫秒更新一次畫面
  // 提示：超音波量測需要時間等待聲波返回，不建議將更新頻率設得太高（例如低於 50ms），
  // 否則容易發生「聲波互相干擾」或「抓不到回波」的現象。
  if (millis() - lastPrintTime >= 150) {
    lastPrintTime = millis();

    testStep++;
    if (testStep > 200) {
      testStep = 1; 
    }

    // ---------------------------------------------------------
    // 測試方法 1：ObjDistance() - 取得實際距離 (公分)
    // 回傳值：float。若超出量測範圍 (大約 340cm 以上，p == 0) 會回傳 999.0
    // ---------------------------------------------------------
    if(testStep <= 100){
      if (testStep == 1) {
        Serial.println("\n\n===========================================");
        Serial.println("--- 測試階段 1：【ObjDistance()】, 約 15 秒 ---");
        Serial.println("===========================================");
      }
      float distF = us_F.ObjDistance();

#ifndef DUAL2S_MINI
      float distL = us_L.ObjDistance();
      float distR = us_R.ObjDistance();
#endif

      Serial.print(testStep);
      Serial.print("【距離(cm)】 前: "); 
      if(distF >= 999.0) Serial.print("---  "); else Serial.print(distF, 1);  
#ifndef DUAL2S_MINI        
      Serial.print(" | 左: "); 
      if(distL >= 999.0) Serial.print("---  "); else Serial.print(distL, 1);    
      Serial.print(" | 右: "); 
      if(distR >= 999.0) Serial.print("---  "); else Serial.print(distR, 1);
#endif      
      Serial.println(); 
    }

    // ---------------------------------------------------------
    // 測試方法 2：ObjSeeking(thresh) - 快速索敵 (布林值)
    // 說明：回傳 True/False，比對目標是否進入我們設定的範圍內。
    // 在實戰主迴圈中，用這個方法判斷會比用 ObjDistance() 更快！
    // ---------------------------------------------------------
    else if (testStep > 100 && testStep <= 200) {
      if (testStep == 101) {
        Serial.println("\n\n===========================================");
        Serial.println("--- 測試階段 2：【ObjSeeking()】, 約 15 秒 ---");
        Serial.println("===========================================");
      }
      
      Serial.print("["); Serial.print(testStep); Serial.print("] ");
      Serial.printf("==> 【索敵狀態 %dcm】: ", ATTACK_RANGE);    
      
      if (us_F.ObjSeeking(ATTACK_RANGE))      { Serial.print("🚨前方發現對手！(準備衝撞)");  } 
#ifndef DUAL2S_MINI       
      else if (us_L.ObjSeeking(ATTACK_RANGE)) { Serial.print("👈左側有目標！(準備左轉)");    }
      else if (us_R.ObjSeeking(ATTACK_RANGE)) { Serial.print("👉右側有目標！(準備右轉)");    }
#endif      
      else                                    { Serial.print("🟢安全 (巡邏中...)");     }
      
      Serial.println(); // 只有在這個階段才換行
    }
  }
}