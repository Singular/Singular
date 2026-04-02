importScripts("https://cdn.jsdelivr.net/npm/xterm-pty@0.9.4/workerTools.js");

onmessage = (msg) => {
  self.Module = self.Module || {};
  self.Module.preRun = self.Module.preRun || [];
  importScripts("Singular.js");

  emscriptenHack(new TtyClient(msg.data));
};