# Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'targets': [
    {
      'target_name': 'G729',
      'type': 'static_library',
      'dependencies': [
        '<(webrtc_root)/common_audio/common_audio.gyp:common_audio',
        'audio_encoder_interface',
      ],
      'include_dirs': [
        './include',
        './source',
        '<(webrtc_root)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          './source',
          '<(webrtc_root)',
        ],
      },
      'sources': [
        'include/g729_interface.h',
        'audio_encoder_g729.cc',
        'g729_inst.h',
        'g729_interface.c',
        'source/acelp_ca.c',
        'source/basic_op.h',
        'source/bits.c',
        'source/calcexc.c',
        'source/cod_ld8a.c',
        'source/cor_func.c',
        'source/de_acelp.c',
        'source/dec_gain.c',
        'source/dec_lag3.c',
        'source/dec_ld8a.c',
        'source/dec_sid.c',
        'source/dspfunc.c',
        'source/dtx.c',
        'source/dtx.h',
        'source/filter.c',
        'source/gainpred.c',
        'source/ld8a.h',
        'source/lpc.c',
        'source/lpcfunc.c',
        'source/lspdec.c',
        'source/lspgetq.c',
        'source/octet.h',
        'source/oper_32b.c',
        'source/oper_32b.h',
        'source/pitch_a.c',
        'source/postfilt.c',
        'source/post_pro.c',
        'source/p_parity.c',
        'source/pred_lt3.c',
        'source/pre_proc.c',
        'source/qsidgain.c',
        'source/qsidlsf.c',
        'source/qua_gain.c',
        'source/qua_lsp.c',
        'source/sid.h',
        'source/tab_dtx.c',
        'source/tab_dtx.h',
        'source/tab_ld8a.c',
        'source/tab_ld8a.h',
        'source/taming.c',
        'source/vad.c',
        'source/vad.h',
        'source/util.c',
      ],
    },  # target G729
  ],
  'conditions': [
    ['include_tests==1', {
      'targets': [
        {
          'target_name': 'G729_unittest',
          'type': 'executable',
          'dependencies': [
            'G729',
            'neteq_unittest_tools',
            '<(webrtc_root)/common_audio/common_audio.gyp:common_audio',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(DEPTH)/testing/gtest.gyp:gtest',
          ],
          'sources': [
            'g729_unittest.cc',
          ],
        }, # G729_unittest
      ],
    }],
  ],
}
