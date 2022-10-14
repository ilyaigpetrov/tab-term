import './node_modules/xterm/lib/xterm.js';

const term = new Terminal();
term.open(document.getElementById('terminal'));

const port = chrome.runtime.connectNative('hello_world_host');
console.log(chrome.runtime.lastError || 'OK 1. Connected to the native host.');
port.onMessage.addListener((response) => {
  console.log(chrome.runtime.lastError || 'OK 2.');
  console.log(`Received: ${response}`);
  term.write(response);
});
console.log(chrome.runtime.lastError || 'OK 3.');
term.onData((data) => {
  console.log('TERM ON DATA:', data);
  port.postMessage(data);
  console.log(chrome.runtime.lastError || 'POST 1.');
});
console.log(chrome.runtime.lastError || 'POST 2.');