#include <dual2s.h>
#include "BluetoothSerial.h"

//DC電機GPIO
#define PIN_MOTO_1A  14
#define PIN_MOTO_1B  32
#define PIN_MOTO_2A  12
#define PIN_MOTO_2B  33
#define PIN_MOTO_3A  25
#define PIN_MOTO_3B  26
#define PIN_MOTO_4A  23
#define PIN_MOTO_4B  22

//蜂鳴器GPIO
#define PIN_BUZZER   15

//超音波GPIO
#define PIN_USC_FRONT_ECHO 13
#define PIN_USC_FRONT_TRIG 27
#define PIN_USC_RIGHT_ECHO 5
#define PIN_USC_RIGHT_TRIG 4
#define PIN_USC_LEFT_ECHO 19
#define PIN_USC_LEFT_TRIG 18

//短距紅外線GPIO
#define PIN_IR_LEFT    34
#define PIN_IR_MIDDLE  35
#define PIN_IR_RIGHT   39

//供電偵測GPIO
#define PIN_BATTERY    36

//預設使用的PWM通道
#define PWM_CH_BUZZER 7
#define PWM_CH_M1A    8
#define PWM_CH_M1B    9
#define PWM_CH_M2A    10
#define PWM_CH_M2B    11
#define PWM_CH_M3A    12
#define PWM_CH_M3B    13
#define PWM_CH_M4A    14
#define PWM_CH_M4B    15

BluetoothSerial SerialBT;  //藍牙串列物件 
char BTCMD[3] = {0};  //儲存藍牙串列接收到的指令
int PWMspeed = 950;  //儲存藍牙串列接收到的PWM speed數值
int SeekCnt = 0;

GoSUMO gs;
Buzzer bz(PIN_BUZZER, PWM_CH_BUZZER);
Power  volt(PIN_BATTERY);
IR3CH  ir(PIN_IR_LEFT, PIN_IR_MIDDLE, PIN_IR_RIGHT);
HCSR04 us_F(PIN_USC_FRONT_ECHO, PIN_USC_FRONT_TRIG);
HCSR04 us_R(PIN_USC_RIGHT_ECHO, PIN_USC_RIGHT_TRIG);
HCSR04 us_L(PIN_USC_LEFT_ECHO, PIN_USC_LEFT_TRIG);


void setup(){
  Serial.begin(115200);
  SerialBT.begin("你的指定車名");  //啟動藍牙串列並設定藍牙裝置名稱
  bz.alarm();
}

void loop(){
  if (Serial.available()) { SerialBT.write(Serial.read());  }
  if (SerialBT.available()) {
    int i;
    for (i=0; i<4; i++){ BTCMD[i] = SerialBT.read(); } 
  }
  if(atoi(BTCMD)>1) { PWMspeed = atoi(BTCMD);}
  
  /*藍牙控制命令對應GoSUMO動作模式，由此開始。*/
  if(BTCMD[0] == 'F') { gs.act(GoSUMO::FORWARD, PWMspeed); }           //GoSUMO機器人向前進 
  else if(BTCMD[0] == 'B') { gs.act(GoSUMO::BACKWARD, PWMspeed); }     //GoSUMO機器人向後退 
  else if(BTCMD[0] == 'L') { gs.act(GoSUMO::GO_LEFT, PWMspeed); }	   //GoSUMO機器人左旋轉
  else if(BTCMD[0] == 'R') { gs.act(GoSUMO::GO_RIGHT, PWMspeed); }     //GoSUMO機器人右旋轉
  else if(BTCMD[0] == 'Z') { gs.stop(); }
  else if(BTCMD[0] == 'H') { bz.alarm(); }
  /*Servo Control, start here. */  
  else if(BTCMD[0] == 'A') {   }  //ServoBase Rising
  else if(BTCMD[0] == 'a') {   }  //ServoBase Declining
  else if(BTCMD[0] == 'P') {   }  //ServoPaw OPEN
  else if(BTCMD[0] == 'p') {   }  //ServoPaw CLOSE
  else if(BTCMD[0] == 'z') {   }  //ARM Homing
  else if(BTCMD[0] == 'C') { 
    Serial.print(us_F.ObjDistance());
    Serial.print(", ");  
    Serial.print(us_R.ObjDistance());
    Serial.print(", ");  
    Serial.println(us_L.ObjDistance());  
    delay(500);
    Serial.println("--");  
  }
  //循線感測器分析模式
  else if(BTCMD[0] == 'y') { 
    Serial.println(ir.ReadTCRT(1000, 1));  
  }
}
