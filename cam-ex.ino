// ESP32-CAM Web Server + PROGMEM HTML (STABLE)
// Gyro hız orantılı + Mod seçici + Durum göstergeleri + Pin modal (DÜZELTİLMİŞ)

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

WebServer server(80);

// ================= MOTOR PINLER =================
int IN1 = 12;
int IN2 = 13;
int IN3 = 14;
int IN4 = 15;
int ENA = 2;
int ENB = 4;

int speedVal = 150;

// ================= HTML =================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32-CAM Robot</title>
<style>
:root{--bg:#0f172a;--card:#1e293b;--btn:#334155;--accent:#38bdf8;--text:#f8fafc;--glow:#7dd3fc}
body{margin:0;font-family:Arial;background:var(--bg);color:var(--text);text-align:center}
.top{padding:10px;font-weight:bold}
.container{max-width:420px;margin:auto;padding:10px}
.card{background:var(--card);padding:12px;border-radius:14px;margin-bottom:10px}
.btn{width:70px;height:70px;font-size:26px;margin:6px;border:none;border-radius:18px;background:var(--btn);color:white}
.btn:active{box-shadow:0 0 20px var(--glow);background:var(--accent)}
.stop{background:#c00}
.mode button{padding:8px 12px;margin:5px;border-radius:10px;border:none}
.active{background:var(--accent)!important;color:#000}
.joybase{width:150px;height:150px;background:#0003;border-radius:50%;margin:15px auto;position:relative;touch-action:none}
.joy{width:60px;height:60px;background:var(--accent);border-radius:50%;position:absolute;top:45px;left:45px}
.bar{height:10px;background:#0005;border-radius:10px;overflow:hidden;margin-top:5px}
.fill{height:100%;width:0;background:var(--accent)}
#pinModal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:#0008}
#pinModal .card{margin:60px auto;max-width:320px}
input{width:40px}
</style>
</head>
<body>
<div class="top">ESP32-CAM ROBOT</div>
<div class="container">
<img id="cam" style="width:100%;border-radius:14px">

<div class="card">
<b>Kontrol Modu</b><br>
<div class="mode">
<button id="m_button" class="active" onclick="setMode('button')">Buton</button>
<button id="m_joy" onclick="setMode('joy')">Joystick</button>
<button id="m_gyro" onclick="setMode('gyro')">Gyro</button>
</div></div>

<div class="card">
<b>Hız:</b> <span id="sv">150</span>
<div class="bar"><div class="fill" id="speedBar"></div></div>
<input type="range" min="0" max="255" value="150" oninput="setSpeed(this.value)">
</div>

<div class="card" id="joyCard" style="display:none">
<div class="joybase" id="jb"><div class="joy" id="j"></div></div>
</div>

<div class="card" id="btnCard">
<button class="btn" onmousedown="send('forward')" ontouchstart="send('forward')" onmouseup="send('stop')" ontouchend="send('stop')">⬆</button><br>
<button class="btn" onmousedown="send('left')" ontouchstart="send('left')" onmouseup="send('stop')" ontouchend="send('stop')">⬅</button>
<button class="btn stop" onclick="send('stop')">⏹</button>
<button class="btn" onmousedown="send('right')" ontouchstart="send('right')" onmouseup="send('stop')" ontouchend="send('stop')">➡</button><br>
<button class="btn" onmousedown="send('backward')" ontouchstart="send('backward')" onmouseup="send('stop')" ontouchend="send('stop')">⬇</button>
</div>

<div class="card">
<b>Durum:</b> <span id="state">STOP</span>
</div>

<div class="card">
<button onclick="openPin()">⚙️ Pin Belirle</button>
</div>
</div>

<div id="pinModal">
 <div class="card">
  <b>Motor Pinleri</b><br><br>
  IN1 <input id="p1"> IN2 <input id="p2"><br><br>
  IN3 <input id="p3"> IN4 <input id="p4"><br><br>
  ENA <input id="pe"> ENB <input id="pf"><br><br>
  <button onclick="applyPins()">Uygula</button>
  <button onclick="closePin()">Kapat</button>
 </div>
</div>

<script>
const base = location.origin;
const cam = document.getElementById('cam');
cam.src = base + ':81/stream';

let mode = 'button';
function setMode(m){
 mode = m;
 ['button','joy','gyro'].forEach(x=>document.getElementById('m_'+x).classList.remove('active'));
 document.getElementById('m_'+m).classList.add('active');
 btnCard.style.display = m=='button'?'block':'none';
 joyCard.style.display = m=='joy'?'block':'none';
}

function send(c){
 fetch(base+'/action?go='+c);
 document.getElementById('state').innerText = c.toUpperCase();
}

function setSpeed(v){
 sv.innerText = v;
 speedBar.style.width = (v/255*100)+'%';
 fetch(base+'/speed?val='+v);
}

// ===== Gyro =====
window.addEventListener('deviceorientation', e=>{
 if(mode!='gyro') return;
 let sp = Math.min(255, Math.abs(e.beta||0)*4);
 setSpeed(sp);
 if(e.beta < -12) send('forward');
 else if(e.beta > 12) send('backward');
 else if(e.gamma > 12) send('right');
 else if(e.gamma < -12) send('left');
 else send('stop');
});

// ===== Joystick =====
const j = document.getElementById('j');
const jb = document.getElementById('jb');
let active = false;
jb.addEventListener('touchstart', ()=>{ if(mode=='joy') active=true; });
document.addEventListener('touchend', ()=>{
 active=false;
 j.style.top='45px'; j.style.left='45px';
 send('stop');
});
document.addEventListener('touchmove', e=>{
 if(!active || mode!='joy') return;
 let r = jb.getBoundingClientRect();
 let x = e.touches[0].clientX - r.left - 30;
 let y = e.touches[0].clientY - r.top - 30;
 x = Math.max(0, Math.min(90, x));
 y = Math.max(0, Math.min(90, y));
 j.style.left = x+'px';
 j.style.top = y+'px';
 let sp = Math.min(255, Math.hypot(x-45, y-45)*5);
 setSpeed(sp);
 if(y<25) send('forward');
 else if(y>65) send('backward');
 else if(x<25) send('left');
 else if(x>65) send('right');
});

// ===== Pin Modal =====
function openPin(){ document.getElementById('pinModal').style.display='block'; }
function closePin(){ document.getElementById('pinModal').style.display='none'; }
function applyPins(){
 fetch(base+`/pins?i1=${p1.value}&i2=${p2.value}&i3=${p3.value}&i4=${p4.value}&ea=${pe.value}&eb=${pf.value}`);
 closePin();
}
</script>
</body>
</html>
)rawliteral";

// ================= MOTOR =================
void applyPinMode(){pinMode(IN1,OUTPUT);pinMode(IN2,OUTPUT);pinMode(IN3,OUTPUT);pinMode(IN4,OUTPUT);} 
void stopMotor(){digitalWrite(IN1,0);digitalWrite(IN2,0);digitalWrite(IN3,0);digitalWrite(IN4,0);} 
void forward(){digitalWrite(IN1,1);digitalWrite(IN2,0);digitalWrite(IN3,1);digitalWrite(IN4,0);} 
void backward(){digitalWrite(IN1,0);digitalWrite(IN2,1);digitalWrite(IN3,0);digitalWrite(IN4,1);} 
void left(){digitalWrite(IN1,0);digitalWrite(IN2,1);digitalWrite(IN3,1);digitalWrite(IN4,0);} 
void right(){digitalWrite(IN1,1);digitalWrite(IN2,0);digitalWrite(IN3,0);digitalWrite(IN4,1);} 

void handleRoot(){ server.send_P(200, "text/html", index_html); }
void handleAction(){ String c=server.arg("go"); if(c=="forward")forward(); else if(c=="backward")backward(); else if(c=="left")left(); else if(c=="right")right(); else stopMotor(); server.send(200,"text/plain","OK"); }
void handleSpeed(){ speedVal=server.arg("val").toInt(); ledcWrite(0,speedVal); ledcWrite(1,speedVal); server.send(200,"text/plain","OK"); }
void handlePins(){ IN1=server.arg("i1").toInt(); IN2=server.arg("i2").toInt(); IN3=server.arg("i3").toInt(); IN4=server.arg("i4").toInt(); ENA=server.arg("ea").toInt(); ENB=server.arg("eb").toInt(); applyPinMode(); server.send(200,"text/plain","OK"); }

void setup(){
 Serial.begin(115200);
 applyPinMode();
 ledcSetup(0,1000,8); ledcSetup(1,1000,8);
 ledcAttachPin(ENA,0); ledcAttachPin(ENB,1);
 ledcWrite(0,speedVal); ledcWrite(1,speedVal);
 WiFi.begin(ssid,password);
 while(WiFi.status()!=WL_CONNECTED) delay(500);
 server.on("/",handleRoot);
 server.on("/action",handleAction);
 server.on("/speed",handleSpeed);
 server.on("/pins",handlePins);
 server.begin();
}
void loop(){ server.handleClient(); }
