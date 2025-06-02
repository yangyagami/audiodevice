#ifndef AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_
#define AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_

#include <tuple>
#include <chrono>
#include <string>

#include <alsa/asoundlib.h>

#include "audio_format.h"

namespace audiodevice {

class AlsaAudioDevice {
 public:
  enum OpenType {
    kNone = 0,
    kRecorder,
    kPlayer,
  };

  enum Error {
    kNoError = 0,
    kAlreadyOpened,
    kUnknowOpenType,

    kBackendError = 1000,
  };

  enum State {
    kRunning = 0,
    kClosed,
  };

  AlsaAudioDevice(AudioFormat *format);
  ~AlsaAudioDevice();

  std::tuple<Error, std::string> Open(const std::string &device_name,
                                      OpenType device_type);
  void Close();

  void SetBufferTime(const std::chrono::milliseconds &buffer_time);
  void SetPeriodTime(const std::chrono::milliseconds &period_time);

  void Write(const void *data, size_t frames);
  void Write(const void *data, const std::chrono::milliseconds &duration);
  void Write(const void *data, const std::chrono::seconds &duration);

  void Read(void *buffer, const std::chrono::milliseconds &duration);
  void Read(void *buffer, int frames);

  State state() const { return state_; }

 private:
  AudioFormat *audio_format_ = nullptr;
  OpenType open_type_ = kNone;
  State state_ = kClosed;

  std::chrono::milliseconds buffer_time_;
  std::chrono::milliseconds period_time_;

  snd_pcm_t *handle_ = nullptr;
  snd_pcm_hw_params_t *params_ = nullptr;
};

}  // namespace audiodevice

#endif  // AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_
