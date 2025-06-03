#include "audiodevice_creator.h"

#include <cassert>

#include "audiodevice.h"
#include "audio_format.h"
#include "alsa_backend/alsa_audio_device.h"

namespace audiodevice {

AudioDevice *AudioDeviceCreator::Create(AudioDeviceCreator::Backend backend,
                                        AudioFormat *format) {
  switch (backend) {
    case kAlsa: {
      return new AlsaAudioDevice(format);
    }
    default: {
      assert(false);
    }
  }

  return nullptr;
}

}  // namespace audiodevice
