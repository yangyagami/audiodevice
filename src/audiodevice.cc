#include "audiodevice.h"

#include <cassert>
#include <chrono>

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

size_t AudioDevice::ConvertBytesSizeToFrames(size_t size) {
  assert(audio_format_);

  return size / audio_format_->channels / (audio_format_->bit_depth / 8);
}

std::chrono::milliseconds AudioDevice::ConvertBytesSizeToTimeMS(size_t size) {
  assert(audio_format_);

  return std::chrono::duration_cast<std::chrono::milliseconds>(
      ConvertBytesSizeToTimeSec(size));
}

std::chrono::seconds AudioDevice::ConvertBytesSizeToTimeSec(size_t size) {
  assert(audio_format_);

  size_t frames = ConvertBytesSizeToFrames(size);

  return std::chrono::seconds(frames / audio_format_->sample_rate);
}

std::chrono::milliseconds AudioDevice::ConvertFramesToTimeMS(size_t frames) {
  assert(audio_format_);

  return std::chrono::milliseconds(frames / audio_format_->sample_rate * 1000);
}

}  // namespace audiodevice
