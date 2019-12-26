// thx to https://github.com/websockets/ws/wiki/Websocket-client-implementation-for-auto-reconnect

let wsClient = {};

wsClient.autoReconnectInterval = 5 * 1000;
wsClient.url = null;
wsClient.ws = null;

wsClient.init = (url) => {
    wsClient.url = url;
};

wsClient.open = () => {
    if (wsClient.ws) {
        wsClient.ws.onopen = null;
        wsClient.ws.onmessage = null;
        wsClient.ws.onclose = null;
        wsClient.ws.onerror = null;
    }

    wsClient.ws = new WebSocket(wsClient.url);

    wsClient.ws.onopen = (websocket, ev) => {
        // console.log('WebSocket: ON_OPEN');
        wsClient.onOpen(websocket, ev)
    };

    wsClient.ws.onmessage = (data, flags) => {
        // console.log('WebSocket: ON_MESSAGE', data.data);
        wsClient.onMessage(data.data, flags)
    };

    wsClient.ws.onclose = (e) => {
        if (e.code === 1000) {
            // console.log('WebSocket: CLOSED', e);
        } else {
            // console.log('WebSocket: CLOSED, RECONNECT', e);
            wsClient.reconnect(e);
        }
        wsClient.onClose(e);
    };

    wsClient.ws.onerror = (e) => {
        if (e.code === 'ECONNREFUSED') {
            // console.log('WebSocket: ERROR, RECONNECT', e);
            wsClient.reconnect(e);
        } else {
            // console.log('WebSocket: ERROR', e);
            wsClient.onError(e);
        }
    };
};

wsClient.send = (data) => {
    // console.log('WebSocket: SEND', data);
    wsClient.ws.send(data);
};

wsClient.reconnect = (e) => {
    // console.log(`WebSocketClient: retry in ${this.autoReconnectInterval}ms`, e);
    let that = this;
    setTimeout(function () {
        // console.log("WebSocketClient: reconnecting...");
        that.open(that.url);
    }, wsClient.autoReconnectInterval);
};

wsClient.onOpen = (e) => {
    // console.log("WebSocketClient: open", e);
};

wsClient.onMessage = (data, flags) => {
    console.log("WebSocketClient: message", data, flags);
};

wsClient.onError = (e) => {
    console.log("WebSocketClient: error", e);
};

wsClient.onClose = (e) => {
    console.log("WebSocketClient: closed", e);
};
