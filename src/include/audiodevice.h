#ifndef AUDIODEVICE_AUDIODEVICE_H_
#define AUDIODEVICE_AUDIODEVICE_H_

#include <tuple>
#include <chrono>
#include <string>

#include "audio_format.h"

namespace audiodevice {

class AudioDevice {
 public:
  enum OpenType {
    kNone = 0,
    kRecorder,
    kPlayer,
  };

  enum Error {
    kNoError = 0,
    kAlreadyOpened,
    kWrongOpenType,
    kUnknownError,
  };

  enum State {
    kRunning = 0,
    kClosed,
  };

  AudioDevice(AudioFormat *format);
  virtual ~AudioDevice();

  virtual std::tuple<Error, std::string> Open(const std::string &device_name,
                                              OpenType device_type) = 0;
  virtual void Close() = 0;

  virtual void SetBufferTime(const std::chrono::milliseconds &buffer_time) = 0;
  virtual void SetPeriodTime(const std::chrono::milliseconds &period_time) = 0;

  virtual Error Write(const void *data, size_t frames) = 0;
  virtual Error Write(const void *data,
                      const std::chrono::milliseconds &duration) = 0;
  virtual Error Write(const void *data,
                      const std::chrono::seconds &duration) = 0;

  virtual Error Read(void *buffer,
                     const std::chrono::milliseconds &duration) = 0;
  virtual Error Read(void *buffer, int frames) = 0;

  virtual void Drain() = 0;
  virtual void Stop() = 0;

  State state() const;

 protected:
  AudioFormat *audio_format_;
  OpenType open_type_;
  State state_;

  std::chrono::milliseconds buffer_time_;
  std::chrono::milliseconds period_time_;
};

}  // namespace audiodevice

#endif  // AUDIODEVICE_AUDIODEVICE_H_
