// Copyright (c) 2015 The Sippet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webrtc/modules/audio_coding/codecs/g729/interface/audio_encoder_g729.h"

#include "webrtc/base/checks.h"
#include "webrtc/base/safe_conversions.h"
#include "webrtc/common_types.h"
#include "webrtc/modules/audio_coding/codecs/g729/interface/g729_interface.h"

namespace webrtc {

namespace {

const int kSampleRateHz = 8000;
const int kNumChannels = 1;
const size_t kSamplesPer10msFrame = 80;
const int kBitsPerSecond = 8000;

AudioEncoderG729::Config CreateConfig(const CodecInst& codec_inst) {
  AudioEncoderG729::Config config;
  config.frame_size_ms = rtc::CheckedDivExact(codec_inst.pacsize, 8);
  config.num_channels = codec_inst.channels;
  config.payload_type = codec_inst.pltype;
  return config;
}

} // empty namespace

bool AudioEncoderG729::Config::IsOk() const {
  if (frame_size_ms % 10 != 0)
    return false;
  if (num_channels != 1)
    return false;
  return true;
}

AudioEncoderG729::AudioEncoderG729(const Config& config)
    : config_(config),
      num_10ms_frames_per_packet_(config.frame_size_ms / 10) {
  RTC_CHECK(config_.IsOk());
  RTC_CHECK_EQ(config.frame_size_ms % 10, 0)
      << "Frame size must be an integer multiple of 10 ms.";
  speech_buffer_.reserve(kSamplesPer10msFrame);
  RTC_CHECK_EQ(0, WebRtcG729_EncoderCreate(&inst_));
  Reset();
}

AudioEncoderG729::AudioEncoderG729(const CodecInst& codec_inst)
    : AudioEncoderG729(CreateConfig(codec_inst)) {}

AudioEncoderG729::~AudioEncoderG729() {
  RTC_CHECK_EQ(0, WebRtcG729_EncoderFree(inst_));
}

void AudioEncoderG729::Reset() {
  RTC_CHECK_EQ(0, WebRtcG729_EncoderInit(inst_, config_.dtx_enabled));
}

int AudioEncoderG729::SampleRateHz() const {
  return kSampleRateHz;
}

int AudioEncoderG729::RtpTimestampRateHz() const {
  // The RTP timestamp rate for G.729 is 8000 Hz.
  return kSampleRateHz;
}

int AudioEncoderG729::NumChannels() const {
  return kNumChannels;
}

size_t AudioEncoderG729::MaxEncodedBytes() const {
  return num_10ms_frames_per_packet_ * 10;
}

size_t AudioEncoderG729::Num10MsFramesInNextPacket() const {
  return num_10ms_frames_per_packet_;
}

size_t AudioEncoderG729::Max10MsFramesInAPacket() const {
  return num_10ms_frames_per_packet_;
}

int AudioEncoderG729::GetTargetBitrate() const {
  return kBitsPerSecond;
}

bool AudioEncoderG729::SetDtx(bool enable) {
  config_.dtx_enabled = enable;
  RTC_CHECK_EQ(0, WebRtcG729_EncoderInit(inst_, config_.dtx_enabled));
  return true;
}

bool AudioEncoderG729::dtx_enabled() const {
  return config_.dtx_enabled;
}

AudioEncoder::EncodedInfo AudioEncoderG729::EncodeInternal(
    uint32_t timestamp,
    const int16_t* audio,
    size_t max_encoded_bytes,
    uint8_t* encoded) {
  RTC_CHECK_GE(max_encoded_bytes, size_t(10));
  if (speech_buffer_.empty())
    first_timestamp_in_buffer_ = timestamp;
  speech_buffer_.insert(speech_buffer_.end(), audio,
                        audio + kSamplesPer10msFrame);
  if (speech_buffer_.size() < (static_cast<size_t>(num_10ms_frames_per_packet_) *
                               kSamplesPer10msFrame)) {
    return EncodedInfo();
  }
  RTC_CHECK_EQ(speech_buffer_.size(),
           static_cast<size_t>(num_10ms_frames_per_packet_) *
           kSamplesPer10msFrame);
  int16_t r = WebRtcG729_Encode(
      inst_, &speech_buffer_[0],
      num_10ms_frames_per_packet_ * kSamplesPer10msFrame,
      encoded);
  speech_buffer_.clear();
  if (r < 0)
    return EncodedInfo();
  EncodedInfo info;
  info.encoded_bytes = r;
  info.encoded_timestamp = first_timestamp_in_buffer_;
  info.payload_type = config_.payload_type;
  return info;
}

}  // namespace webrtc
