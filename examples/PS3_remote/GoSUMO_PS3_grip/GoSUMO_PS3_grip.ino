/*=====================================================================================
yesio.net / 2026.04.26 / by nick
# Filename：GoSUMO_PS3.ino
# Function：使用PS3搖桿控制GoSUMO運動之使用範例-【抓地胎模式】 / GoSUMO Grip motion control by PS3 remote.
  - 左右手控制：RIGHT_HAND_PS3
  - 低電壓警示：鋰電池電壓低於6V時, 黃光5秒間隔閃爍。
  - PS3斷線重新啟動連線：2秒檢查
  - 降速微動模式：右手模式下,常按不放L2鍵進入降速微動 (左手模式 = R2鍵)。
  - 投石機功能：PS3搖桿【UP鍵】啟動機構動作。
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

//PS3搖桿預設右手控制, 註解即可換為左手控制
#define RIGHT_HAND_PS3

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

unsigned long lastTime_Battery = 0;   // 紀錄最後一次收到搖桿訊號的時間
unsigned long lastTime_PS3 = 0;   // 紀錄最後一次收到搖桿訊號的時間
bool PS3isConnectedSafe = false;    // 自定義的安全連線狀態

bool LOWPWR_Blink = false;
int  SpeedMAX = 1023;
int  SpeedSLOW = 500;

bool fDoCatapult = false; 

void notify() {
  lastTime_PS3 = millis();      // 只要搖桿有動作，就更新時間
  PS3isConnectedSafe = true;    // 標記為有效連線
  
  if (Ps3.data.button.up)    fDoCatapult = true; //Serial.println("按下：上鍵");
  //if (Ps3.data.button.down)  Serial.println("按下：下鍵");

#ifdef RIGHT_HAND_PS3  
  int topSpeed = Ps3.data.button.l2 ? SpeedSLOW : SpeedMAX;
  int stick_y = Ps3.data.analog.stick.ry;
  int stick_x = Ps3.data.analog.stick.rx;
#else
  int topSpeed = Ps3.data.button.r2 ? SpeedSLOW : SpeedMAX;
  int stick_y = Ps3.data.analog.stick.ly;
  int stick_x = Ps3.data.analog.stick.lx;
#endif  

  if(stick_y <= -10 && abs(stick_x) < 10) gs.act(GoSUMO::FORWARD, map(stick_y, -10, -128, 150, topSpeed));
  else if(stick_y >= 10 && abs(stick_x) < 10) gs.act(GoSUMO::BACKWARD, map(stick_y, 10, 127, 150, topSpeed));
  else if(stick_x >= 10 && abs(stick_y) < 10) gs.act(GoSUMO::TURN_RIGHT, map(stick_x, 10, 127, 150, topSpeed));
  else if(stick_x <= -10 && abs(stick_y) < 10) gs.act(GoSUMO::TURN_LEFT, map(stick_x, -10, -128, 150, topSpeed));
  else gs.stop();

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

  //蜂鳴器指示 - 系統啟動
  bz.alarm(600);

  Ps3.attach(notify);              // 註冊按鍵變更時的回呼函式
  Ps3.attachOnConnect(onConnect);  // 註冊成功連線時的回呼函式
  Ps3.begin("20:00:00:00:90:00");  // 依據你的搖桿位址修改藍牙位址。
}

void loop() {

  // 排程檢查區 ==========================================
  // 檢查 PS3 連線狀態 - 超過2秒未更新PS3連線旗標則視為斷線,強制ESP32全機重啟,啟動重新連線。
  if (PS3isConnectedSafe && (millis() - lastTime_PS3 > 2000)) { reconnectPS3();   }

  // --- 原有的電池檢查邏輯 ---
  if (millis() - lastTime_Battery > 5000) {
    lastTime_Battery = millis();
    float currentVolt = pwr.read();
    Serial.println(currentVolt);
    if (currentVolt < 5.5){
      if(!LOWPWR_Blink){ led.fillColor(stateLED::YELLOW); LOWPWR_Blink = true; }
      else{ led.clear(); LOWPWR_Blink = false;}
    }
  }

  if(fDoCatapult){ Catapult(); } //呼叫投石機

  // ==========================================

} //End_of_LOOP

void Catapult(){
  fDoCatapult = false;
  s18.write(0);  // 設定目標角度
  delay(1000);    // 等待 1 秒讓舵機轉到位  
  s18.write(170);
}
