
#include "artnet.h"

//------------------------------------------------------------------------------
Artnet::Artnet()
    : LOG("Artnet"),
      // artnet_(),
      e131(UNIVERSE_COUNT),
      // universe_(0),
      timeoutUs_(0),
      lastPaketTimestampUs_(0),
      dataCallback_(NULL),
      timeoutCallback_(NULL)
// permanentDataCallback_(NULL),
// resetCallback_(NULL)
//------------------------------------------------------------------------------
{}

// //------------------------------------------------------------------------------
// void Artnet::onArtnetData(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
// //------------------------------------------------------------------------------
// {
//   // LOG.i("Paket received. Universe: '%u', sequence: '%u', length: '%d'.", universe, sequence, length);
//   //   for (uint8_t i = 0; (i < length) && (i < 16); ++i) {
//   //     LOG.d("[%u] = %u", i, data[i]);
//   //   }
//   //   if (length > 16) {
//   //     LOG.d("...");
//   //   }

//   // we only use the first paket
//   if (sequence == 0 && length > 0 && universe == universe_) {
//     // switch ((messageType_t)data[0]) {
//     // case PERMANENT: {
//     //   if (permanentDataCallback_ && length > 1) {
//     //     permanentDataCallback_(data[1], (uint16_t*)&data[4], (length - 4) >> 1);
//     //   }
//     // } break;
//     // case TEMPORARY: {
//     if (dataCallback_ && length > 1) {
//       dataCallback_(data, length);
//     }
//     // } break;
//     // case RESET:
//     //   if (resetCallback_) {
//     //     resetCallback_();
//     //   }
//     //   break;
//     // }
//   }
// }

//------------------------------------------------------------------------------
bool Artnet::begin(uint16_t universe, uint64_t timeoutUs)
//------------------------------------------------------------------------------
{
  // universe_ = universe;

  // using namespace std;
  // using namespace std::placeholders;
  // Callback<void(uint16_t, uint16_t, uint8_t, uint8_t*)>::func = bind(&Artnet::onArtnetData, this, _1, _2, _3, _4);
  // callback_t func = static_cast<callback_t>(Callback<void(uint16_t, uint16_t, uint8_t, uint8_t*)>::callback);
  // artnet_.setArtDmxCallback(func);
  // artnet_.begin();

  timeoutUs_ = timeoutUs;
  return e131.begin(E131_MULTICAST, universe, UNIVERSE_COUNT);
}

//------------------------------------------------------------------------------
void Artnet::loop()
//------------------------------------------------------------------------------
{
  uint64_t now = esp_timer_get_time();
  // artnet_.read();
  if (!e131.isEmpty()) {
    lastPaketTimestampUs_ = now;
    e131_packet_t packet;
    e131.pull(&packet);  // Pull packet from ring buffer
    if (dataCallback_) {
      dataCallback_(&packet.property_values[1], htons(packet.property_value_count) - 1);
    }
  }

  if (timeoutCallback_ && lastPaketTimestampUs_ && lastPaketTimestampUs_ + timeoutUs_ < now) {
    Serial.println("XXXXX TIMEOUT");
    lastPaketTimestampUs_ = 0;
    timeoutCallback_();
  }
}

//------------------------------------------------------------------------------
void Artnet::onData(DataCallback cb)
//------------------------------------------------------------------------------
{
  dataCallback_ = cb;
}

//------------------------------------------------------------------------------
void Artnet::onTimeout(TimeoutCallback cb)
//------------------------------------------------------------------------------
{
  timeoutCallback_ = cb;
}
