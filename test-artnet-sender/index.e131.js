var e131 = require('e131');

const l = 30; // RGB Mode
// const l = 10; // white mode
// var client = new e131.Client('192.168.178.23');
var client = new e131.Client('192.168.178.34');
var packet = client.createPacket(l);  // we want 8 RGB (x3) slots
var slotsData = packet.getSlotsData();
packet.setSourceName('test E1.31 client');
packet.setUniverse(0x01);  // make universe number consistent with the client
packet.setOption(packet.Options.PREVIEW, true);  // don't really change any fixture
packet.setPriority(packet.DEFAULT_PRIORITY);  // not strictly needed, done automatically

setRGB(0, 255, 0, 0);
setRGB(1, 0, 255, 0);
setRGB(2, 0, 0, 255);
setRGB(5, 255, 255, 255);
//cycleColor() 
staticColor()

function setRGB(index, r, g, b) {
  const i = index * 3;
  slotsData[i] = r;
  slotsData[i + 1] = g;
  slotsData[i + 2] = b;
}

function staticColor() {
  client.send(packet, function () {
    setTimeout(staticColor, 500);
  });
}

function cycleColor() {
  let first = slotsData[0];
  for (let i = 1; i < l; ++i) {
    slotsData[i - 1] = slotsData[i];
  }
  slotsData[l - 1] = first;
  client.send(packet, function () {
    setTimeout(cycleColor, 50);
  });
} 