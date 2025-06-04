#ifndef AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_
#define AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_

#include <tuple>
#include <chrono>
#include <string>

#include <alsa/asoundlib.h>

#include "audio_format.h"
#include "audiodevice.h"

namespace audiodevice {

class AlsaAudioDevice : public AudioDevice {
 public:
  AlsaAudioDevice(AudioFormat *format);
  virtual ~AlsaAudioDevice();

  std::tuple<Error, std::string> Open(const std::string &device_name,
                                      OpenType device_type) override;
  void Close() override;

  void SetBufferTime(const std::chrono::milliseconds &buffer_time) override;
  void SetPeriodTime(const std::chrono::milliseconds &period_time) override;

  Error Write(const void *data, size_t frames) override;
  Error Write(const void *data, const std::chrono::milliseconds &duration) override;
  Error Write(const void *data, const std::chrono::seconds &duration) override;

  Error Read(void *buffer, const std::chrono::milliseconds &duration) override;
  Error Read(void *buffer, int frames) override;

  void Drain() override;
  void Stop() override;

  State state() override;

 private:
  snd_pcm_t *handle_ = nullptr;
  snd_pcm_hw_params_t *params_ = nullptr;
};

}  // namespace audiodevice

#endif  // AUDIODEVICE_ALSA_BACKED_ALSA_AUDIO_DEVICE_H_
