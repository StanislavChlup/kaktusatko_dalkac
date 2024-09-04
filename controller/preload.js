const { contextBridge, ipcRenderer } = require('electron');

// Expose sendData function to the renderer process
contextBridge.exposeInMainWorld('udp', {
  sendData: (pk, fns) => {
    ipcRenderer.invoke('sendData', pk, fns);
  },
  reload: () => {
    ipcRenderer.invoke('reload');
  },
  receive: (channel, func) => {
    ipcRenderer.on(channel, (event, ...args) => func(...args));
  }
});
