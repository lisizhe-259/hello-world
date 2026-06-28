// 定义两路LED引脚（可自行修改为你面包板实际接线引脚）
const int ledAPin = 2;
const int ledBPin = 4;

// PWM统一参数
const int freq = 5000;
const int resolution = 8;  // 取值范围 0~255

void setup() {
  Serial.begin(115200);
  
  // 分别绑定两路独立PWM通道
  ledcAttach(ledAPin, freq, resolution);
  ledcAttach(ledBPin, freq, resolution);
}

void loop() {
  // 正向循环：A渐亮，B渐暗
  for(int val = 0; val <= 255; val++){
    ledcWrite(ledAPin, val);        // A占空比递增
    ledcWrite(ledBPin, 255 - val);  // B占空比反向递减
    delay(10);
  }

  // 反向循环：A渐暗，B渐亮
  for(int val = 255; val >= 0; val--){
    ledcWrite(ledAPin, val);
    ledcWrite(ledBPin, 255 - val);
    delay(10);
  }

  Serial.println("一次反相呼吸周期完成");
}