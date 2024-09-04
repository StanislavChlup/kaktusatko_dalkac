let pressedKeys = {
  w: false,
  s: false,
  a: false,
  d: false,
};
let functions = {
  o: false,
  c: false,
  us: false,
  col: false,
};

window.addEventListener("keydown", (e) => {
  switch (e.code) {
    case "KeyW":
      if (pressedKeys.w == true) return;
      pressedKeys.w = true;
      break;
    case "KeyS":
      if (pressedKeys.s == true) return;
      pressedKeys.s = true;
      break;
    case "KeyA":
      if (pressedKeys.a == true) return;
      pressedKeys.a = true;
      break;
    case "KeyD":
      if (pressedKeys.d == true) return;
      pressedKeys.d = true;
      break;
    case "Numpad8":
      if (pressedKeys.w == true) return;
      pressedKeys.w = true;
      break;
    case "Numpad5":
      if (pressedKeys.s == true) return;
      pressedKeys.s = true;
      break;
    case "Numpad4":
      if (pressedKeys.a == true) return;
      pressedKeys.a = true;
      break;
    case "Numpad6":
      if (pressedKeys.d == true) return;
      pressedKeys.d = true;
      break;

    // special functions
    case "Numpad7":
      functions.o = true;
      break;
    case "Numpad9":
      functions.c = true;
      break;
    case "Numpad1":
      functions.us = true;
      break;
    case "Numpad2":
      functions.col = true;
      break;

    default:
      return;
  }
  udp.sendData(pressedKeys, functions);
  for(let fn in functions){
    functions[fn] = false;
  }
});

window.addEventListener("keyup", (e) => {
  if (pressedKeys[e.key] == false) return;
  switch (e.code) {
    case "KeyW":
      if (pressedKeys.w == false) return;
      pressedKeys.w = false;
      break;
    case "KeyS":
      if (pressedKeys.s == false) return;
      pressedKeys.s = false;
      break;
    case "KeyA":
      if (pressedKeys.a == false) return;
      pressedKeys.a = false;
      break;
    case "KeyD":
      if (pressedKeys.d == false) return;
      pressedKeys.d = false;
      break;
    case "Numpad8":
      if (pressedKeys.w == false) return;
      pressedKeys.w = false;
      break;
    case "Numpad5":
      if (pressedKeys.s == false) return;
      pressedKeys.s = false;
      break;
    case "Numpad4":
      if (pressedKeys.a == false) return;
      pressedKeys.a = false;
      break;
    case "Numpad6":
      if (pressedKeys.d == false) return;
      pressedKeys.d = false;
      break;
    default:
      return;
  }
  udp.sendData(pressedKeys, functions);
});


udp.receive('color', (color) => {
  document.querySelector("#rgb").style.background = `rgb(${100*color.r/255}%, ${100*color.g/255}%, ${100*color.b/255}%)`;
  document.querySelector("#color").innerHTML = `rgb(${color.r}, ${color.g}, ${color.b})<br>${color.name}`;
});

udp.receive('usdistance', (dis) => {
  document.querySelector("#distance").innerHTML = dis + "mm";
});


