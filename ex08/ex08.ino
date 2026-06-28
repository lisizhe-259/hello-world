#include <WiFi.h>
#include <WebServer.h>

// WiFi配置
const char* ssid = "iPhone";
const char* password = "12345678";

// 全局硬件常量（解决变量未声明报错）
const int TOUCH_PIN = T0;
const int ALARM_LED = 2;
const int LED_FREQ = 5000;
const int LED_RES = 8;

// 全局安防状态
bool isArm = false;    // 布防标记
bool isAlarm = false;  // 报警锁定标记

WebServer server(80);

// 网页生成函数（修复字符串拼接报错）
String makeHtmlPage() {
  String armState = isArm ? "已布防" : "已撤防";
  String alarmState = isAlarm ? "⚠️ 报警锁定中" : "正常待机";

  String html = String(R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32安防主机</title>
  <style>
    body {font-family:Arial; text-align:center; margin-top:60px; background:#f5f5f5;}
    .box {max-width:500px; margin:0 auto; padding:30px; background:#fff; border-radius:12px; box-shadow:0 0 10px #ccc;}
    .state {font-size:22px; margin:20px 0;}
    .arm {color:green; font-weight:bold;}
    .disarm {color:#666; font-weight:bold;}
    .alarm {color:red; font-weight:bold;}
    button {padding:14px 30px; font-size:18px; margin:10px; border:none; border-radius:8px; cursor:pointer;}
    #btnArm {background:#28a745; color:white;}
    #btnDisarm {background:#dc3545; color:white;}
  </style>
</head>
<body>
  <div class="box">
    <h1>安防控制中心</h1>
    <div class="state">系统状态：<span class=")rawliteral")
    + (isArm ? "arm" : "disarm")
    + String(R"rawliteral(">)rawliteral")
    + armState
    + String(R"rawliteral(</span></div>
    <div class="state">报警状态：<span class=")rawliteral")
    + (isAlarm ? "alarm" : "")
    + String(R"rawliteral(">)rawliteral")
    + alarmState
    + String(R"rawliteral(</span></div>
    <hr>
    <button id="btnArm">布防 Arm</button>
    <button id="btnDisarm">撤防 Disarm</button>
  </div>

<script>
  const baseUrl = window.location.origin;
  document.getElementById("btnArm").addEventListener("click", ()=>{
    fetch(`${baseUrl}/arm`).then(()=>location.reload());
  });
  document.getElementById("btnDisarm").addEventListener("click", ()=>{
    fetch(`${baseUrl}/disarm`).then(()=>location.reload());
  });
</script>
</body>
</html>
)rawliteral");
  return html;
}

// 主页路由
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makeHtmlPage());
}

// 布防接口
void handleArm() {
  isArm = true;
  Serial.println("系统已布防，进入警戒模式");
  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防接口：完整重置所有状态
void handleDisarm() {
  // 全部状态归零重置
  isArm = false;
  isAlarm = false;
  ledcWrite(ALARM_LED, 0); // 关闭报警LED
  Serial.println("系统撤防，全部状态已重置，报警解除");
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  // PWM初始化LED
  ledcAttach(ALARM_LED, LED_FREQ, LED_RES);
  ledcWrite(ALARM_LED, 0);

  // WiFi连接
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页地址：http://");
  Serial.println(WiFi.localIP());

  // 绑定路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();

  // 仅布防且无报警时，检测触摸触发
  if (isArm && !isAlarm) {
    uint16_t touchVal = touchRead(TOUCH_PIN);
    if (touchVal < 30) {
      isAlarm = true;
      Serial.println("触摸触发，报警锁定，仅撤防可解除");
    }
  }

  // 报警锁定：持续高频闪烁
  if (isAlarm) {
    ledcWrite(ALARM_LED, 255);
    delay(80);
    ledcWrite(ALARM_LED, 0);
    delay(80);
  }
}