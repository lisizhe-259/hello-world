// 定义LED引脚
const int ledPin = 2;
// 定义触摸引脚（使用支持触摸功能的GPIO）
const int touchPin = 4;  // 例如 GPIO4 (T0)

// 设置PWM属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 速度档位变量
int speedLevel = 1;            // 当前档位：1, 2, 3
const int maxSpeedLevel = 3;   // 最大档位

// 速度参数配置
struct SpeedConfig {
  int step;           // 每次步进值
  int delayMs;        // 延时时间(ms)
};

// 三个档位的配置：档位1最慢，档位3最快
const SpeedConfig speedConfigs[3] = {
  {1, 15},    // 档位1: 慢速呼吸
  {2, 10},    // 档位2: 中速呼吸
  {4, 5}      // 档位3: 快速呼吸
};

// 触摸检测变量
int lastTouchState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;  // 防抖延时(ms)

void setup() {
  Serial.begin(115200);
  
  // 配置触摸引脚为输入（内部上拉）
  pinMode(touchPin, INPUT_PULLUP);
  
  // 初始化LED PWM
  ledcAttach(ledPin, freq, resolution);
  
  // 启动时显示当前档位
  Serial.print("Starting with speed level: ");
  Serial.println(speedLevel);
}

void loop() {
  // 1. 检测触摸输入（检测下降沿）
  int currentTouchState = digitalRead(touchPin);
  
  if (lastTouchState == HIGH && currentTouchState == LOW) {
    // 防抖处理
    if (millis() - lastDebounceTime > debounceDelay) {
      // 切换档位：1->2->3->1...
      speedLevel++;
      if (speedLevel > maxSpeedLevel) {
        speedLevel = 1;
      }
      
      // 输出当前档位信息
      Serial.print("Speed level changed to: ");
      Serial.println(speedLevel);
      Serial.print("Step: ");
      Serial.print(speedConfigs[speedLevel-1].step);
      Serial.print(", Delay: ");
      Serial.println(speedConfigs[speedLevel-1].delayMs);
      
      lastDebounceTime = millis();
    }
  }
  lastTouchState = currentTouchState;
  
  // 2. 呼吸灯PWM控制
  // 获取当前档位的配置
  int currentStep = speedConfigs[speedLevel-1].step;
  int currentDelay = speedConfigs[speedLevel-1].delayMs;
  
  // 逐渐变亮
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle += currentStep) {
    ledcWrite(ledPin, dutyCycle);
    delay(currentDelay);
  }
  
  // 逐渐变暗
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle -= currentStep) {
    ledcWrite(ledPin, dutyCycle);
    delay(currentDelay);
  }
  
  // 串口调试（避免刷屏，每完成一个呼吸周期输出一次）
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 2000) {
    Serial.println("Breathing cycle completed");
    lastPrintTime = millis();
  }
}