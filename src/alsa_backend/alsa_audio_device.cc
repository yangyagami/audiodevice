#include "alsa_audio_device.h"

#include <iostream>
#include <vector>
#include <cassert>

#include <alsa/asoundlib.h>

#include "audio_format.h"

using namespace std::chrono_literals;

namespace audiodevice {

AlsaAudioDevice::AlsaAudioDevice(AudioFormat *format)
    : AudioDevice(format) {}

AlsaAudioDevice::~AlsaAudioDevice() {
  Close();
}

std::tuple<AudioDevice::Error, std::string> AlsaAudioDevice::Open(
    const std::string &device_name,
    OpenType open_type) {
  if (!handle_ && !params_ && state_ != kRunning) {
    int dir = 0;
    int rc;

    snd_pcm_stream_t stream_type;
    // 打开 PCM 设备
    switch (open_type) {
      case AudioDevice::kRecorder: {
        stream_type = SND_PCM_STREAM_CAPTURE;
        break;
      }
      case AudioDevice::kPlayer: {
        stream_type = SND_PCM_STREAM_PLAYBACK;
        break;
      }
      default: {
        return std::make_tuple(AudioDevice::kWrongOpenType, "");
      }
    }
    rc = snd_pcm_open(&handle_, device_name.c_str(),
                      stream_type, 0);
    if (rc < 0) {
      std::string error = "Unable to open pcm device: ";
      error += device_name + ", ";
      error += snd_strerror(rc);
      error += ", rc: ";
      error += std::to_string(rc);

      return std::make_tuple(
          static_cast<Error>(AudioDevice::kUnknownError), error);
    }

    snd_pcm_hw_params_alloca(&params_);
    snd_pcm_hw_params_any(handle_, params_);

    // 设置参数
    snd_pcm_format_t sample_format;
    unsigned int sample_rate;
    unsigned int channels;

    switch (audio_format_->bit_depth) {
      case 8: {
        if (audio_format_->sample_format == AudioFormat::kSigned) {
          sample_format = SND_PCM_FORMAT_S8;
        } else {
          sample_format = SND_PCM_FORMAT_U8;
        }
        break;
      }
      case 16: {
        if (audio_format_->sample_format == AudioFormat::kSigned) {
          if (audio_format_->byte_order == AudioFormat::kLittleEndian) {
            sample_format = SND_PCM_FORMAT_S16_LE;
          } else {
            sample_format = SND_PCM_FORMAT_S16_BE;
          }
        } else {
          if (audio_format_->byte_order == AudioFormat::kLittleEndian) {
            sample_format = SND_PCM_FORMAT_U16_LE;
          } else {
            sample_format = SND_PCM_FORMAT_U16_BE;
          }
        }
        break;
      }
      default: {
        assert(false);
      }
    }

    sample_rate = audio_format_->sample_rate;
    channels = audio_format_->channels;

    snd_pcm_hw_params_set_access(handle_, params_, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle_, params_, sample_format);
    snd_pcm_hw_params_set_rate_near(handle_,
                                    params_,
                                    &sample_rate,
                                    &dir);
    snd_pcm_hw_params_set_channels(handle_, params_, channels);

    rc = snd_pcm_hw_params(handle_, params_);
    if (rc < 0) {
      std::string error = "Unable to set HW parameters, ";
      error += snd_strerror(rc);
      error += ", rc: ";
      error += std::to_string(rc);

      return std::make_tuple(
          static_cast<Error>(AudioDevice::kUnknownError), error);
    }

    audio_format_->sample_rate = sample_rate;
  } else {
    return std::make_tuple(AudioDevice::kAlreadyOpened, "");
  }

  open_type_ = open_type;
  state_ = kRunning;

  return std::make_tuple(AudioDevice::kNoError, "");
}

void AlsaAudioDevice::Close() {
  if (handle_ && params_) {
    if (open_type_ == kPlayer) {
      snd_pcm_drain(handle_);
    }
    snd_pcm_close(handle_);

    handle_ = nullptr;
    params_ = nullptr;
    state_ = kClosed;
    open_type_ = kNone;
  }
}

void AlsaAudioDevice::SetBufferTime(
    const std::chrono::milliseconds &buffer_time) {
  buffer_time_ = buffer_time;
}

void AlsaAudioDevice::SetPeriodTime(
    const std::chrono::milliseconds &period_time) {
  period_time_ = period_time;
}

AudioDevice::Error AlsaAudioDevice::Write(const void *data, size_t frames) {
  assert(handle_);

  if (open_type_ == kPlayer) {
    snd_pcm_state_t state = snd_pcm_state(handle_);

    if (state != SND_PCM_STATE_PREPARED && state != SND_PCM_STATE_RUNNING) {
      snd_pcm_prepare(handle_);
    }

    std::cout << snd_pcm_state_name(snd_pcm_state(handle_)) << std::endl;

    int rc = snd_pcm_writei(handle_, data, frames);

    if (rc < 0) {
      std::cout << "write failed state:" << snd_pcm_state_name(snd_pcm_state(handle_)) << std::endl;
      rc = snd_pcm_recover(handle_, rc, 0);
      if (rc < 0) {
        std::cerr << "pcm write failed: " << snd_strerror(rc) << std::endl;
      } else {
        rc = snd_pcm_writei(handle_, data, frames);
      }
    }

    // TODO(yangsiyu): Handle return code.
  }

  return AudioDevice::kNoError;
}

AudioDevice::Error AlsaAudioDevice::Write(const void *data,
                            const std::chrono::milliseconds &duration) {
  assert(handle_);

  if (open_type_ == kPlayer && duration.count() > 0) {
    Write(data, audio_format_->sample_rate * duration.count() / 1000);
  }

  return AudioDevice::kNoError;
}

AudioDevice::Error AlsaAudioDevice::Write(const void *data,
                                          const std::chrono::seconds &duration) {
  assert(handle_);

  if (open_type_ == kPlayer && duration.count() > 0) {
    Write(data, audio_format_->sample_rate * duration.count());
  }

  return AudioDevice::kNoError;
}

AudioDevice::Error AlsaAudioDevice::Read(void *buffer,
                           const std::chrono::milliseconds &duration) {
  return AudioDevice::kNoError;
}

AudioDevice::Error AlsaAudioDevice::Read(void *buffer, int frames) {
  return AudioDevice::kNoError;
}

void AlsaAudioDevice::Drain() {
  assert(handle_);
  if (open_type_ != kNone) {
    snd_pcm_drain(handle_);
  }
}

void AlsaAudioDevice::Stop() {
  assert(handle_);

  if (open_type_ == kPlayer) {
    snd_pcm_drop(handle_);
    std::cout << snd_pcm_state_name(snd_pcm_state(handle_)) << std::endl;
  }
}

AudioDevice::State AlsaAudioDevice::state() {
  assert(handle_);

  switch (snd_pcm_state(handle_)) {
    case SND_PCM_STATE_OPEN:
    case SND_PCM_STATE_SETUP:
    case SND_PCM_STATE_PREPARED: {
      state_ = AudioDevice::kIdle;
      break;
    }
    case SND_PCM_STATE_RUNNING: {
      state_ = AudioDevice::kRunning;
      break;
    }
    default: {
      // TODO(yangsiyu): Handle remain status
      break;
    }
  }

  return state_;
}

}  // namespace audiodevice
