#include <dual2s.h>
#include <PS2X_lib.h>
#include <Servo.h>

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

GoSUMO gs;
Buzzer bz(PIN_BUZZER, PWM_CH_BUZZER);
#define PS2_DAT    4
#define PS2_CMD    17
#define PS2_SEL    16
#define PS2_CLK    21
#define pressures  false
#define rumble     false
PS2X ps2x;
int error = -1;
byte PS2X_type = 0;
byte vibrate = 0;
int tryNum = 1;

/*-- 宣告區(全域變數) --*/
int SeekCnt = 0;
unsigned long prevMillis = 0;  // 暫存經過時間（毫秒）
const long interval = 5000;     // 指定間隔時間5秒(5000毫秒)。
//int PWMSpeed_MAX = 1023; //DC電機最大PWM值(速度)
//int PWMSpeed_MIN = 150;  //DC電機最小PWM值(速度)

//Servo Motor 變數區 & 機械臂微調區 -------
//重要，詳見影片介紹：https://youtu.be/PnZQBKgZXok
Servo servoBase;
Servo servoPaw;
int homeBase = 90; //機械臂原點角度。
int homePaw = 90;  //機械爪原點角度。

int posBase = homeBase;
int posBaseMAX = 170; //機械臂最大抬升角度。
int posBaseMIN = 90;  //機械臂最小降低角度。
int posPaw = homePaw;
int posPawMAX = 90;   //機械爪最大開啟角度。
int posPawMIN = 30;   //機械爪最小閉合角度。

void setup()
{
  Serial.begin(115200);
  bz.alarm();
  PS2X_INIT(); //注意，此將占用DUAL右側超音波

  //ServoMotor初始化
  servoBase.attach(19); //servo-2、棕線
  servoPaw.attach(18);  //servo-1、白線
  ServoARM("Home", "");    
}

void loop()
{
  if(PS2X_type != 2){ 
    ps2x.read_gamepad(false, vibrate);  //讀取PS2命令
    Serial.println(ps2x.Analog(PSS_RY));
    Serial.println(ps2x.Analog(PSS_RX));

    if(ps2x.Analog(PSS_RY) <= 118){ gs.act(GoSUMO::FORWARD, map(ps2x.Analog(PSS_RY),128,0,150,1023)); }    //前進 
    if(ps2x.Analog(PSS_RY) >= 138){ gs.act(GoSUMO::BACKWARD, map(ps2x.Analog(PSS_RY),128,255,150,1023)); } //後退    
    if(ps2x.Analog(PSS_RX) <= 118){ gs.act(GoSUMO::GO_LEFT, map(ps2x.Analog(PSS_RX),128,0,150,1023)); }    //左旋轉 
    if(ps2x.Analog(PSS_RX) >= 138){ gs.act(GoSUMO::GO_RIGHT, map(ps2x.Analog(PSS_RX),128,255,150,1023)); } //右旋轉
    if((ps2x.Analog(PSS_RX) > 125 && ps2x.Analog(PSS_RX) < 130) && (ps2x.Analog(PSS_RY) > 125 && ps2x.Analog(PSS_RY) < 130)) { gs.stop(); } 

    if(ps2x.Analog(PSS_LX) <= 118){ ServoARM("Paw", "OPEN");  }    
    if(ps2x.Analog(PSS_LX) >= 138){ ServoARM("Paw", "CLOSE"); }  
    if(ps2x.Analog(PSS_LY) <= 118){ ServoARM("Base", "DOWN"); }    
    if(ps2x.Analog(PSS_LY) >= 138){ ServoARM("Base", "UP");   }   

    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) {  ServoARM("Home", "");  } //Servo復位90度
  }  
  delay(15); 
}

/*====== 副程式區，start here. ========*/
void ServoARM(String _ACTION, String _DIR){
	if(_ACTION == "Base"){
	  if((_DIR == "UP")&&(posBase < posBaseMAX )){servoBase.write(++posBase); }
	  if((_DIR == "DOWN")&&(posBase > posBaseMIN )){servoBase.write(--posBase); }
	}
	
	if(_ACTION == "Paw"){
	  if((_DIR == "OPEN")&&(posPaw < posPawMAX )){servoPaw.write(++posPaw); }
	  if((_DIR == "CLOSE")&&(posPaw > posPawMIN )){servoPaw.write(--posPaw);}
	}	
	
	if(_ACTION == "Home"){
    Serial.println("servoHome");
	   servoBase.write(homeBase);
     servoPaw.write(homePaw);
     posPaw = homePaw;
     posBase = homeBase;
	   delay(1000);
	}	
}

void PS2X_INIT(){
 while (error != 0) {
  delay(1000);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  Serial.print("#try config ");  Serial.println(tryNum);  tryNum ++;
 }
 Serial.println(ps2x.Analog(1), HEX);
 PS2X_type = ps2x.readType();
 switch(PS2X_type) {
   case 0: Serial.printf("Unknown Controller type found, type is %d", PS2X_type);  break;
   case 1: Serial.printf("DualShock Controller found, type is %d", PS2X_type);  break;
   case 2: Serial.printf("GuitarHero Controller found, type is %d", PS2X_type);  break;
   case 3: Serial.printf("Wireless Sony DualShock Controller found, type is %d", PS2X_type);  break;
 }
}