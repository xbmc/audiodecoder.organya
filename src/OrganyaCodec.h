/*
 *      Copyright (C) 2019 Team Kodi
 *      Copyright (C) 2014 Arne Morten Kvarving
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/Filesystem.h>
#include <kodi/General.h>

#include "decoder.h"
#include "organya.h"

class ATTRIBUTE_HIDDEN COrganyaCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  COrganyaCodec(KODI_HANDLE instance) : CInstanceAudioDecoder(instance) {};
  ~COrganyaCodec() override;
  bool Init(const std::string& filename, unsigned int filecache, int& channels, int& samplerate,
            int& bitspersample, int64_t& totaltime,
            int& bitrate, AEDataFormat& format,
            std::vector<AEChannel>& channellist) override;
  int ReadPCM(uint8_t* buffer, int size, int& actualsize) override;
  int64_t Seek(int64_t time) override;
  bool ReadTag(const std::string& file, std::string& title, std::string& artist, int& length) override { return true; }

private:
  inline int mul_div(int number, int numerator, int denominator)
  {
    long long ret = number;
    ret *= numerator;
    ret /= denominator;
    return (int) ret;
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
