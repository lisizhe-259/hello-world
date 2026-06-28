// 定义LED引脚
const int ledPin = 2;
// 定义触摸引脚（使用支持触摸功能的GPIO）
const int touchPin = 4;  // 例如 GPIO4 (T0)

// 设置PWM属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 速度档位变量
int speedLevel = 0;            // 当前档位索引：0, 1, 2, 3, 4
const int maxSpeedLevel = 4;   // 最大档位索引（共5个档位）

// 速度参数配置 - 档位间差异更明显
struct SpeedConfig {
  int step;           // 每次步进值（值越大变化越快）
  int delayMs;        // 延时时间(ms)（值越小变化越快）
  const char* name;   // 档位名称
};

// 五个档位的配置：从极慢到极快
const SpeedConfig speedConfigs[5] = {
  {1, 30, "极慢"},    // 档位0: 非常慢的呼吸
  {1, 15, "慢速"},    // 档位1: 慢速呼吸
  {2, 10, "中速"},    // 档位2: 中速呼吸
  {3, 5,  "快速"},    // 档位3: 快速呼吸
  {5, 2,  "极快"}     // 档位4: 非常快的呼吸（几乎像闪烁）
};

// 触摸检测变量
int lastTouchState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;  // 防抖延时(ms)

// 用于呼吸效果的变量
int brightness = 0;
int direction = 1;  // 1: 增加亮度, -1: 减少亮度

void setup() {
  Serial.begin(115200);
  
  // 配置触摸引脚为输入（内部上拉）
  pinMode(touchPin, INPUT_PULLUP);
  
  // 初始化LED PWM
  ledcAttach(ledPin, freq, resolution);
  
  // 启动时显示当前档位
  printSpeedInfo();
}

void loop() {
  // 1. 检测触摸输入（检测下降沿）
  int currentTouchState = digitalRead(touchPin);
  
  if (lastTouchState == HIGH && currentTouchState == LOW) {
    // 防抖处理
    if (millis() - lastDebounceTime > debounceDelay) {
      // 切换档位：0->1->2->3->4->0...
      speedLevel++;
      if (speedLevel > maxSpeedLevel) {
        speedLevel = 0;
      }
      
      // 输出当前档位信息
      printSpeedInfo();
      
      lastDebounceTime = millis();
    }
  }
  lastTouchState = currentTouchState;
  
  // 2. 呼吸灯PWM控制 - 使用连续变化方式（更平滑）
  // 获取当前档位的配置
  int currentStep = speedConfigs[speedLevel].step;
  int currentDelay = speedConfigs[speedLevel].delayMs;
  
  // 更新亮度值
  brightness += direction * currentStep;
  
  // 检查是否到达边界，反转方向
  if (brightness >= 255) {
    brightness = 255;
    direction = -1;
  } else if (brightness <= 0) {
    brightness = 0;
    direction = 1;
  }
  
  // 输出PWM值
  ledcWrite(ledPin, brightness);
  
  // 延时
  delay(currentDelay);
}

// 打印档位信息
void printSpeedInfo() {
  Serial.println("=========================");
  Serial.print("当前档位: ");
  Serial.print(speedLevel + 1);
  Serial.print(" (");
  Serial.print(speedConfigs[speedLevel].name);
  Serial.println(")");
  Serial.print("步进值: ");
  Serial.println(speedConfigs[speedLevel].step);
  Serial.print("延时: ");
  Serial.println(speedConfigs[speedLevel].delayMs);
  
  // 计算并显示完整呼吸周期的大致时间
  int cycleTime = (255 / speedConfigs[speedLevel].step) * speedConfigs[speedLevel].delayMs * 2;
  Serial.print("完整周期约: ");
  Serial.print(cycleTime);
  Serial.println(" ms");
  Serial.println("=========================");
}