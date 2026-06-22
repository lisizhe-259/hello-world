// 定义LED引脚
const int ledPin = 2;  
const int freq = 5000;
const int resolution = 8;

// 闪烁控制变量
unsigned long previousMillis = 0;
const long interval = 500;  // 1Hz = 1秒周期，亮500ms，灭500ms
bool ledState = false;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
}

void loop() {
  unsigned long currentMillis = millis();

  // 检查是否到达切换时间点
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // 保存当前时间

    ledState = !ledState;  // 翻转状态

    if (ledState) {
      ledcWrite(ledPin, 255);  // 亮（最大亮度）
    } else {
      ledcWrite(ledPin, 0);    // 灭
    }
  }


}