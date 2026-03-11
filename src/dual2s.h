#ifndef DUAL2S_H
#define DUAL2S_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h> // 引入底層 LED 驅動引擎
#include "Ps3Controller.h"

/*--------------------------------------------------------*/
// DUAL2S 硬體預設腳位定義 (可選載入)
/*--------------------------------------------------------*/
// 如果使用者在 #include "dual2s.h" 之前 #define NO_DUAL2S_PINS 則不會載入以下定義，避免名稱衝突。
#ifndef NO_DUAL2S_PINS
namespace DUAL2S_HW {
  // DC 電機 (M1~M4)
  constexpr uint8_t M1A = 14; constexpr uint8_t M1B = 32;
  constexpr uint8_t M2A = 12; constexpr uint8_t M2B = 33;
  constexpr uint8_t M3A = 25; constexpr uint8_t M3B = 26;
  constexpr uint8_t M4A = 23; constexpr uint8_t M4B = 22;
  
  //MG995 ServoMotor
  constexpr uint8_t MG18 = 18;
  constexpr uint8_t MG19 = 19;
  constexpr uint8_t MG2 = 2;

  // 蜂鳴器
  constexpr uint8_t BUZZER = 15;
  // ws2812b全彩
  constexpr uint8_t WSLED = 2;

  // 超音波 (Echo, Trig)
  constexpr uint8_t USC_F_ECHO = 13; constexpr uint8_t USC_F_TRIG = 27;
  constexpr uint8_t USC_R_ECHO = 5;  constexpr uint8_t USC_R_TRIG = 4; 
  constexpr uint8_t USC_L_ECHO = 19; constexpr uint8_t USC_L_TRIG = 18;

  // 短距紅外線 (類比輸入)
  constexpr uint8_t IR_L = 34;
  constexpr uint8_t IR_M = 35;
  constexpr uint8_t IR_R = 39;

  // 供電偵測
  constexpr uint8_t BATTERY = 36;
}
#endif


/*--------------------------------------------------------*/
// BUZZER 
/*--------------------------------------------------------*/
// 1. 改用 namespace 與 constexpr 節省記憶體，避免重複定義
namespace Pitch {
  constexpr uint16_t Do_3=131, Re_3=147, Mi_3=165, Fa_3=175, Sol_3=196, La_3=220, Si_3=247;
  constexpr uint16_t Do_4=262, Re_4=294, Mi_4=330, Fa_4=349, Sol_4=392, La_4=440, Si_4=494;
  constexpr uint16_t Do_5=523, Re_5=587, Mi_5=659, Fa_5=698, Sol_5=784, La_5=880, Si_5=988;
}

// 定義音符結構
struct Note {
  uint16_t freq;      // 頻率 (Hz)
  uint16_t duration;  // 持續時間 (毫秒)
};

/*** BUZZER ***/
class Buzzer {
  private:
    byte _pin;
    
    // 背景播放狀態機變數
    const Note* _currentMelody;
    uint16_t _melodyLength;
    uint16_t _noteIndex;
    unsigned long _lastNoteTime;
    bool _isPlaying;

  public:
    Buzzer(byte pin);
    void noTone();
    // 基礎發聲 (注意：這裡移除了 duration 的 delay，改為純發聲)
    void tone(uint16_t frequency, uint8_t volume = 127); 
	
	void alarm(int freq = -1);
    
    // 背景播放引擎：負責在 loop 中推進音符
    void update();
    
    // 播放自訂旋律
    void playMelody(const Note* melody, uint16_t length);

    // 內建的系統豐富音效
    void soundBoot();      // 開機音效
    void soundConnect();   // 連線成功
    void soundError();     // 錯誤警告
    void soundAttack();    // 攻擊/投石警報
};

/*--------------------------------------------------------*/
// WS2812B (全彩狀態指示燈) 
/*--------------------------------------------------------*/
class WS2812B {
  private:
    uint8_t _pin;
    uint16_t _num_leds;
    Adafruit_NeoPixel _strip; // 組合 (Composition)：把 Adafruit 物件包在裡面

  public:
    // 定義機器人的預設燈光狀態
    typedef enum {
      IDLE,       // 待機 (呼吸燈或微弱白光)
      CONNECTED,  // 藍牙已連線 (藍燈)
      SEARCHING,  // 循跡/搜尋敵人中 (黃燈)
      ATTACK,     // 發現敵人/投石攻擊 (紅燈閃爍或常亮)
      ERROR,       // 異常狀態 (紫燈)
	  LOWPWR
    } Status;

    // 建構子：傳入腳位與 LED 數量
    WS2812B(uint8_t pin, uint16_t num_leds);
    
    // 初始化函式 (要在主程式 setup 中呼叫)
    void begin();
    
    // 基礎控制：設定單顆顏色或全亮
    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void clear();
    void setBrightness(uint8_t brightness);
    
    // 進階控制：一鍵切換機器人狀態燈效
    void setStatus(Status state);
};

/*--------------------------------------------------------*/
// POWER 電壓讀取
/*--------------------------------------------------------*/
class Power {
  private:
    uint8_t _pin;
  public:
    Power(uint8_t pin);
    float read();
}; 

/*--------------------------------------------------------*/
// UltraSonic 超音波測距
/*--------------------------------------------------------*/
class HCSR04 {
  private:
    uint8_t _pin_echo;
    uint8_t _pin_trig;

  public:
    typedef enum { USC_FRONT, USC_LEFT, USC_RIGHT } USC_DIR;

    HCSR04(uint8_t pin_echo, uint8_t pin_trig);
    bool ObjSeeking(uint8_t thresh);
    float ObjDistance();
    unsigned long probing();
};  

/*****------- IR3CH (Non-blocking) -------*****/
class IR3CH {
  private:
    uint8_t _pin_L;
    uint8_t _pin_M;
    uint8_t _pin_R;
    
    // 背景狀態機與計時變數
    unsigned long _lastUpdate;
    unsigned long _lastDebugPrint;
    byte _currentState;

  public:
    // 新增：定義循線/邊界的物理顏色
    typedef enum {
      BLACK_LINE, // 數值 > TH 觸發 (吸光，反射弱)
      WHITE_LINE  // 數值 < TH 觸發 (反光，反射強)
    } LineType;
	
    IR3CH(uint8_t pin_L, uint8_t pin_M, uint8_t pin_R);
    
    // 必須在主迴圈呼叫，負責定期在背景更新感測器數值
    void update(uint16_t TH, LineType lineType = BLACK_LINE, bool DEBUG = false);
	
	//一次性讀取3通道數值並轉換為byte數值, traditional way
	byte read3CH(uint16_t TH, LineType lineType = BLACK_LINE, bool DEBUG = false);
    
    // 瞬間取得最新的循跡狀態 (000 ~ 111)，不浪費時間
    byte getState();
    
    // 獨立取得單一感測器的類比原始數值 (供進階演算法使用)
    uint16_t getValL();
    uint16_t getValM();
    uint16_t getValR();
};


/*****------- MOTOR -------*****/
class Motor {
  private:
    uint16_t _m_frequency = 1000;
    uint8_t _m_resolution = 10;
    uint8_t _pin_a;
    uint8_t _pin_b;
    // ESP32 v3.0.0 不再需要手動管理 channel

  public:
    typedef enum { STOP, CW, CCW } Motor_Action;

    Motor(uint8_t pin_a, uint8_t pin_b); // 移除 channel 參數
    void act(Motor_Action dir, uint16_t speed); // 改用 Enum 強型別
    void stop();
};

/*****------- GoSUMO -------*****/
class GoSUMO {
  private:
    // 宣告四個馬達的指標，用來接收外部傳入的實體馬達
    Motor* _m1;
    Motor* _m2;
    Motor* _m3;
    Motor* _m4;

  public:
    typedef enum {
      FORWARD, BACKWARD, GO_LEFT, GO_RIGHT,
      R_TVL, L_TVL, RF_TVL, RB_TVL, LF_TVL, LB_TVL
    } Motion;

    // 建構子：要求傳入四顆馬達的記憶體位址
    GoSUMO(Motor* m1, Motor* m2, Motor* m3, Motor* m4);
    
    void act(Motion motion, uint16_t speed);
    void linetracking(Motion motion, uint16_t speed, uint16_t wheel_difference);
    void stop();   
};


/*--------------------------------------------------------*/
/*** SERVO MOTOR Implementation (High Precision) ***/
/*--------------------------------------------------------*/
class ServoMotor {
  private:
    uint8_t _pin;
    uint16_t _min_us; // 硬體脈衝校正值 (例如 500)
    uint16_t _max_us; // 硬體脈衝校正值 (例如 2500)
    
    // === 新增：軟體角度限制變數 ===
    int _limitMinAngle; // 機構允許的最小角度
    int _limitMaxAngle; // 機構允許的最大角度

    // ... (保留原本的變數：_freq, _resolution, _currentAngle 等) ...
    const uint32_t _freq = 50;      
    const uint8_t  _resolution = 16; 
    double _currentAngle;      
    double _targetAngle;       
    unsigned long _lastMoveTime; 
    int _stepDelay;    

  public:
    ServoMotor(uint8_t pin, uint16_t min_us = 500, uint16_t max_us = 2500);
    
	// 【關鍵修改】新增 defaultAngle 參數，預設為 -1 (代表未指定)
    // 參數順序：最小限制、最大限制、開機預設角度
    void begin(int minAngle = 0, int maxAngle = 180, int defaultAngle = -1);
    
    // ... (其他函式保持不變) ...
    void write(int angle);
    void writeMicroseconds(uint16_t us);
    void setTarget(int angle, int speed = 0);
    bool update();
    bool isMoving();
    int read();
};

#endif
