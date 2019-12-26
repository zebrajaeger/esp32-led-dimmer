let app = {};

app.wsUrl = null;
app.pendingValue = null;
app.connected = false;

app.idle = true;

app.pollSendMessage = () => {
    if (app.connected && app.idle && app.pendingValue) {
        app.idle = false;
        wsClient.send(JSON.stringify(app.pendingValue));
        app.pendingValue = null;
    }
};

app.onWSMessage = (msg) => {
    let msgO = JSON.parse(msg);
    if (msgO.data) {
        // todo check size, values
        // this message is send after connecting or setting channelData otherwise (i.E. MQTT, Artnet)
        sliders.setChannels(app.unMapArray(msgO.data));
    } else {
        // response of set channels has no data, only msgId
        app.idle = true;
        app.pollSendMessage();
    }
};

app.sendMessage = (json) => {
    app.pendingValue = json;
    app.pollSendMessage();
};

app.onSliderChange = (values) => {
    // console.log("APP.sliderChange", values)
    app.sendMessage({
        msgId: utils.uuid(),
        data: app.mapArray(values)
    });
};

app.mapValue = (v) => {
    let a = Math.ceil(Math.pow(4095, v / 4095));
    if (a > 4095) a = 4095;
    if (a < 0) a = 0;
    return a;
};

app.unMapValue = (v) => {
    // a = pow(b, 5)
    // b = pow (a, 1.0 / 5);

    // a = Math.pow(b, c);
    // c = Math.log(a)/Math.log(b)

    // example
    // b = 4095
    // c = v/4095
    // a = Math.pow(4095, 100/4095) = 1.2252122450615488
    // c = (Math.log(a)/Math.log(b)) * 4095
    // c = (Math.log(1.2252122450615488)/Math.log(4095)) * 4095 = 99.99999999999996 -> ok

    let c = (Math.log(v) / Math.log(4095)) * 4095;
    if (c > 4095) c = 4095;
    if (c < 0) c = 0;
    return c;
};

app.mapArray = (vA) => {
    let result = new Array(vA.length);
    for (let i = 0; i < vA.length; ++i) {
        result[i] = app.mapValue(vA[i]);
    }
    return result;
};

app.unMapArray = (vA) => {
    let result = new Array(vA.length);
    for (let i = 0; i < vA.length; ++i) {
        result[i] = app.unMapValue(vA[i]);
    }
    return result;
};

app.init = () => {
    utils.init(8);
    sliders.init();

    wsClient.init('ws://' + new URL(window.location.href).hostname + ':81/');
    wsClient.open();

    wsClient.onOpen = (e) => {
        app.connected = true;
        app.pollSendMessage();
        $('.connectionstate').addClass('connected');
    };

    wsClient.onClose = () => {
        app.connected = false;
        $('.connectionstate').removeClass('connected');
    };

    wsClient.onMessage = app.onWSMessage;

    sliders.onChange = app.onSliderChange;
};

$(() => {
    app.init();
});
