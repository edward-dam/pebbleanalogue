//Author: Ed Dam

var Clay       = require('./clay');
var clayConfig = require('./config');
var clay       = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }
  var settings     = clay.getSettings(e.response, false);
  var settingsFlat = {};
  Object.keys(settings).forEach(function(key) {
    if (typeof settings[key] === 'object' && settings[key]) {
      settingsFlat[key] = settings[key].value;
    } else {
      settingsFlat[key] = settings[key];
    }
  });
  Pebble.postMessage(settingsFlat);
});

function restoreSettings() {
  var settings = JSON.parse(localStorage.getItem('clay-settings'));
  if (settings) {
    Pebble.postMessage(settings);
  }
}

Pebble.on('message', function(event) {
  if (event.data.command === 'settings') {
    restoreSettings();
  }
});
