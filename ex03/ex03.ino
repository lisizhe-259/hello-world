// 定义LED引脚
const int ledPin = 2;  
const int freq = 5000;
const int resolution = 8;

// 闪烁控制变量
unsigned long previousMillis = 0;
int step = 0;
int sequence[] = {200, 200, 200, 200, 200, 600,   // S: 点-灭-点-灭-点-长灭
                  600, 200, 600, 200, 600, 600,    // O: 划-灭-划-灭-划-长灭
                  200, 200, 200, 200, 200, 2000};  // S: 点-灭-点-灭-点-长停顿
bool ledState = false;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= sequence[step]) {
    previousMillis = currentMillis;

    ledState = !ledState;
    ledcWrite(ledPin, ledState ? 255 : 0);

    step++;
    if (step >= 18) {
      step = 0;
      Serial.println("SOS cycle completed");
    }
  }
}
