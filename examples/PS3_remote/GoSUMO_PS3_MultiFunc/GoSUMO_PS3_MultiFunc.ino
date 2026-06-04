/*=====================================================================================
yesio.net / 2026.06.4 / by nick (Modified for Tank & Omni integration)
# Filename：GoSUMO_PS3_MultiFunc.ino
# Function：使用PS3搖桿控制GoSUMO運動之使用範例
  - 控制模式區分 (透過 #define 切換):
    1. 左右手控制 (預設單搖桿抓地胎)
    2. 坦克模式 (TANK_PS3)
    3. 麥克納姆輪模式 (OMNI_MOTION)
  - 低電壓警示：鋰電池電壓低於6V時, 黃光5秒間隔閃爍。
  - PS3斷線重新啟動連線：2秒檢查
  - 降速微動模式：依據左右手模式，按住 L2 或 R2 鍵降速。
  - 投石機功能：PS3搖桿【UP鍵⬆️】啟動機構動作。
  - 鏟斗功能：PS3搖桿【三角🔼】舉起，【叉叉✖️】放下。
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

// ================== 編譯選項區 ==================
// PS3搖桿預設右手控制, 註解即可換為左手控制
//#define RIGHT_HAND_PS3

// 坦克控制模式 (若取消註解，將覆蓋左右手單搖桿設定，強制使用雙搖桿控制履帶/左右輪)
#define TANK_PS3 

// 麥克納姆輪模式 (若取消註解，搖桿解算將支援平移與斜向，不支援坦克模式下同時使用)
//#define OMNI_MOTION 
// ===============================================

Motor m1(14, 32); 
Motor m2(12, 33);
Motor m3(25, 26); 
Motor m4(23, 22);  //dual2s-mini 23, 27
GoSUMO gs(&m3, &m1, &m4, &m2); //左前, 左後, 右前, 右後 -- GoSUMO

Buzzer bz(DUAL2S_HW::BUZZER);            //建立物件 - Buzzer, GPIO 15
stateLED led(DUAL2S_HW::WSLED, 2);        //建立物件 - ws2812b兩顆, GPIO 2
Power pwr(DUAL2S_HW::BATTERY);           //建立物件 - dual2s控制器電壓偵測

IR3CH ir(DUAL2S_HW::IR_L, DUAL2S_HW::IR_M, DUAL2S_HW::IR_R); //建立物件 - ir, 參數順序：左, 中, 右
ServoMotor s18(DUAL2S_HW::MG18);  //投石機
ServoMotor s19(DUAL2S_HW::MG19);  //鏟斗

unsigned long lastTime_Battery = 0;   // 紀錄最後一次收到搖桿訊號的時間
unsigned long lastTime_PS3 = 0;   // 紀錄最後一次收到搖桿訊號的時間
bool PS3isConnectedSafe = false;    // 自定義的安全連線狀態

bool LOWPWR_Blink = false;
int  SpeedMAX = 1023;
int  SpeedSLOW = 500;

bool fDoCatapult = false; 
bool fBucketRaise = false; // 鏟斗舉起旗標
bool fBucketLower = false; // 鏟斗放下旗標

void notify() {
  lastTime_PS3 = millis();      // 只要搖桿有動作，就更新時間
  PS3isConnectedSafe = true;    // 標記為有效連線
  
  // 機構按鍵綁定
  if (Ps3.data.button.up)       fDoCatapult = true;  //⬆️鍵，發球
  if (Ps3.data.button.triangle) fBucketRaise = true; //🔼鍵，舉起鏟斗
  if (Ps3.data.button.cross)    fBucketLower = true; //✖️鍵，放下鏟斗

  // 減速微動判斷 (按下 L2 或 R2 時減速)
  int topSpeed = (Ps3.data.button.l2 || Ps3.data.button.r2) ? SpeedSLOW : SpeedMAX;  

  int threshold = 10; // 死區設定

// ======== 坦克模式解算 ========
#ifdef TANK_PS3
  int stick_ly = Ps3.data.analog.stick.ly;
  int stick_ry = Ps3.data.analog.stick.ry;
  int spd = map(max(abs(stick_ry), abs(stick_ly)), threshold, 128, 150, topSpeed); 
  
  if (stick_ly < -threshold){ Tank_LW(1, spd); } //左輪前進
  else if (stick_ly > threshold){ Tank_LW(2, spd); } //左輪後退
  else{ Tank_LW(0, 0); } //左輪停止

  if (stick_ry < -threshold){ Tank_RW(1, spd); } //右輪前進
  else if (stick_ry > threshold){ Tank_RW(2, spd); } //右輪後退
  else{ Tank_RW(0, 0); } //右輪停止

// ======== 單搖桿模式解算 ========
#else 
  #ifdef RIGHT_HAND_PS3  
    int stick_y = Ps3.data.analog.stick.ry;
    int stick_x = Ps3.data.analog.stick.rx;
  #else
    int stick_y = Ps3.data.analog.stick.ly;
    int stick_x = Ps3.data.analog.stick.lx;
  #endif 
  
  int spd = map(max(abs(stick_x), abs(stick_y)), threshold, 128, 150, topSpeed); // 統一速度映射

  // --- 麥克納姆輪解算 (OMNI_MOTION) ---
  #ifdef OMNI_MOTION
    if (stick_y < -threshold && abs(stick_x) < threshold) { gs.act(GoSUMO::FORWARD, spd); } 
    else if (stick_y > threshold && abs(stick_x) < threshold) { gs.act(GoSUMO::BACKWARD, spd); } 
    else if (stick_x < -threshold && abs(stick_y) < threshold) { gs.act(GoSUMO::STRAFE_LEFT, spd); } 
    else if (stick_x > threshold && abs(stick_y) < threshold) { gs.act(GoSUMO::STRAFE_RIGHT, spd); } 
    else if (stick_y < -threshold && stick_x < -threshold) { gs.act(GoSUMO::DIAG_FL, spd); } 
    else if (stick_y < -threshold && stick_x > threshold) { gs.act(GoSUMO::DIAG_FR, spd); } 
    else if (stick_y > threshold && stick_x < -threshold) { gs.act(GoSUMO::DIAG_BL, spd); } 
    else if (stick_y > threshold && stick_x > threshold) { gs.act(GoSUMO::DIAG_BR, spd); } 
    else { gs.stop(); } 

    // 麥克納姆輪專屬微調 (方塊左轉、圈圈右轉)
    if (Ps3.data.button.circle){ gs.act(GoSUMO::TURN_RIGHT, topSpeed); delay(10); gs.stop();}
    if (Ps3.data.button.square){ gs.act(GoSUMO::TURN_LEFT, topSpeed); delay(10); gs.stop(); }

  // --- 一般抓地胎解算 ---
  #else
    if (stick_y < -threshold && abs(stick_x) < threshold) { gs.act(GoSUMO::FORWARD, spd); }
    else if (stick_y > threshold && abs(stick_x) < threshold) { gs.act(GoSUMO::BACKWARD, spd); }
    else if (stick_x > threshold && abs(stick_y) < threshold) { gs.act(GoSUMO::TURN_RIGHT, spd); }
    else if (stick_x < -threshold && abs(stick_y) < threshold) { gs.act(GoSUMO::TURN_LEFT, spd); }
    else { gs.stop(); }
  #endif // OMNI_MOTION 結束
#endif // TANK_PS3 結束
}

void onConnect() {
  //蜂鳴器指示 - 連線
  bz.alarm(800);
  Serial.println("PS3 控制器已連接！");
}

void reconnectPS3() {
  gs.stop(); // 確保重啟期間馬達不運轉
  delay(100);
  ESP.restart(); // 強制 ESP32 全機重啟
}

void setup() {
  Serial.begin(115200);
  led.begin();

  s18.begin(0, 180, 180); //投石機
  s19.begin(0, 180, 180); //鏟斗

  //蜂鳴器指示 - 系統啟動
  bz.alarm(600);

  Ps3.attach(notify);              // 註冊按鍵變更時的回呼函式
  Ps3.attachOnConnect(onConnect);  // 註冊成功連線時的回呼函式
  Ps3.begin("20:00:00:00:43:85");  // 依據你的搖桿位址修改藍牙位址。
}

void loop() {

  // 排程檢查區 ==========================================
  // 檢查 PS3 連線狀態 - 超過2秒未更新PS3連線旗標則視為斷線,強制ESP32全機重啟,啟動重新連線。
  if (PS3isConnectedSafe && (millis() - lastTime_PS3 > 2000)) { reconnectPS3();   }

  // --- 原有的電池檢查邏輯 ---
  if (millis() - lastTime_Battery > 5000) {
    lastTime_Battery = millis();
    float currentVolt = pwr.read();
    // Serial.println(currentVolt);
    if (currentVolt < 5.5){
      if(!LOWPWR_Blink){ led.fillColor(stateLED::YELLOW); LOWPWR_Blink = true; }
      else{ led.clear(); LOWPWR_Blink = false;}
    }
  }

  // --- 機構作動區 ---
  if(fDoCatapult){ Catapult(); } //呼叫投石機
  if(fBucketRaise){ Bucket_Raise(); } //呼叫鏟斗舉起
  if(fBucketLower){ Bucket_Lower(); } //呼叫鏟斗放下

  // ==========================================

} //End_of_LOOP

// --- 機構與底盤動作函式區 ---
//投石機動作
void Catapult(){
  fDoCatapult = false;
  s18.write(0);  // 設定目標角度
  delay(1000);    // 等待 1 秒讓舵機轉到位  
  s18.write(170);
}

//舉起鏟斗
void Bucket_Raise(){
  fBucketRaise = false;
  s19.write(0); // 直接寫入舉起角度, 依據你的機構角度填入(0-180)
}

//放下鏟斗
void Bucket_Lower(){
  fBucketLower = false;
  s19.write(170); // 直接寫入放下角度, 依據你的機構角度填入(0-180)
}

#ifdef TANK_PS3
void Tank_LW(uint8_t dir, int spd){
  switch(dir){
    case 0://STOP
      m1.stop(); m3.stop();
      break;
    case 1://Forward
      m1.act(Motor::CCW, spd); m3.act(Motor::CCW, spd);
      break;
    case 2://Backward
      m1.act(Motor::CW, spd); m3.act(Motor::CW, spd);
      break;
  }
}

void Tank_RW(uint8_t dir, int spd){
  switch(dir){
    case 0://STOP
      m2.stop(); m4.stop();
      break;
    case 1://Forward
      m2.act(Motor::CW, spd); m4.act(Motor::CW, spd);
      break;
    case 2://Backward
      m2.act(Motor::CCW, spd); m4.act(Motor::CCW, spd);
      break;
  }
}
#endif