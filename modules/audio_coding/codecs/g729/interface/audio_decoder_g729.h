// Copyright (c) 2015 The Sippet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_DECODER_G729_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_DECODER_G729_H_

#include "webrtc/modules/audio_coding/codecs/audio_decoder.h"
#include "webrtc/modules/audio_coding/codecs/g729/interface/g729_interface.h"

namespace webrtc {

class AudioDecoderG729 : public AudioDecoder {
 public:
  AudioDecoderG729();
  ~AudioDecoderG729() override;
  bool HasDecodePlc() const override;
  size_t DecodePlc(size_t num_frames, int16_t* decoded) override;
  void Reset() override;
  size_t Channels() const override;
  int PacketDuration(const uint8_t* encoded, size_t encoded_len) const override;

 protected:
  int DecodeInternal(const uint8_t* encoded,
                     size_t encoded_len,
                     int sample_rate_hz,
                     int16_t* decoded,
                     SpeechType* speech_type) override;

 private:
  G729DecInst* dec_state_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderG729);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_G729_INCLUDE_AUDIO_DECODER_G729_H_
