#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include <alsa/asoundlib.h>

#include "audiodevice_creator.h"
#include "audiodevice.h"
#include "audio_format.h"

#define SAMPLE_RATE 44100
#define FREQUENCY 440.0   // A4 note
#define DURATION 5.322    // seconds

using namespace std::chrono_literals;

int main() {
  // 计算样本数
  int samples = static_cast<int>(DURATION * SAMPLE_RATE);
  int16_t *buffer = new int16_t[samples];

  // 生成正弦波
  for (int i = 0; i < samples; ++i) {
    buffer[i] = static_cast<int16_t>(32767 * sin(2 * M_PI * FREQUENCY * i / SAMPLE_RATE));
  }

  audiodevice::AudioFormat audio_format;
  audio_format.sample_rate = 44100;
  audio_format.bit_depth = 16;

  audiodevice::AudioDeviceCreator audiodevice_creator;

  audiodevice::AudioDevice *player = audiodevice_creator.Create(
      audiodevice::AudioDeviceCreator::kAlsa,
      &audio_format);

  auto [error, msg] = player->Open("default", audiodevice::AudioDevice::kPlayer);

  if (error != audiodevice::AudioDevice::kNoError) {
    std::cout << error << ": " << msg << std::endl;
    delete[] buffer;
    return -1;
  }

  // std::thread t([&](){
  //   std::this_thread::sleep_for(800ms);
  //   player.Stop();
  // });
  // t.detach();

  auto start = std::chrono::high_resolution_clock::now();
  if (player->state() == audiodevice::AudioDevice::kIdle) {
    auto total_audio_time_ms = player->ConvertFramesToTimeMS(samples);
    std::cout << "总时间: " << total_audio_time_ms.count() << "ms" << std::endl;
    player->Write(buffer, total_audio_time_ms);
    start = std::chrono::high_resolution_clock::now();
  }

  bool loop = true;
  while (loop) {
    switch (player->state()) {
      case audiodevice::AudioDevice::kRunning: {
        // Playing
        auto end = std::chrono::high_resolution_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        std::cout << "经过: " << dt.count() << "ms" << std::endl;
        if (dt >= 5000ms) {
          std::cout << "播放完成" << std::endl;
          loop = false;
        }
        break;
      }
    }
    std::this_thread::sleep_for(20ms);
  }

  delete player;
  delete[] buffer;
  return 0;
}

// #include <stdio.h>

// #include <iostream>
// #include <cmath>
// #include <thread>

// #include <alsa/asoundlib.h>

// #define SAMPLE_RATE 44100
// #define FREQUENCY 440.0 // A4 note
// #define DURATION 5.0    // seconds

// using namespace std::chrono_literals;

// int main() {
//     snd_pcm_t *handle;
//     snd_pcm_hw_params_t *params;
//     unsigned int rate = SAMPLE_RATE;
//     int dir;
//     int pcm;

//     pcm = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
//     if (pcm < 0) {
//         std::cerr << "Unable to open PCM device: " << snd_strerror(pcm) << std::endl;
//         return 1;
//     }

//     snd_pcm_hw_params_alloca(&params);
//     snd_pcm_hw_params_any(handle, params);

//     snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
//     snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
//     snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
//     snd_pcm_hw_params_set_channels(handle, params, 1);

//     pcm = snd_pcm_hw_params(handle, params);
//     if (pcm < 0) {
//         std::cerr << "Unable to set HW parameters: " << snd_strerror(pcm) << std::endl;
//         return 1;
//     }

//     int samples = static_cast<int>(DURATION * SAMPLE_RATE);
//     int16_t *buffer = new int16_t[samples];

//     for (int i = 0; i < samples; ++i) {
//         buffer[i] = static_cast<int16_t>(32767 * sin(2 * M_PI * FREQUENCY * i / SAMPLE_RATE));
//     }

//     auto start = std::chrono::high_resolution_clock::now();;

//     if (snd_pcm_state(handle) != SND_PCM_STATE_RUNNING) {
//       pcm = snd_pcm_writei(handle, buffer, samples);
//       if (pcm < 0) {
//         std::cerr << "Playback error: " << snd_strerror(pcm) << std::endl;
//       }
//       start = std::chrono::high_resolution_clock::now();;
//     }

//     // std::thread t([handle](){
//     //   std::this_thread::sleep_for(1s);
//     //   snd_pcm_drop(handle);
//     // });
//     // t.detach();

//     bool loop = true;
//     while (loop) {
//       // snd_pcm_sframes_t delay;
//       // snd_pcm_delay(handle, &delay);
//       std::cout << snd_pcm_state_name(snd_pcm_state(handle)) << std::endl;

//       switch (snd_pcm_state(handle)) {
//         case SND_PCM_STATE_SETUP: {
//           // STOPPED
//           std::cout << "停止播放" << std::endl;
//           break;
//         }
//         case SND_PCM_STATE_XRUN: {
//           // Play finished
//           std::cout << "播放完成" << std::endl;
//           break;
//         }
//         case SND_PCM_STATE_RUNNING: {
//           // Playing
//           auto end = std::chrono::high_resolution_clock::now();
//           auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
//               end - start);
//           std::cout << "经过: " << dt.count() << "ms" << std::endl;
//           if (dt >= 5000ms) {
//             std::cout << "播放完成" << std::endl;
//             loop = false;
//           }
//           break;
//         }
//       }
//       std::this_thread::sleep_for(20ms);
//     }

//     snd_pcm_drain(handle);

//     delete[] buffer;
//     snd_pcm_close(handle);

//     return 0;
// }

// #include <alsa/asoundlib.h>

// static char *device = "default";            /* playback device */
// unsigned char buffer[16*1024];              /* some random data */

// int main(void)
// {
//   int err;
//   unsigned int i;
//   snd_pcm_t *handle;
//   snd_pcm_sframes_t frames;

//   for (i = 0; i < sizeof(buffer); i++)
//     buffer[i] = random() & 0xff;

//   if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
//     printf("Playback open error: %s\n", snd_strerror(err));
//     exit(EXIT_FAILURE);
//   }
//   if ((err = snd_pcm_set_params(handle,
//                                 SND_PCM_FORMAT_U8,
//                                 SND_PCM_ACCESS_RW_INTERLEAVED,
//                                 1,
//                                 48000,
//                                 1,
//                                 500000)) < 0) {   /* 0.5sec */
//     printf("Playback open error: %s\n", snd_strerror(err));
//     exit(EXIT_FAILURE);
//   }

//   for (i = 0; i < 16; i++) {
//     frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
//     if (frames < 0)
//       frames = snd_pcm_recover(handle, frames, 0);
//     if (frames < 0) {
//       printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
//       break;
//     }
//     if (frames > 0 && frames < (long)sizeof(buffer))
//       printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);
//   }

//   /* pass the remaining samples, otherwise they're dropped in close */
//   err = snd_pcm_drain(handle);
//   if (err < 0)
//     printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
//   snd_pcm_close(handle);
//   return 0;
// }
