/*
 *  Copyright (C) 2014-2021 Arne Morten Kvarving
 *  Copyright (C) 2019-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "decoder.h"
#include "organya.h"

#include <algorithm>
#include <iostream>
#include <kodi/Filesystem.h>
#include <kodi/General.h>
#include <kodi/addon-instance/AudioDecoder.h>
#include <vector>

class ATTR_DLL_LOCAL COrganyaCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  COrganyaCodec(const kodi::addon::IInstanceInfo& instance) : CInstanceAudioDecoder(instance) {}
  ~COrganyaCodec() override;
  bool Init(const std::string& filename,
            unsigned int filecache,
            int& channels,
            int& samplerate,
            int& bitspersample,
            int64_t& totaltime,
            int& bitrate,
            AudioEngineDataFormat& format,
            std::vector<AudioEngineChannel>& channellist) override;
  int ReadPCM(uint8_t* buffer, size_t size, size_t& actualsize) override;
  int64_t Seek(int64_t time) override;
  bool ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag) override;

private:
  inline int mul_div(int number, int numerator, int denominator)
  {
    long long ret = number;
    ret *= numerator;
    ret /= denominator;
    return (int)ret;
  }

  int m_cfgFadeTime = 1000;
  int m_cfgSampleRate = 48000;
  int m_cfgLoopCnt = 1;
  bool m_cfgLoopIndefinitely = false;

  kodi::vfs::CFile m_file;
  org_decoder_t* m_tune = nullptr;
  int64_t m_length = 0;
  int64_t m_samplesDecoded = 0;
  int m_fadeTime = 0;
};
