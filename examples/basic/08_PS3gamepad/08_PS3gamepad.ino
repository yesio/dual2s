/*=====================================================================================
yesio.net / 2026.03.12 / by nick
# Filename：08_PS3GamePad.ino
# Function：PS3搖桿使用範例 / Exampde code for PS3 GamePad.
# Toolchain & Libs：ESP32 Arduino Core v3.3.5 (ESP-IDF v5.1), dual2s
======================================================================================*/
#include <dual2s.h>

Buzzer bz(DUAL2S_HW::BUZZER);

void notify() {

    // 顯示按鍵狀態
    if (Ps3.data.button.cross)    Serial.println("按下：X 按鈕");
    if (Ps3.data.button.circle)   Serial.println("按下：O 按鈕");
    if (Ps3.data.button.triangle) Serial.println("按下：△ 按鈕");
    if (Ps3.data.button.square)   Serial.println("按下：□ 按鈕");

    if (Ps3.data.button.up)    Serial.println("按下：上鍵");
    if (Ps3.data.button.down)  Serial.println("按下：下鍵");
    if (Ps3.data.button.left)  Serial.println("按下：左鍵");
    if (Ps3.data.button.right) Serial.println("按下：右鍵");

    if (Ps3.data.button.l1) Serial.println("按下：L1 按鈕");
    if (Ps3.data.button.l2) Serial.println("按下：L2 按鈕");
    if (Ps3.data.button.r1) Serial.println("按下：R1 按鈕");
    if (Ps3.data.button.r2) Serial.println("按下：R2 按鈕");

    if (Ps3.data.button.select) Serial.println("按下：SELECT 按鈕");
    if (Ps3.data.button.start)  Serial.println("按下：START 按鈕");
    if (Ps3.data.button.ps)     Serial.println("按下：PS 按鈕");

    // 顯示搖桿數值
    Serial.printf("左搖桿 X: %d, Y: %d\n", Ps3.data.analog.stick.lx, Ps3.data.analog.stick.ly);
    Serial.printf("右搖桿 X: %d, Y: %d\n", Ps3.data.analog.stick.rx, Ps3.data.analog.stick.ry);

}

void onConnect() {
  //連線成功
  for(int i=0; i<3; i++){ bz.alarm(); delay(200);}

  Serial.println("PS3 控制器已連接！");
}

void setup() {
  Serial.begin(115200);
  bz.alarm();

  Ps3.attach(notify);              // 設定按鍵變更時的回呼函式
  Ps3.attachOnConnect(onConnect);  // 設定成功連線時的回呼函式
  Ps3.begin("20:00:00:00:00:00");  // 輸入你的搖桿位址
}

void loop() {
  //主程式區 ---------------------------------------
  //Develop your code here.
}
