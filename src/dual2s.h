#ifndef DUAL2S_H
  #define DUAL2S_H
  #include <Arduino.h>
  
  //Buzzer Pitch define
  struct Pitch{
    uint16_t Do_3=131, Re_3=147, Mi_3=165, Fa_3=175, Sol_3=196, La_3=220, Si_3=247;
    uint16_t Do_4=262, Re_4=294, Mi_4=330, Fa_4=349, Sol_4=392, La_4=440, Si_4=494;
    uint16_t Do_5=523, Re_5=587, Mi_5=659, Fa_5=698, Sol_5=784, La_5=880, Si_5=988;
  };
  
/*** MOTOR ***/
  class Motor{
    private:
      uint16_t _m_frequency = 1000;
      uint8_t _m_resolution = 10;
      uint8_t _pin_a;
      uint8_t _pin_b;
      uint8_t _channel_a;
      uint8_t _channel_b;

    public:
      typedef enum{
        STOP,
        CW,
        CCW
      } Motor_Action;

      Motor(uint8_t pin_a, uint8_t pin_b, uint8_t channel_a, uint8_t channel_b);
      void act(uint8_t dir, uint16_t speed);
      void stop();
  };

/*** BUZZER ***/
  class Buzzer{
    private:
      byte _pin;
      byte _channel;  //esp32 pwm channel 0 - 15

    public:
      Buzzer(byte pin, byte channel);
      void noTone();
      void tone(uint16_t frequency, uint16_t duration, uint8_t volume);
      void alarm();
  };

/*** UltraSonic ***/
  class HCSR04{
    private:
      uint8_t _pin_echo;
      uint8_t _pin_trig;

    public:
      typedef enum{
        USC_FRONT,
        USC_LEFT,
        USC_RIGHT
      } USC_DIR;

      HCSR04(uint8_t pin_echo, uint8_t pin_trig);
      bool ObjSeeking(uint8_t thresh);
      float ObjDistance();
      unsigned long probing();
  };  

/*** IR3CH ***/
  class IR3CH{
    private:
      uint8_t _pin_L;
      uint8_t _pin_M;
      uint8_t _pin_R;

    public:
      IR3CH(uint8_t pin_L, uint8_t pin_M, uint8_t pin_R);
      byte ReadTCRT(uint8_t TH, bool DEBUG);

  }; 

/*** POWER ***/
  class Power{
    private:
      uint8_t _pin;
    public:
      Power(uint8_t pin);
      float voltage();
  };   

/*** GoSUMO ***/
  class GoSUMO{
    private:
      uint16_t _m_frequency = 1000;
      uint8_t _m_resolution = 10;
      uint8_t _pin_m1A = 14;  uint8_t _pin_m1B = 32;
      uint8_t _pin_m2A = 12;  uint8_t _pin_m2B = 33;
      uint8_t _pin_m3A = 25;  uint8_t _pin_m3B = 26;
      uint8_t _pin_m4A = 23;  uint8_t _pin_m4B = 22;

      uint8_t _channel_1a = 8; uint8_t _channel_1b = 9; 
      uint8_t _channel_2a = 10; uint8_t _channel_2b = 11;
      uint8_t _channel_3a = 12; uint8_t _channel_3b = 13;
      uint8_t _channel_4a = 14; uint8_t _channel_4b = 15;

    public:
      typedef enum{
        FORWARD,
        BACKWARD,
        GO_LEFT,
        GO_RIGHT
      } Motion;

      GoSUMO();
      void act(uint8_t motion, uint16_t speed);
      void stop();

  };


#endif
