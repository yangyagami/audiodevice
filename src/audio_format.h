#ifndef AUDIODEVICE_AUDIO_FORMAT_H_
#define AUDIODEVICE_AUDIO_FORMAT_H_

namespace audiodevice {

struct AudioFormat {
  enum SampleFormat {
    kSigned = 0,
    kUnsigned,
    // TODO(yangsiyu): more sample format
  };

  enum ByteOrder {
    kLittleEndian = 0,
    kBigEndian,
  };

  int sample_rate;
  int channels;
  int bit_depth;
  SampleFormat sample_format;
  ByteOrder byte_order;

  AudioFormat()
      : sample_rate(8000),
        channels(1),
        bit_depth(8),
        sample_format(kSigned),
        byte_order(kLittleEndian) {
  }
};

}  // namespace audiodevice

#endif  // AUDIODEVICE_AUDIO_FORMAT_H_
