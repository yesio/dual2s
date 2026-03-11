/*=====================================================================================
yesio.net / 2026.03.01 / by nick
# Filename：GoSUMO_Dualling.ino
# Function：使用PS3搖桿控制GoSUMO運動之使用範例-【抓地胎模式】 / GoSUMO Grip motion control by PS3 remote.
  - 左右手控制：RIGHT_HAND_PS3
  - 低電壓警示：鋰電池電壓低於6V時, 黃橘光5秒間隔閃爍。
  - PS3斷線重新啟動連線：2秒檢查
  - 降速微動模式：右手模式下,常按不放L2鍵進入降速微動 (左手模式 = R2鍵)。
  - 啟動/結束對戰模式：PS3 - 【L1按鍵】啟動、【R1按鍵】結束
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

//PS3搖桿預設右手控制, 註解即可換為左手控制
#define RIGHT_HAND_PS3

Motor m1(DUAL2S_HW::M1A, DUAL2S_HW::M1B);
Motor m2(DUAL2S_HW::M2A, DUAL2S_HW::M2B);
Motor m3(DUAL2S_HW::M3A, DUAL2S_HW::M3B);
Motor m4(DUAL2S_HW::M4A, DUAL2S_HW::M4B);
GoSUMO gs(&m1, &m2, &m3, &m4);
Buzzer bz(DUAL2S_HW::BUZZER);            //建立物件 - Buzzer, GPIO 15
WS2812B led(DUAL2S_HW::WSLED, 2);        //建立物件 - ws2812b兩顆, GPIO 2
Power pwr(DUAL2S_HW::BATTERY);           //建立物件 - dual2s控制器電壓偵測

IR3CH ir(DUAL2S_HW::IR_L, DUAL2S_HW::IR_M, DUAL2S_HW::IR_R); //建立物件 - ir, 參數順序：左(IR1), 中(IR2), 右(IR3)

// =================【逃脫狀態機變數】=================
bool isEvading = false;        // 是否正在執行逃脫動作
byte currentEvadeState = 0;    // 觸發逃脫時的 IR 狀態 (1~7)
int evadePhase = 0;            // 逃脫連續技的階段 (1: 旋轉/後退, 2: 往前脫離)
unsigned long evadeTimer = 0;  // 逃脫動作的計時器

// 逃脫參數設定 (依照你的馬達轉速調整毫秒數)
const int TURN_TIME = 150;     // 「小旋轉」的時間
const int FORWARD_TIME = 200;  // 「前進小距離」的時間
const int ESCAPE_SPEED = 1023; // 逃脫時的生死時速


//時間戳記
unsigned long lastTime_Battery = 0;   // 紀錄最後一次收到搖桿訊號的時間
unsigned long lastTime_PS3 = 0;   // 紀錄最後一次收到搖桿訊號的時間

bool PS3isConnectedSafe = false;    // 自定義的安全連線狀態

bool LOWPWR_Blink = false;
int  SpeedMAX = 1023;
int  SpeedSLOW = 500;

bool fGoDUALLING = false;

void notify() {
  lastTime_PS3 = millis();      // 只要搖桿有動作，就更新時間
  PS3isConnectedSafe = true;    // 標記為有效連線
  
  if (Ps3.data.button.l1){ fGoDUALLING = true;   Serial.println("GoDualling");}    //上鍵:手臂抬起
  if (Ps3.data.button.r1){ fGoDUALLING = false; Serial.println("STOP GoDualling"); }  //下鍵:手臂放下

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
  led.setStatus(WS2812B::CONNECTED);
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
      if(!LOWPWR_Blink){ led.setStatus(WS2812B::LOWPWR); LOWPWR_Blink = true; }
      else{ led.clear(); LOWPWR_Blink = false;}
    }
  }

  // ==========================================
  if(!fGoDUALLING) return;

  // 1. ================= 【背景感測器更新區】 =================
  //bz.update();
  //us_F.update(); 
  ir.update(2000, IR3CH::BLACK_LINE, false); 
  // ... 其他 update ...

  // 2. ================= 【最高優先級：邊界逃脫機制】 =================
  byte irState = ir.getState();

} //End_of_LOOP
