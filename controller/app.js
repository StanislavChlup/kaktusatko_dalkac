const { app, BrowserWindow, ipcMain, contextBridge } = require("electron");
const path = require("path");
const dgram = require("dgram");



let mainWindow;
function createWindow() {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: false, // Disable nodeIntegration
      preload: path.join(__dirname, "preload.js"), // Preload script to expose functions to renderer process
    },
  });

  mainWindow.loadFile("index.html"); // Load your HTML file
}

app.whenReady().then(createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

app.on("activate", () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

const client = dgram.createSocket("udp4");

client.connect(80, "192.168.4.1", () => {
  console.log("connected");
});

function sendData(pk, functions) {
  let data = "u";
  for (let key in pk) {
    data += pk[key] ? "1" : "0";
  }
  for (let fn in functions) {
    data += functions[fn] ? "1" : "0";
  }
  client.send(data);
}

ipcMain.handle("sendData", (event, pk, fns) => {
  sendData(pk, fns);
});

function reload(){
  client.disconnect();
  client.connect(80, "192.168.4.1", () => {
    console.log("reconnected");
  });
}

ipcMain.handle("reload", (event) => {
  reload();
});


client.on("message", (msg, rinfo) => {
  msg = msg.toString();
  switch(msg[0]){
    case 'u':
      mainWindow.webContents.send('usdistance', msg.slice(1));
      break;
    case 'c':
      msg = msg.slice(1);
      let data = msg.split(",");
      let name =  + data[3] == 0 ? "red" :
                  + data[3] == 1 ? "green" : 
                  + data[3] == 2 ? "blue" : 
                  "none"
      mainWindow.webContents.send('color', {r: +data[0], g: +data[1], b: +data[2], name: name});
      break;
  }

})

