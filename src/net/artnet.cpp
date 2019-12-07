
#include "artnet.h"

//------------------------------------------------------------------------------
Artnet::Artnet()
    : LOG("Artnet"),
      artnet_(),
      universe_(0),
      temporaryDataCallback_(NULL),
      permanentDataCallback_(NULL),
      resetCallback_(NULL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void Artnet::onArtnetData(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
//------------------------------------------------------------------------------
{
//   LOG.d("Paket received. Universe: '%u', sequence: '%u', length: '%d'.", universe, sequence, length);
//   for (uint8_t i = 0; (i < length) && (i < 16); ++i) {
//     LOG.d("[%u] = %u", i, data[i]);
//   }
//   if (length > 16) {
//     LOG.d("...");
//   }

  // we only use the first paket
  if (sequence == 0 && length > 0 && universe == universe_) {
    switch ((messageType_t)data[0]) {
      case PERMANENT: {
        if (permanentDataCallback_ && length > 1) {
          permanentDataCallback_(data[1], (uint16_t*)&data[4], (length - 4) >> 1);
        }
      } break;
      case TEMPORARY: {
        if (permanentDataCallback_ && length > 1) {
          temporaryDataCallback_(data[1], (uint16_t*)&data[4], (length - 4) >> 1);
        }
      } break;
      case RESET:
        if (resetCallback_) {
          resetCallback_();
        }
        break;
    }
  }
}

//------------------------------------------------------------------------------
bool Artnet::begin(uint16_t universe)
//------------------------------------------------------------------------------
{
  universe_ = universe;

  using namespace std;
  using namespace std::placeholders;
  Callback<void(uint16_t, uint16_t, uint8_t, uint8_t*)>::func = bind(&Artnet::onArtnetData, this, _1, _2, _3, _4);
  callback_t func = static_cast<callback_t>(Callback<void(uint16_t, uint16_t, uint8_t, uint8_t*)>::callback);
  artnet_.setArtDmxCallback(func);

  artnet_.begin();
  return true;  // hopefully...
}

//------------------------------------------------------------------------------
void Artnet::loop()
//------------------------------------------------------------------------------
{
  artnet_.read();
}

//------------------------------------------------------------------------------
void Artnet::onTemporaryData(DataCallback cb)
//------------------------------------------------------------------------------
{
  temporaryDataCallback_ = cb;
}

//------------------------------------------------------------------------------
void Artnet::onPermanentData(DataCallback cb)
//------------------------------------------------------------------------------
{
  permanentDataCallback_ = cb;
}

//------------------------------------------------------------------------------
void Artnet::onResetData(ResetCallback cb)
//------------------------------------------------------------------------------
{
  resetCallback_ = cb;
}