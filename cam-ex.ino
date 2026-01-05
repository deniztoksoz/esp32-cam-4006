// ESP32-CAM Web Server + PROGMEM HTML + Gyro kontrol
// HTML gömülü, joystick + buton + hız + telefon eğme

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

WebServer server(80);

// ================= MOTOR PINLER =================
#define IN1 12
#define IN2 13
#define IN3 14
#define IN4 15
#define ENA 2
#define ENB 4

int speedVal = 150;

// ================= HTML (PROGMEM) =================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32-CAM Kontrol</title>
<style>
:root{--bg:#0f172a;--card:#1e293b;--btn:#334155;--accent:#38bdf8;--text:#f8fafc}
body{margin:0;font-family:Arial;background:var(--bg);color:var(--text);text-align:center}
.top{display:flex;justify-content:space-between;padding:10px}
button{border:none;border-radius:16px}
.container{max-width:420px;margin:auto}
.btn{width:70px;height:70px;font-size:26px;margin:6px;background:var(--btn);color:white}
.stop{background:#c00}
.speed{background:var(--card);padding:10px;border-radius:14px}
.joybase{width:150px;height:150px;background:var(--card);border-radius:50%;margin:20px auto;position:relative;touch-action:none}
.joy{width:60px;height:60px;background:var(--accent);border-radius:50%;position:absolute;top:45px;left:45px}
</style>
</head>
<body>
<div class="top"><b>ESP32-CAM</b><button onclick="gyroPerm()">📱 Gyro</button></div>
<div class="container">
<img src="" id="cam" style="width:100%;border-radius:14px">
<div class="speed">Hız: <span id="sv">150</span><br><input type="range" min="0" max="255" value="150" oninput="speed(this.value)"></div>
<div class="joybase" id="jb"><div class="joy" id="j"></div></div>
<button class="btn" onmousedown="cmd('forward')" onmouseup="cmd('stop')">⬆</button><br>
<button class="btn" onmousedown="cmd('left')" onmouseup="cmd('stop')">⬅</button>
<button class="btn stop" onclick="cmd('stop')">⏹</button>
<button class="btn" onmousedown="cmd('right')" onmouseup="cmd('stop')">➡</button><br>
<button class="btn" onmousedown="cmd('backward')" onmouseup="cmd('stop')">⬇</button>
</div>
<script>
const base=location.origin;document.getElementById('cam').src=base+':81/stream';
function cmd(c){fetch(base+'/action?go='+c)}
function speed(v){sv.innerText=v;fetch(base+'/speed?val='+v)}
function gyroPerm(){if(DeviceOrientationEvent.requestPermission){DeviceOrientationEvent.requestPermission().then(p=>{if(p=='granted')gyro()})}else gyro()}
function gyro(){window.addEventListener('deviceorientation',e=>{if(e.beta<-15)cmd('forward');else if(e.beta>15)cmd('backward');else if(e.gamma>15)cmd('right');else if(e.gamma<-15)cmd('left');else cmd('stop')})}
const j=document.getElementById('j'),jb=document.getElementById('jb');let a=false;jb.ontouchstart=()=>a=true;document.ontouchend=()=>{a=false;j.style.top='45px';j.style.left='45px';cmd('stop')};document.ontouchmove=e=>{if(!a)return;let r=jb.getBoundingClientRect();let x=e.touches[0].clientX-r.left-30;let y=e.touches[0].clientY-r.top-30;x=Math.max(0,Math.min(90,x));y=Math.max(0,Math.min(90,y));j.style.left=x+'px';j.style.top=y+'px';if(y<25)cmd('forward');else if(y>65)cmd('backward');else if(x<25)cmd('left');else if(x>65)cmd('right')}
</script>
</body>
</html>
)rawliteral";

// ================= MOTOR FONKS =================
void stopMotor(){digitalWrite(IN1,0);digitalWrite(IN2,0);digitalWrite(IN3,0);digitalWrite(IN4,0);} 
void forward(){digitalWrite(IN1,1);digitalWrite(IN2,0);digitalWrite(IN3,1);digitalWrite(IN4,0);} 
void backward(){digitalWrite(IN1,0);digitalWrite(IN2,1);digitalWrite(IN3,0);digitalWrite(IN4,1);} 
void left(){digitalWrite(IN1,0);digitalWrite(IN2,1);digitalWrite(IN3,1);digitalWrite(IN4,0);} 
void right(){digitalWrite(IN1,1);digitalWrite(IN2,0);digitalWrite(IN3,0);digitalWrite(IN4,1);} 

void handleRoot(){server.send_P(200,"text/html",index_html);} 
void handleAction(){
 String c=server.arg("go");
 if(c=="forward")forward();
 else if(c=="backward")backward();
 else if(c=="left")left();
 else if(c=="right")right();
 else stopMotor();
 server.send(200,"text/plain","OK");
}

void setup(){
 Serial.begin(115200);
 pinMode(IN1,OUTPUT);pinMode(IN2,OUTPUT);pinMode(IN3,OUTPUT);pinMode(IN4,OUTPUT);
 ledcSetup(0,1000,8);ledcSetup(1,1000,8);
 ledcAttachPin(ENA,0);ledcAttachPin(ENB,1);
 ledcWrite(0,speedVal);ledcWrite(1,speedVal);
 WiFi.begin(ssid,password);
 while(WiFi.status()!=WL_CONNECTED) delay(500);
 server.on("/",handleRoot);
 server.on("/action",handleAction);
 server.begin();
 Serial.println(WiFi.localIP());
}
void loop(){server.handleClient();}
