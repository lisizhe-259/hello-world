/*
 * ESP32 触摸控制呼吸灯 - 5档速度可调
 * 硬件连接：
 * - LED: GPIO2 (或你使用的引脚)
 * - 触摸: GPIO4 (直接接导线作为触摸感应)
 * 
 * 使用方法：
 * - 触摸GPIO4的导线切换速度档位
 * - 共5档：极慢→慢速→中速→快速→极快
 */

// ==================== 引脚定义 ====================
const int ledPin = 2;        // LED控制引脚
const int touchPin = T0;     // 触摸引脚 GPIO4 (T0)

// ==================== PWM设置 ====================
const int freq = 5000;          // PWM频率 5000Hz
const int resolution = 8;       // PWM分辨率 8位 (0-255)

// ==================== 速度档位配置 ====================
int speedLevel = 0;            // 当前档位 0-4
const int maxSpeedLevel = 4;   // 最大档位

struct SpeedConfig {
  int step;           // 亮度步进值
  int delayMs;        // 每步延时(ms)
  const char* name;   // 档位名称
};

// 5个档位：从极慢到极快
const SpeedConfig speedConfigs[5] = {
  {1, 30, "极慢"},    // 档位1
  {1, 15, "慢速"},    // 档位2
  {2, 10, "中速"},    // 档位3
  {3, 5,  "快速"},    // 档位4
  {5, 2,  "极快"}     // 档位5
};

// ==================== 触摸检测 ====================
int touchThreshold = 100;       // 触摸阈值（数值越小越灵敏）
unsigned long lastTouchTime = 0;
const unsigned long debounceDelay = 300;  // 防抖延时(ms)

// ==================== 呼吸灯变量 ====================
int brightness = 0;
int direction = 1;  // 1: 变亮, -1: 变暗

// ==================== 初始化 ====================
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("=================================");
  Serial.println("  触摸呼吸灯 - 5档速度控制");
  Serial.println("=================================");
  Serial.println("触摸GPIO4切换速度档位");
  Serial.println("");
  
  // 初始化PWM
  ledcAttach(ledPin, freq, resolution);
  
  // 显示当前档位
  printSpeedInfo();
}

// ==================== 主循环 ====================
void loop() {
  // 1. 检测触摸
  checkTouch();
  
  // 2. 呼吸灯控制
  controlBreathing();
}

// ==================== 触摸检测函数 ====================
void checkTouch() {
  // 读取触摸值（数值越小表示触摸越强）
  int touchValue = touchRead(touchPin);
  
  // 检测触摸
  if (touchValue < touchThreshold) {
    // 防抖处理
    if (millis() - lastTouchTime > debounceDelay) {
      // 切换档位
      speedLevel++;
      if (speedLevel > maxSpeedLevel) {
        speedLevel = 0;
      }
      
      // 输出信息
      Serial.println(">>> 触摸检测! <<<");
      Serial.print("触摸值: ");
      Serial.println(touchValue);
      printSpeedInfo();
      
      lastTouchTime = millis();
    }
  }
}

// ==================== 呼吸灯控制函数 ====================
void controlBreathing() {
  // 获取当前档位配置
  int currentStep = speedConfigs[speedLevel].step;
  int currentDelay = speedConfigs[speedLevel].delayMs;
  
  // 更新亮度
  brightness += direction * currentStep;
  
  // 边界反转
  if (brightness >= 255) {
    brightness = 255;
    direction = -1;
  } else if (brightness <= 0) {
    brightness = 0;
    direction = 1;
  }
  
  // 输出PWM
  ledcWrite(ledPin, brightness);
  
  // 延时
  delay(currentDelay);
}

// ==================== 打印档位信息 ====================
void printSpeedInfo() {
  Serial.println("-------------------------");
  Serial.print("当前档位: ");
  Serial.print(speedLevel + 1);
  Serial.print("/5 (");
  Serial.print(speedConfigs[speedLevel].name);
  Serial.println(")");
  Serial.print("步进值: ");
  Serial.print(speedConfigs[speedLevel].step);
  Serial.print("  |  延时: ");
  Serial.print(speedConfigs[speedLevel].delayMs);
  Serial.println("ms");
  
  // 计算周期时间
  int cycleTime = (255 / speedConfigs[speedLevel].step) * speedConfigs[speedLevel].delayMs * 2;
  Serial.print("完整周期: 约 ");
  if (cycleTime >= 1000) {
    Serial.print(cycleTime / 1000.0, 1);
    Serial.println(" 秒");
  } else {
    Serial.print(cycleTime);
    Serial.println(" ms");
  }
  Serial.println("-------------------------");
}