#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "iPhone";
const char* password = "12345678";
const int LED_PIN = 2;
const int freq = 5000;
const int resolution = 8;

WebServer server(80);

// ========== 网页生成函数（放在setup上方）==========
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>PWM滑动条调光</title>
  <style>
    body {font-family:Arial; text-align:center; margin-top:60px;}
    .slider-box {margin:30px auto; width:80%; max-width:500px;}
    input[type="range"] {width:100%; height:10px;}
    #valueText {font-size:24px; color:#222; margin:15px 0;}
  </style>
</head>
<body>
  <h1>LED PWM亮度调节</h1>
  <div class="slider-box">
    <p>亮度滑动条 (0~255)</p>
    <input type="range" id="brightSlider" min="0" max="255" value="0">
    <div id="valueText">当前亮度：0</div>
  </div>

<script>
  const slider = document.getElementById("brightSlider");
  const valueText = document.getElementById("valueText");
  const baseUrl = window.location.origin;

  slider.addEventListener("input", function(){
    let val = this.value;
    valueText.innerText = "当前亮度：" + val;
    fetch(`${baseUrl}/set?pwm=${val}`)
      .catch(err => console.log("请求失败：", err));
  });
</script>
</body>
</html>
)rawliteral";
  return html;
}

// ========== 路由处理函数（放在setup上方）==========
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSetPWM() {
  int pwmVal = 0;
  if(server.hasArg("pwm")){
    pwmVal = server.arg("pwm").toInt();
    pwmVal = constrain(pwmVal, 0, 255);
  }
  ledcWrite(LED_PIN, pwmVal);
  Serial.print("收到亮度值：");
  Serial.println(pwmVal);
  server.send(200, "text/plain", "OK");
}

// ========== 初始化函数（现在在所有函数下方）==========
void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, freq, resolution);
  ledcWrite(LED_PIN, 0);
  
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页地址：http://");
  Serial.println(WiFi.localIP());

  // 此时handleRoot已经提前定义，不会报未声明错误
  server.on("/", handleRoot);
  server.on("/set", handleSetPWM);
  server.begin();
}

void loop() {
  server.handleClient();
}