# Copyright (c) 2015 The Sippet project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'targets': [
    {
      'target_name': 'webrtc_g729',
      'type': 'static_library',
      'conditions': [
        ['build_g729==1', {
          'dependencies': [
            '<(g729_dir)/g729.gyp:g729'
          ],
          'export_dependent_settings': [
            '<(g729_dir)/g729.gyp:g729',
          ],
          'direct_dependent_settings': {
            'include_dirs': [  # need by Neteq audio classifier.
              '<(g729_dir)/src',
            ],
          },
        }],
      ],
      'dependencies': [
        'audio_encoder_interface',
      ],
      'include_dirs': [
        '<(webrtc_root)',
        '<(g729_dir)/src',
      ],
      'sources': [
        'audio_encoder_g729.cc',
        'interface/audio_encoder_g729.h',
        'interface/g729_interface.h',
        'g729_inst.h',
        'g729_interface.c',
      ],
    },
  ],
  'conditions': [
    ['include_tests==1', {
      'targets': [
        {
          'target_name': 'webrtc_g729_unittest',
          'type': 'executable',
          'dependencies': [
            'webrtc_g729',
            'neteq_unittest_tools',
            '<(webrtc_root)/common_audio/common_audio.gyp:common_audio',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(DEPTH)/testing/gtest.gyp:gtest',
          ],
          'sources': [
            'g729_unittest.cc',
          ],
        }, # webrtc_g729_unittest
      ],
    }],
  ],
}
