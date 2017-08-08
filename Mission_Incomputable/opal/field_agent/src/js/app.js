/* -------------------------------------------------------------------------- *
 *  Globals
 * -------------------------------------------------------------------------- */

// Load the app configuration data.
var appinfo = require('./appinfo.js');

// A unique ID for this pebble/app.
var PEBBLE_DEVICE_ID;

// The URI endpoint for the WebSocket connection.
var wsUri;

var websocket;

/* -------------------------------------------------------------------------- *
 *  Utility functions.
 * -------------------------------------------------------------------------- */

// INFO, DEBUG, ERROR, WEBSOCKET(-RX,-TX,-ERROR), BT(-RX,-TX,-ERROR)
function log(prefix, message) {
   if (appinfo.config['appjsDebug'] === 'true' || prefix !== 'DEBUG') {
    console.log(prefix + ': ' + message);
   }
}

// Convert an array buffer to a string (e.g., messages from the Game Server)
function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

/* -------------------------------------------------------------------------- *
 *  Location handlers.
 * -------------------------------------------------------------------------- */

/*
 * Handle a successful retrieval of location information.
 */
function locationSuccess(pos) {
  // Send location data.
  log('DEBUG', 'Success requesting location!' + ' (' + pos.coords.latitude + ',' + pos.coords.longitude + ')');

  // On success, send back to pebble (not directly to server....).
  var formattedLocationMsg = pos.coords.latitude + '|' + pos.coords.longitude; 
  sendStringToPebble('AppKeyLocation', formattedLocationMsg);
  // doWsSend(formattedLocationMsg); // ***KEEP FOR DEBUG*** On success, send directly to proxy/target server.
}

/*
 * Handle an unsuccessful retrieval of location information.
 */
function locationError(err) {
  if(err.code == err.PERMISSION_DENIED) {
    log('ERROR', 'Location access was denied by the user.');  
  } else {
    log('ERROR', 'Location error (' + err.code + '): ' + err.message);
  }
}

/*
 * Explicitly request the user's location data (as determined by smartphone GPS).
 */
function getLocation() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {enableHighAccuracy: true, timeout: 10000, maximumAge: 10000}
  );
}

/* -------------------------------------------------------------------------- *
 *  ReconnectingWebScoket. Modified from:
 *  https://gist.githubusercontent.com/trevordixon/7239401/raw/f127a032de9039d7b8c48b524aa561c50e7a4033/ReconnectingWebSocket.js
 * -------------------------------------------------------------------------- */

// MIT License:
//
// Copyright (c) 2010-2012, Joe Walnes
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/**
 * This behaves like a WebSocket in every way, except if it fails to connect,
 * or it gets disconnected, it will repeatedly poll until it succesfully connects
 * again.
 *
 * It is API compatible, so when you have:
 *   ws = new WebSocket('ws://....');
 * you can replace with:
 *   ws = new ReconnectingWebSocket('ws://....');
 *
 * The event stream will typically look like:
 *  onconnecting
 *  onopen
 *  onmessage
 *  onmessage
 *  onclose // lost connection
 *  onconnecting
 *  onopen  // sometime later...
 *  onmessage
 *  onmessage
 *  etc... 
 *
 * It is API compatible with the standard WebSocket API.
 *
 * Latest version: https://github.com/joewalnes/reconnecting-websocket/
 * - Joe Walnes
 */
// var WebSocket = require('ws');

// function ReconnectingWebSocket(url, protocols) {
//     protocols = protocols || [];

//     // These can be altered by calling code.
//     this.debug = false;
//     this.reconnectInterval = 1000;
//     this.timeoutInterval = 2000;

//     var self = this;
//     var ws;
//     var forcedClose = false;
//     var timedOut = false;
    
//     this.url = url;
//     this.protocols = protocols;
//     this.readyState = WebSocket.CONNECTING;
//     this.URL = url; // Public API

//     this.onopen = function(event) {
//         console.log('connected.');
//     };

//     this.onclose = function(event) {
//         console.log('disconnected.');
//     };

//     this.onconnecting = function(event) {
//         console.log('connecting to \'' + url + '\'...');
//     };

//     this.onmessage = function(event) {
//         console.log('message: ' + event.data);
//     };

//     this.onerror = function(event) {
//         // if (self.debug || ReconnectingWebSocket.debugAll) {
//             console.log('  -> ' + JSON.stringify(event));
//         // }
//     };

//     function connect(reconnectAttempt) {
//         console.log('CONNECT called - reconnectAttempt='+reconnectAttempt);
//         ws = new WebSocket(url, protocols);
        
//         self.onconnecting();
//         if (self.debug || ReconnectingWebSocket.debugAll) {
//             console.log('ReconnectingWebSocket', 'attempt-connect', url);
//         }
        
//         var localWs = ws;
//         var timeout = setTimeout(function() {
//             if (self.debug || ReconnectingWebSocket.debugAll) {
//                 console.log('ReconnectingWebSocket', 'connection-timeout', url);
//             }
//             timedOut = true;
//             localWs.close();
//             timedOut = false;
//         }, self.timeoutInterval);
        
//         ws.onopen = function(event) {
//             clearTimeout(timeout);
//             if (self.debug || ReconnectingWebSocket.debugAll) {
//                 console.log('ReconnectingWebSocket', 'onopen', url);
//             }
//             self.readyState = WebSocket.OPEN;
//             reconnectAttempt = false;
//             self.onopen(event);
//         };
        
//         ws.onclose = function(event) {
//             clearTimeout(timeout);
//             ws = null;
//             if (forcedClose) {
//                 self.readyState = WebSocket.CLOSED;
//                 self.onclose(event);
//             } else {
//                 self.readyState = WebSocket.CONNECTING;
//                 // self.onconnecting();
//                 if (!reconnectAttempt && !timedOut) {
//                     if (self.debug || ReconnectingWebSocket.debugAll) {
//                         console.log('ReconnectingWebSocket', 'onclose', url);
//                     }
//                     self.onclose(event);
//                 }
//                 setTimeout(function() {
//                     console.log('ReconnectingWebSocket', 'RECONNECTING?!!!  ', url);
//                     connect(true);
//                 }, self.reconnectInterval);
//             }
//         };

//         ws.onmessage = function(event) {
//             if (self.debug || ReconnectingWebSocket.debugAll) {
//                 console.log('ReconnectingWebSocket', 'onmessage', url, event.data);
//             }
//             self.onmessage(event);
//         };

//         ws.onerror = function(event) {
//             if (self.debug || ReconnectingWebSocket.debugAll) {
//                 console.log('ReconnectingWebSocket', 'onerror', url, event.data);
//             }
//             self.onerror(event);
//             //SHOULD ws.onclose() BE CALLED RIGHT AFTER THIS?
//         };
//     }

//     // Create the actual WebSocket connection! 
//     // connect(url);
//     connect(false);

//     this.send = function(data) {
//         if (ws) {
//             console.log('*******REALLY??!!!!***********', 'send', url, data);
//             if (self.debug || ReconnectingWebSocket.debugAll) {
//                 console.log('ReconnectingWebSocket', 'send', url, data);
//             }
//             return ws.send(data);
//         // } else {
//         //     throw 'INVALID_STATE_ERR : Pausing to reconnect websocket';
//         }
//     };

//     this.close = function() {
//         if (ws) {
//             forcedClose = true;
//             ws.close();
//         }
//     };

//     /**
//      * Additional public API method to refresh the connection if still open (close, re-open).
//      * For example, if the app suspects bad data / missed heart beats, it can try to refresh.
//      */
//     this.refresh = function() {
//         if (ws) {
//             ws.close();
//         }
//     };
// }

/**
 * Setting this to true is the equivalent of setting all instances of ReconnectingWebSocket.debug to true.
 */
// ReconnectingWebSocket.debugAll = true;
// module.exports = ReconnectingWebSocket;
// module.exports.ReconnectingWebSocket = ReconnectingWebSocket;

/* -------------------------------------------------------------------------- *
 *  WebSocket communication handlers.
 * -------------------------------------------------------------------------- */

function onOpen(evt) {
  var msg = 'connected to ' + wsUri;
  log('WEBSOCKET', msg);
}

function onClose(evt) {
  var msg = 'disconnected from ' + wsUri;
  log('WEBSOCKET', msg);

  // TODO: try to reconnect at some point?
}

function onReceive(evt) {
  // Incoming data is represented as Array Buffer -- convert and send to pebble.
  var msg = evt.data; //ab2str(evt.data);
  log('WEBSOCKET-RX', msg);

  // Received message from the server -- forward to the pebble.
  sendStringToPebble('AppKeyRecvMsg', msg); 
}

function onError(evt) {
  var msg = evt.data;
  log('WEBSOCKET-ERROR', msg);
}

function doWsSend(msg) {
  log('WEBSOCKET-TX', msg);
  websocket.send(msg);
}

/*
 * "Constructor" for the WebSocket connection.
 */
function initWebSocket(host, port) {
  wsUri = 'ws://'+host+':'+port;
  websocket = new WebSocket(wsUri);
  // websocket = new ReconnectingWebSocket(wsUri);

  // log('WEBSOCKET', 'connecting to \'' + wsUri + '...\'');

  // Setup handlers for open/close/receive/error events.
  websocket.onopen = function(evt) { onOpen(evt); };
  websocket.onclose = function(evt) { onClose(evt); };
  websocket.onmessage = function(evt) { onReceive(evt); };
  websocket.onerror = function(evt) { onError(evt); };
}

/* -------------------------------------------------------------------------- *
 *  Pebble BT communication handlers. 
 * -------------------------------------------------------------------------- */

/* 
 * Setup listener for the AppMessage ('appmessage') event, which fires when an 
 * AppMessage is sent -from- the Pebble smartwatch -to- the smartphone.
 */
Pebble.addEventListener('appmessage', function(e) {
    var msg = 'AppMessage from Pebble received: ' + JSON.stringify(e.payload) + ' ';
    var key;

    if (e.payload['AppKeyJSReady']) {
      // no-op -- cool...I sent this to you first...and this means nothing to 
      // me now...I was just letting you know you can talk to me now...
      key = 'AppKeyJSReady';

    } else if (e.payload['AppKeySendMsg']) {
      // Send message from pebble on to the proxy.
      key = 'AppKeySendMsg';
      doWsSend(e.payload[key]); 

    } else if (e.payload['AppKeyRecvMsg']) {
      // no-op -- pebble can't tell ME it received a message from the Game Server
      // because only I can do that! Silly pebble...you're drunk pebble, go home.
      key = 'AppKeyRecvMsg';

    } else if (e.payload['AppKeyLocation']) {
      // Get an update on the location per the request of the pebble watch and 
      // send location data back.
      key = 'AppKeyLocation';
      getLocation();

    } else if (e.payload['AppKeyPebbleId']) {
      // Send the super unique pebble ID (i.e., watch token) back.
      key = 'AppKeyPebbleId';
      sendStringToPebble(key, PEBBLE_DEVICE_ID);

    } else if (e.payload['AppKeySendError']) {
      // no-op -- why are you telling me this? I was being nice and letting you 
      // know that I can't seem to talk to YOUR proxy / Game Server...figure it 
      // out yourself, pebble!
      key = 'AppKeySendError';

    } else {
      // no-op -- really pebble? you don't make any sense...
      key = 'UnknownKey';
    }

    log('BT-RX', msg); //+ ' <' + key + ':' + e.payload[key] + '>');
  }
);

/*
 * Wrapper function for sending an AppMessage -to- the Pebble smartwatch -from-
 * the smartphone.
 */
function sendStringToPebble(key, value) {
  // Build message
  var dict = {};
  dict[key] = value;

  Pebble.sendAppMessage(dict, 
    function() { // ACK (i.e., communication acknowledged by pebble watch)
      log('BT-TX', 'sendAppMessage success: \'{' + key + " : " + value + '}\'');
    }, function() { // NACK (i.e., communication not acknowledged by pebble watch)
      log('BT-TX', 'sendAppMessage failed: \'{' + key + " : " + value + '}\'');
  });
}

/* -------------------------------------------------------------------------- *
 *  Pebble system handlers. 
 * -------------------------------------------------------------------------- */

/* 
 * Setup listener for the 'ready' event, which fires when the JS environment on 
 * the phone is first available after launch. 
 *
 * This is really the "constructor" or "initializer" for the companion app side.
 */
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS Environment ready!');
    PEBBLE_DEVICE_ID = Pebble.getWatchToken();

    // Update s_js_ready on the pebble watch.
    sendStringToPebble('AppKeyJSReady', PEBBLE_DEVICE_ID);

    // Try to establish connection to proxy over WebSocket.
    initWebSocket( appinfo.config['proxyWSSServerURL'], appinfo.config['proxyWSSServerPort'] ); 

    /*
     * Debug Output Upon JS Environment Initialization. 
     */
    // Get the account token (a unique account token that is associated with the Pebble account of the current user)
    log('DEBUG', 'Pebble Account Token: ' + Pebble.getAccountToken());    

    // Get the watch token (a unique token that can be used to identify a Pebble device)
    log('DEBUG', 'Pebble Watch Token: ' + Pebble.getWatchToken());

    // Configuration data.
    log('DEBUG', 'CONFIG:: ' + JSON.stringify(appinfo));
    log('DEBUG', ' proxy server: ' + appinfo.config['proxyWSSServerURL'] + ':' + appinfo.config['proxyWSSServerPort']);
    log('DEBUG', 'target server: ' + appinfo.config['targetServerURL'] + ':' + appinfo.config['targetServerPort']);

    /*
     * TODO: SEND NOTIFICATION WHEN NEW MESSAGE COMES IN? OR JUST SEND KEY WITH MESSAGE? 
     */
    // var title = 'New Intel Available!';
    // var body = 'Your Pebble Watch Token: ' + Pebble.getWatchToken();
    // Pebble.showSimpleNotificationOnPebble(title, body);
  }
);
