<html>
  <head>
    <title>ESP32-CAM Robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  margin: 0;
  background: #333;
  color: white;
}

.controller {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-top: 20px;
}

.row {
  display: flex;
  justify-content: center;
  margin: 5px 0;
}

.btn {
  width: 70px;
  height: 70px;
  margin: 15px;
  font-size: 26px;
  font-weight: bold;
  border-radius: 15px;
  border: none;
  background: linear-gradient(145deg, #2f4468, #1c2b45);
  color: white;
  box-shadow: 0 4px 8px rgba(0,0,0,0.6);
  cursor: pointer;
}

.btn:active {
  transform: scale(0.95);
  background: #00aaff;
}

.btn.stop {
  background: #aa0000;
  font-weight: bold;
}

.btn.stop:active {
  background: #ff0000;
}

    </style>
  </head>
  <body>
    <h1>ESP32-CAM Robot</h1>
    <img src="" id="photo" >
<div class="controller">
  <div class="row">
    <button class="btn up"
      onmousedown="toggleCheckbox('forward');"
      ontouchstart="toggleCheckbox('forward');"
      onmouseup="toggleCheckbox('stop');"
      ontouchend="toggleCheckbox('stop');">
      ▲
    </button>
  </div>

  <div class="row">
    <button class="btn left"
      onmousedown="toggleCheckbox('left');"
      ontouchstart="toggleCheckbox('left');"
      onmouseup="toggleCheckbox('stop');"
      ontouchend="toggleCheckbox('stop');">
      ◀
    </button>

    <button class="btn stop"
      onclick="toggleCheckbox('stop');">
      ■
    </button>

    <button class="btn right"
      onmousedown="toggleCheckbox('right');"
      ontouchstart="toggleCheckbox('right');"
      onmouseup="toggleCheckbox('stop');"
      ontouchend="toggleCheckbox('stop');">
      ▶
    </button>
  </div>

  <div class="row">
    <button class="btn down"
      onmousedown="toggleCheckbox('backward');"
      ontouchstart="toggleCheckbox('backward');"
      onmouseup="toggleCheckbox('stop');"
      ontouchend="toggleCheckbox('stop');">
      ▼
    </button>
  </div>
</div>

   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
     xhr.send();
   }
   window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
