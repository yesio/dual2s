//1. 標頭檔，引入函數庫------------
#include <dual2s.h>
#include "BluetoothSerial.h"

//2. 全域變數---------------------
//前置處理器、全域變數、物件宣告
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

//藍牙物件、變數------------------------------------
BluetoothSerial SerialBT;  //藍牙串列物件 
char BTCMD[3] = {0};  //儲存藍牙串列接收到的指令
int PWMspeed = 950;  //儲存藍牙串列接收到的PWM speed數值
int SeekCnt = 0;

//物件宣告-----------------------------------------
//例如：GoSUMO gs;



//3. 初始化函數(標準格式)----------
void setup(){
  Serial.begin(115200);
  SerialBT.begin("GSmetal");  //啟動藍牙串列並設定藍牙裝置名稱

}

//4. 主函數(標準格式)：程式入口-----
void loop(){
  if (Serial.available()) { SerialBT.write(Serial.read());  }
  if (SerialBT.available()) {
    int i;
    for (i=0; i<4; i++){ BTCMD[i] = SerialBT.read(); } 
  }
  if(atoi(BTCMD)>1) { PWMspeed = atoi(BTCMD);}
  
  /*藍牙控制命令對應GoSUMO動作模式，由此開始。*/
  if(BTCMD[0] == 'F') { //例如，收到手機傳來的【字元F】指令時，要求GoSUMO進行某種指定動作/功能
    //某種指定動作、功能，也可為指定函數入口...

  }           

}

//5. 副函數區：個人定義的程式區段----

