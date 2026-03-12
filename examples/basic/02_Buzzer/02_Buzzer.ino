/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：02_Buzzer.ino
# Function：蜂鳴器(Buzzer類別)使用範例
  Test Code of dual2S's Buzzer CLASS.
# Buzzer Pin G15 in dual2s HW  
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

// 宣告 Buzzer 物件
// dual2s G15控制 蜂鳴器, DUAL2S_HW::BUZZER
// 參數：控制腳位
Buzzer bz(DUAL2S_HW::BUZZER);

// 測試流程控制變數
unsigned long lastActionTime = 0;
int testStep = 0;
unsigned long waitTime = 3000; // 新增：預設每個步驟等待 3 秒

#define Song_ImperialMarch //使用帝國進行曲或小蜜蜂測試
#ifdef Song_ImperialMarch 
  // 威風凜凜的帝國進行曲  
  constexpr uint16_t Eb4 = 311; // 降 Mi
  constexpr uint16_t Fs4 = 370; // 升 Fa / 降 Sol
  constexpr uint16_t Ab4 = 415; // 降 La
  constexpr uint16_t Bb4 = 466; // 降 Si
  constexpr uint16_t Db5 = 554; // 高音降 Re
  constexpr uint16_t Eb5 = 622; // 高音降 Mi
  constexpr uint16_t Fs5 = 740; // 高音升 Fa
  const Note mySong[] = {
    // 登、登、登、(登~登)
    {Pitch::Sol_4, 400}, {Pitch::Sol_4, 400}, {Pitch::Sol_4, 400}, {Eb4, 300}, {Bb4, 100}, 
    // 登、(登~登)、登——
    {Pitch::Sol_4, 400}, {Eb4, 300}, {Bb4, 100}, {Pitch::Sol_4, 800},
    // 叮、叮、叮、(叮~登)
    {Pitch::Re_5, 400}, {Pitch::Re_5, 400}, {Pitch::Re_5, 400}, {Eb5, 300}, {Bb4, 100},
    // 登、(登~登)、登—— 
    {Fs4, 400}, {Eb4, 300}, {Bb4, 100}, {Pitch::Sol_4, 800},

    // 登(高)、登(低)、登(高)
    {Pitch::Sol_5, 400}, {Pitch::Sol_4, 300}, {Pitch::Sol_4, 100}, {Pitch::Sol_5, 400},
    // 降登、登、(登~登~登) -> 經典的半音下行
    {Fs5, 300}, {Pitch::Fa_5, 100}, {Pitch::Mi_5, 150}, {Eb5, 150}, {Pitch::Mi_5, 400},
    // 休止符 + 降La、降Re、Do、Si
    {0, 200}, {Ab4, 200}, {Db5, 400}, {Pitch::Do_5, 300}, {Pitch::Si_4, 100},
    // 降Si、La、降Si
    {Bb4, 150}, {Pitch::La_4, 150}, {Bb4, 400},
    // 休止符 + 降Mi、降Sol、降Mi、降Sol
    {0, 200}, {Eb4, 200}, {Fs4, 400}, {Eb4, 300}, {Fs4, 100},
    // 降Si、Sol、降Si、Re(高音延伸結尾)
    {Bb4, 400}, {Pitch::Sol_4, 300}, {Bb4, 100}, {Pitch::Re_5, 800}
  };
#else
  //小蜜蜂
  const Note mySong[] = {
  // 嗡嗡嗡，大家一起勤做工 (5 3 3, 4 2 2)
    {Pitch::Sol_4, 300}, {Pitch::Mi_4, 300}, {Pitch::Mi_4, 600},
    {Pitch::Fa_4, 300}, {Pitch::Re_4, 300}, {Pitch::Re_4, 600},
    
    // 來匆匆，去匆匆，做工興味濃 (1 2 3 4, 5 5 5)
    {Pitch::Do_4, 300}, {Pitch::Re_4, 300}, {Pitch::Mi_4, 300}, {Pitch::Fa_4, 300},
    {Pitch::Sol_4, 300}, {Pitch::Sol_4, 300}, {Pitch::Sol_4, 600},
    
    // 天暖花好不做工，將來哪裡好過冬 (5 3 3, 4 2 2)
    {Pitch::Sol_4, 300}, {Pitch::Mi_4, 300}, {Pitch::Mi_4, 600},
    {Pitch::Fa_4, 300}, {Pitch::Re_4, 300}, {Pitch::Re_4, 600},
    
    // 嗡嗡嗡，大家一起勤做工 (1 3 5 5, 1)
    {Pitch::Do_4, 300}, {Pitch::Mi_4, 300}, {Pitch::Sol_4, 300}, {Pitch::Sol_4, 300},
    {Pitch::Do_4, 1200}
  };
#endif 

void setup() {
  Serial.begin(115200);
  Serial.println("=== Buzzer 音效測試開始 ===");

  // ==========================================
  // 測試 1：阻塞式警報 (適合開機或嚴重錯誤時使用)
  // ==========================================
  Serial.println("測試：阻塞式 alarm()");
  bz.alarm();       // 預設 800Hz 警報
  delay(500);
  bz.alarm(1200);   // 自訂 1200Hz 警報
  delay(1000);

  // ==========================================
  // 測試 2：基礎持續發聲 (tone 與 noTone)
  // ==========================================
  Serial.println("測試：基礎 tone() 與 noTone()");
  bz.tone(Pitch::Do_4, 127); // 發出中央 Do，音量一半 (127/255)
  delay(1000);               // 讓聲音持續 1 秒
  bz.noTone();               // 手動關閉聲音
  delay(1000);

  Serial.println("=== 進入非阻斷背景播放測試 ===");
  lastActionTime = millis();
}

void loop() {
  // 1. ================= 【背景音效更新區】 =================
  // 【必須】這行一定要放在 loop 最頂端，不可省略！
  // 它負責在背景檢查音符是否該切換了，不佔用 CPU 時間
  bz.update();
  uint16_t noteCount = sizeof(mySong) / sizeof(mySong[0]); 

  
  // 2. ================= 【狀態切換測試區】 =================
  // 利用 millis() 每隔 3 秒 (3000ms) 觸發下一個音效，證明主迴圈沒有被卡死
  if (millis() - lastActionTime > waitTime) {
    lastActionTime = millis();
    testStep++;
    
    waitTime = 3000; // 每次進入新步驟前，先把等待時間重置為 3 秒

    switch(testStep) {
      case 1:
        Serial.println("播放：開機音效 (soundBoot)");
        bz.soundBoot();
        break;
        
      case 2:
        Serial.println("播放：連線音效 (soundConnect)");
        bz.soundConnect();
        break;
        
      case 3:
        Serial.println("播放：錯誤警告 (soundError)");
        bz.soundError();
        break;
        
      case 4:
        Serial.println("播放：攻擊警報 (soundAttack)：帝國進行曲");
        bz.soundAttack();
        break;

      case 5:
        Serial.println("播放：提示音 (soundNotify)");
        bz.soundNotify(); 
        break;

      case 6:
        Serial.println("播放：自訂旋律 (playMelody)");
        bz.playMelody(mySong, noteCount); 
         waitTime = 15000;
        break;
        
      case 7:
        Serial.println("=== 測試循環結束，重新開始 ===");
        testStep = 0; // 歸零重新測試
        break;
    }
  }
}