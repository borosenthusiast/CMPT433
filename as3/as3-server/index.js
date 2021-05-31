// After creating package.json, install modules:
//   $ npm install
//
// Launch app with:
//   $ node index.js
// based off of the demo code
var config = require('./config.js');
const express = require('express');
const PORT_NUMBER = config.srv_port;

var indexRoute = require('./routes/indexRoute');
var checkRoute = require('./routes/checkRoute');
var http = require('http')

/* 
 * Create the static web app
 */
var app = express();
var server = http.createServer(app).listen(PORT_NUMBER);
app.use('/', indexRoute);
app.use('/nodeCheck', checkRoute);
/*
 * Create the Userver to listen for the websocket
 */
var udpServer = require('./lib/udp_server');
udpServer.listen(server);

