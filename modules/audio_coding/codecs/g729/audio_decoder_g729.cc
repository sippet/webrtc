// Copyright (c) 2015 The Sippet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webrtc/modules/audio_coding/codecs/g729/interface/audio_decoder_g729.h"

#include "webrtc/base/checks.h"

namespace webrtc {

AudioDecoderG729::AudioDecoderG729() {
  WebRtcG729_DecoderCreate(&dec_state_);
}

AudioDecoderG729::~AudioDecoderG729() {
  WebRtcG729_DecoderFree(dec_state_);
}

size_t AudioDecoderG729::Channels() const {
  return 1;
}

bool AudioDecoderG729::HasDecodePlc() const {
  return true;
}

int AudioDecoderG729::DecodeInternal(const uint8_t* encoded,
                                     size_t encoded_len,
                                     int sample_rate_hz,
                                     int16_t* decoded,
                                     SpeechType* speech_type) {
  RTC_DCHECK_EQ(sample_rate_hz, 8000);
  int16_t temp_type = 1;  // Default is speech.
  int16_t ret = WebRtcG729_Decode(dec_state_, encoded,
                                  static_cast<int16_t>(encoded_len), decoded,
                                  &temp_type);
  *speech_type = ConvertSpeechType(temp_type);
  return ret;
}

size_t AudioDecoderG729::DecodePlc(size_t num_frames, int16_t* decoded) {
  return WebRtcG729_DecodePlc(dec_state_, decoded, num_frames);
}

void AudioDecoderG729::Reset() {
  WebRtcG729_DecoderInit(dec_state_);
}

int AudioDecoderG729::PacketDuration(const uint8_t* encoded,
                                     size_t encoded_len) const {
  // One or more G.729A frames, followed by one or more G.729B frames
  size_t speech_frms = encoded_len / 10;
  size_t sid_frms = (encoded_len % 10) / 2;
  return (speech_frms + sid_frms) * 80;
}

}  // namespace webrtc
