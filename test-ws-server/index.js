const fs = require('fs');
const path = require('path');

const express = require('express')();
const WebSocket = require('ws');

const open = require('open');

let channels = new Array(16);
channels.fill(1024);

// http-server
express.get('/', function (req, res) {
    res.sendFile(path.join(__dirname, 'index.html'));
});
express.listen(80);


// WS Server
const wss = new WebSocket.Server({port: 81});
wss.on('connection', ws => {
        console.log('WS connected');

    let timerHandler = setInterval(() => {
        console.log("WS SEND");
        ws.send(JSON.stringify(channels));
    }, 2000);

    ws.on('message', message => {
        console.log('WS received: %s', message);
        ws.send(JSON.stringify(channels));
    });

    ws.on('close', function close() {
        console.log('WS disconnected');
        clearInterval(timerHandler);
    });

});

console.log("STARTED");
open('http://localhost:80');
