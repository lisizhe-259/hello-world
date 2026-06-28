#include <WiFi.h>
#include <WebServer.h>

// WiFi信息
const char* ssid = "iPhone";
const char* password = "12345678";

const int TOUCH_PIN = T0; // GPIO4触摸引脚
WebServer server(80);

// 网页代码
String htmlPage = String(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width">
</head>
<body style="text-align:center;margin-top:50px;font-size:30px;">
<h2>触摸传感器数值</h2>
<div id="num" style="font-size:100px;color:green;">0</div>
<script>
// 定时AJAX拉取数据
function getData(){
  fetch("/touch")
  .then(res=>res.text())
  .then(v=>document.getElementById("num").innerText=v);
}
setInterval(getData,100);
</script>
</body>
</html>
)rawliteral");

// 下发网页
void indexPage(){
  server.send(200,"text/html;charset=utf-8",htmlPage);
}

// 上报触摸数据
void sendTouch(){
  server.send(200,"text/plain",String(touchRead(TOUCH_PIN)));
}

void setup(){
  Serial.begin(115200);
  // 连WiFi
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){delay(500);}
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
  // 绑定接口
  server.on("/",indexPage);
  server.on("/touch",sendTouch);
  server.begin();
}

void loop(){
  server.handleClient();
}
