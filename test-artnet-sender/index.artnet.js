const artnet = require('artnet')({
    host: '192.168.178.34'
});

function send() {
    let data = new Array(16);
    data.fill(0);
for(let i=0; i<16;++i){

   data[i] = Math.random() * 256;
//    data[i] = i*16;
}
    console.log("SEND", data);
    artnet.set(1, 0, data);
}

setInterval(send, 50)
