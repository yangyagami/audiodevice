#include "audiodevice.h"

#include "audio_format.h"

using namespace std::chrono_literals;

namespace audiodevice {

AudioDevice::AudioDevice(AudioFormat *format)
    : audio_format_(format),
      open_type_(kNone),
      state_(kClosed),
      buffer_time_(0ms),
      period_time_(0ms) {}

AudioDevice::~AudioDevice() {
}

AudioDevice::State AudioDevice::state() const {
  return state_;
}

}  // namespace audiodevice
