var e131 = require('e131');
 
var client = new e131.Client('192.168.178.34');  // or use a universe
var packet = client.createPacket(16);  // we want 8 RGB (x3) slots
var slotsData = packet.getSlotsData();
packet.setSourceName('test E1.31 client');
packet.setUniverse(0x01);  // make universe number consistent with the client
packet.setOption(packet.Options.PREVIEW, true);  // don't really change any fixture
packet.setPriority(packet.DEFAULT_PRIORITY);  // not strictly needed, done automatically

slotsData[0] = 100;
slotsData[1] = 100;
slotsData[2] = 100;
slotsData[13] = 100;
slotsData[14] = 100;
slotsData[15] = 255;  
client.send(packet) 

// slotsData is a Buffer view, you can use it directly
var color = 0;
function cycleColor() {
  for (var idx=0; idx<slotsData.length; idx++) {
    slotsData[idx] = color % 0xff;
    color = color + 90;
  }
  client.send(packet, function () {
    setTimeout(cycleColor, 125);
  });
} 
// cycleColor() 