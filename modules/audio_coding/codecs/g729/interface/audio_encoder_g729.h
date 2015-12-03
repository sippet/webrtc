// Copyright (c) 2015 The Sippet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_ENCODER_G729_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_ENCODER_G729_H_

#include <vector>

#include "webrtc/modules/audio_coding/codecs/audio_encoder.h"
#include "webrtc/modules/audio_coding/codecs/audio_encoder_mutable_impl.h"
#include "webrtc/modules/audio_coding/codecs/g729/interface/g729_interface.h"

namespace webrtc {

class AudioEncoderG729 : public AudioEncoder {
 public:
  struct Config {
   public:
    explicit Config();
    bool IsOk() const;
    int frame_size_ms;
    int payload_type;
    bool dtx_enabled;
  };

  explicit AudioEncoderG729(const Config& config);
  ~AudioEncoderG729() override;

  int SampleRateHz() const override;
  int NumChannels() const override;
  size_t MaxEncodedBytes() const override;
  int RtpTimestampRateHz() const override;
  int Num10MsFramesInNextPacket() const override;
  int Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;

  bool dtx_enabled() const { return dtx_enabled_; }

  EncodedInfo EncodeInternal(uint32_t rtp_timestamp,
                             const int16_t* audio,
                             size_t max_encoded_bytes,
                             uint8_t* encoded) override;

 private:
  const int payload_type_;
  const bool dtx_enabled_;
  const int num_10ms_frames_per_packet_;
  std::vector<int16_t> speech_buffer_;
  uint32_t first_timestamp_in_buffer_;
  G729EncInst* inst_;
};

struct CodecInst;

class AudioEncoderMutableG729
    : public AudioEncoderMutableImpl<AudioEncoderG729> {
 public:
  explicit AudioEncoderMutableG729(const CodecInst& codec_inst);

  // Set G729 DTX. Once enabled, G729 stops transmission, when it detects voice
  // being inactive.
  bool SetDtx(bool enable) override;

  bool dtx_enabled() const {
    CriticalSectionScoped cs(encoder_lock_.get());
    return encoder()->dtx_enabled();
  }
};

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_ENCODER_G729_H_
