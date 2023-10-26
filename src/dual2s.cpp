#include <dual2s.h>


/*** GoSUMO ***/
GoSUMO::GoSUMO(){
  pinMode(_pin_m1A, OUTPUT); pinMode(_pin_m1B, OUTPUT);
  pinMode(_pin_m2A, OUTPUT); pinMode(_pin_m2B, OUTPUT);
  pinMode(_pin_m3A, OUTPUT); pinMode(_pin_m3B, OUTPUT);
  pinMode(_pin_m4A, OUTPUT); pinMode(_pin_m4B, OUTPUT);

  ledcSetup(_channel_1a, _m_frequency, _m_resolution);   ledcSetup(_channel_1b, _m_frequency, _m_resolution);
  ledcAttachPin(_pin_m1A, _channel_1a);  ledcAttachPin(_pin_m1B, _channel_1b);
  ledcSetup(_channel_2a, _m_frequency, _m_resolution);   ledcSetup(_channel_2b, _m_frequency, _m_resolution);
  ledcAttachPin(_pin_m2A, _channel_2a);  ledcAttachPin(_pin_m2B, _channel_2b);
  ledcSetup(_channel_3a, _m_frequency, _m_resolution);   ledcSetup(_channel_3b, _m_frequency, _m_resolution);
  ledcAttachPin(_pin_m3A, _channel_3a);  ledcAttachPin(_pin_m3B, _channel_3b);
  ledcSetup(_channel_4a, _m_frequency, _m_resolution);   ledcSetup(_channel_4b, _m_frequency, _m_resolution);
  ledcAttachPin(_pin_m4A, _channel_4a);  ledcAttachPin(_pin_m4B, _channel_4b);
}

void GoSUMO::act(uint8_t motion, uint16_t speed){
  if(motion == FORWARD){
    ledcWrite(_channel_1a, 0); ledcWrite(_channel_1b, speed); //Moto-1
    ledcWrite(_channel_3a, 0); ledcWrite(_channel_3b, speed); //Moto-3
    ledcWrite(_channel_2a, speed); ledcWrite(_channel_2b, 0); //Moto-2
    ledcWrite(_channel_4a, speed); ledcWrite(_channel_4b, 0); //Moto-4 
  }
  else if(motion == BACKWARD){
    ledcWrite(_channel_1a, speed); ledcWrite(_channel_1b, 0); //Moto-1
    ledcWrite(_channel_3a, speed); ledcWrite(_channel_3b, 0); //Moto-3
    ledcWrite(_channel_2a, 0); ledcWrite(_channel_2b, speed); //Moto-2
    ledcWrite(_channel_4a, 0); ledcWrite(_channel_4b, speed); //Moto-4 
  }
  else if(motion == GO_LEFT){
    ledcWrite(_channel_1a, speed); ledcWrite(_channel_1b, 0); //Moto-1
    ledcWrite(_channel_3a, speed); ledcWrite(_channel_3b, 0); //Moto-3
    ledcWrite(_channel_2a, speed); ledcWrite(_channel_2b, 0); //Moto-2
    ledcWrite(_channel_4a, speed); ledcWrite(_channel_4b, 0); //Moto-4  
  }
  else if(motion == GO_RIGHT){
    ledcWrite(_channel_1a, 0); ledcWrite(_channel_1b, speed); //Moto-1
    ledcWrite(_channel_3a, 0); ledcWrite(_channel_3b, speed); //Moto-3
    ledcWrite(_channel_2a, 0); ledcWrite(_channel_2b, speed); //Moto-2
    ledcWrite(_channel_4a, 0); ledcWrite(_channel_4b, speed); //Moto-4 
  }
}
void GoSUMO::stop(){
    ledcWrite(_channel_1a, 0); ledcWrite(_channel_1b, 0); //Moto-1
    ledcWrite(_channel_3a, 0); ledcWrite(_channel_3b, 0); //Moto-3
    ledcWrite(_channel_2a, 0); ledcWrite(_channel_2b, 0); //Moto-2
    ledcWrite(_channel_4a, 0); ledcWrite(_channel_4b, 0); //Moto-4  
}

/*** MOTOR ***/
Motor::Motor(uint8_t pin_a, uint8_t pin_b, uint8_t channel_a, uint8_t channel_b){
  _pin_a = pin_a; 
  _pin_b = pin_b;
  _channel_a = channel_a;
  _channel_b = channel_b;

  //The _channel was used.
  if (ledcRead(_channel_a)) { log_e("channel %d is already in use", ledcRead(_channel_a)); return; }
  if (ledcRead(_channel_b)) { log_e("channel %d is already in use", ledcRead(_channel_b)); return; }

  pinMode(_pin_a, OUTPUT);
  pinMode(_pin_b, OUTPUT);

  ledcSetup(_channel_a, _m_frequency, _m_resolution);
	ledcSetup(_channel_b, _m_frequency, _m_resolution);
  ledcAttachPin(_pin_a, _channel_a);    
  ledcAttachPin(_pin_b, _channel_b);
}

void Motor::act(uint8_t dir, uint16_t speed){
  if(dir == CW){  ledcWrite(_channel_a, speed); ledcWrite(_channel_b, 0);  }
  else if(dir == CCW){  ledcWrite(_channel_a, 0); ledcWrite(_channel_b, speed);  }
}

void Motor::stop(){  ledcWrite(_channel_a, 0); ledcWrite(_channel_b, 0);  }



/*** ULTRASONIC ***/
HCSR04::HCSR04(uint8_t pin_echo, uint8_t pin_trig){
  _pin_echo = pin_echo;
  _pin_trig = pin_trig;
  pinMode(_pin_trig, OUTPUT); 
  pinMode(_pin_echo, INPUT);   
}

unsigned long HCSR04::probing(){
  digitalWrite(_pin_trig, HIGH);
  delayMicroseconds(5);      //維持trig腳位在高電位5微秒
  digitalWrite(_pin_trig, LOW);
  return pulseIn(_pin_echo, HIGH);
}

float HCSR04::ObjDistance(){
  float d = 0.0;
  d = probing()/58.0; //計算物件的距離
  return d;
}

bool HCSR04::ObjSeeking(uint8_t thresh){
  //int fightThresh = thresh*58; //單位為公分
  //long distance = probing(); //讀取障礙物的距離
  if(probing() < (thresh*58)) { return true; } //物體進入範圍
  else { return false; }  //物體不在範圍之內
}

/*** IR3CH ***/
IR3CH::IR3CH(uint8_t pin_L, uint8_t pin_M, uint8_t pin_R){
  _pin_L = pin_L;
  _pin_M = pin_M;
  _pin_R = pin_R;

  pinMode(_pin_L, INPUT);
  pinMode(_pin_M, INPUT);
  pinMode(_pin_R, INPUT);
}

byte IR3CH::ReadTCRT(uint8_t TH, bool DEBUG){
  byte _bTrace = 0;
  
  if(analogRead(_pin_R) > TH) { _bTrace = _bTrace | B001; }
  else { _bTrace = _bTrace & B110; }
  
  if(analogRead(_pin_M) > TH) { _bTrace = _bTrace | B010; }
  else { _bTrace = _bTrace & B101; }
  
  if(analogRead(_pin_L) > TH) { _bTrace = _bTrace | B100; }
  else { _bTrace = _bTrace & B011; }
  
  /* 讀取類比數值，並將3CH結果輸出至串列視窗  */
  if (DEBUG){
  Serial.print(analogRead(_pin_L));
  Serial.print("(LEFT) / ");
  Serial.print(analogRead(_pin_M));
  Serial.print("(Middle) / ");
  Serial.print(analogRead(_pin_R));
  Serial.print("(RIGHT)");
  Serial.println(_bTrace);
  Serial.println("----");
  delay(1000);
  }
  /*-------------------------------------------------------------*/
  
  return _bTrace;
}

/*** BUZZER ***/
Buzzer::Buzzer(byte pin = 15, byte channel = 15){
  _pin = pin;
  _channel = channel;
  pinMode(_pin, OUTPUT);
}

void Buzzer::alarm(){
  tone(800, 200, 30); 
}

void Buzzer::noTone(){
   ledcDetachPin(_pin);
   ledcWrite(_channel, 0); 
}

void Buzzer::tone(uint16_t frequency, uint16_t duration, uint8_t volume){
  ledcSetup(_channel, frequency, 8);
  ledcAttachPin(_pin, _channel);
  ledcWrite(_channel, volume);
  if (duration) {
    delay(duration);
    noTone();
  } 
}

/*** POWER ***/
Power::Power(uint8_t pin){
  _pin = pin;
  pinMode(_pin, INPUT);
  analogSetAttenuation(ADC_11db); //衰減值3.6v：ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  analogSetWidth(12); //解析度10bits：9, 10, 11, 12
}

float Power::voltage(){
  //ADC解析度為12bits (4096階)
  //ADC衰減值為3.6V
  //分壓比為~0.25 (目標端電阻100 / 量測端電阻33)
  float _fVal = (analogRead(_pin)*0.0009)/0.25;
  return _fVal;
}
