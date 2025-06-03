#ifndef AUDIODEVICE_AUDIODEVICE_CREATOR_H_
#define AUDIODEVICE_AUDIODEVICE_CREATOR_H_

#include "audiodevice.h"
#include "audio_format.h"

namespace audiodevice {

class AudioDeviceCreator {
 public:
  enum Backend {
    kAlsa = 0,
  };

  AudioDevice *Create(Backend backend, AudioFormat *format);
};

}  // namespace audiodevice

#endif  // AUDIODEVICE_AUDIODEVICE_CREATOR_H_
