var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var CurrentTimeStr = "TBD";

// Listen for when the config is requested
function show_configuration_handler (e) {
    console.log('show configuration!');
    clay.config = clayConfig;
    Pebble.openURL(clay.generateUrl());
}
Pebble.addEventListener('showConfiguration', show_configuration_handler);

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  // Get the keys and values from each config item
  var dict = clay.getSettings(e.response);

  // Send settings values to watch side
  Pebble.sendAppMessage(dict, function(e) {
    console.log('Sent config data to Pebble');
  }, function(e) {
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));
  });
});

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
function app_message_handler(e) {
    console.log('AppMessage received!');
    console.log(JSON.stringify(e));
}                     
Pebble.addEventListener('appmessage', app_message_handler);

