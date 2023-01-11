
#include "artnet.h"

//------------------------------------------------------------------------------
Artnet::Artnet()
    : LOG("Artnet"),
      e131(UNIVERSE_COUNT),
      timeoutUs_(0),
      lastPaketTimestampUs_(0),
      dataCallback_(NULL),
      timeoutCallback_(NULL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Artnet::begin(uint16_t universe, uint64_t timeoutUs)
//------------------------------------------------------------------------------
{
  timeoutUs_ = timeoutUs;
  return e131.begin(E131_MULTICAST, universe, UNIVERSE_COUNT);
}

//------------------------------------------------------------------------------
void Artnet::loop()
//------------------------------------------------------------------------------
{
  uint64_t now = esp_timer_get_time();
  if (!e131.isEmpty()) {
    lastPaketTimestampUs_ = now;
    e131_packet_t packet;
    e131.pull(&packet);  // Pull packet from ring buffer
    if (dataCallback_) {
      dataCallback_(&packet.property_values[1], htons(packet.property_value_count) - 1);
    }
  }

  if (timeoutCallback_ && lastPaketTimestampUs_ && lastPaketTimestampUs_ + timeoutUs_ < now) {
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
