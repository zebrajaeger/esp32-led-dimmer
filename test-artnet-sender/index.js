const os = require('os');
const broadcastAddress = require('broadcast-address');

// get commandline arguments
let use = null;
if (process.argv[2]) {
    use = process.argv[2];
}

let universe = 0;
if (process.argv[3]) {
    universe = process.argv[3];
}

// get network names
const obj = os.networkInterfaces();
let names = [];
for (var name in obj) {
    names.push(name);
}
names.sort();

// show network names, selection and broadcast address
let broadcast = null;
console.log("Found Networks:")
var index = 0;
for (var name of names) {
    const netBroadcast = broadcastAddress(name);
    if (name == use || index == use) {
        console.log(index++, ">>>", name, netBroadcast);
        broadcast = netBroadcast;
    } else {
        console.log(index++, "   ", name, netBroadcast);
    }
}

console.log("Using", broadcast);
console.log("Universe", universe);
if (broadcast) {
    // start sender
    const artnet = require('artnet')({
        host: broadcast
    });

    let data = new Array((1 + 1 + 16) * 2);
    let index = 0;

    function setValue16(adr,value){
        let index = adr + adr;
        data[index] = (value >> 8) & 0xff;
        data[index+1] = value & 0xff;
    }

    function send() {
        data.fill(0);
        index = (index + 1) % 16;
        setValue16(0,1); // temporary
        setValue16(1,1000); // 1kHz
        setValue16(index+2, 4095);
        console.log("SEND", data);
        artnet.set(universe, 1, data);
        setTimeout(send, 1000)
    }

    console.log("START");
    send();
} else {
    // no start possible, show help
    console.log("No broadcast adress -> no artnet sender start possible");
    console.log("usage:");
    console.log("----------------------------------------");
    console.log("npm start <net*> [<universe**>]");
    console.log("");
    console.log("* net is the index of the table or the name of the network in table.");
    console.log("** default universe is 0.");
    console.log("----------------------------------------");
}
