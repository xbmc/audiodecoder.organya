/*
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

#include "RingBuffer.h"

#include "decoder.h"
#include "organya.h"

#include <iostream>

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/Filesystem.h>


class COrganyaCodec : public kodi::addon::CInstanceAudioDecoder
  , public kodi::addon::CAddonBase
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
  org_decoder_t* tune;
  int64_t length;
  int64_t pos;
  CRingBuffer sample_buffer;
};


bool COrganyaCodec::Init(const std::string& strFile, unsigned int filecache, int& channels,
                         int& samplerate, int& bitspersample, int64_t& totaltime,
                         int& bitrate, AEDataFormat& format, std::vector<AEChannel>& channelinfo)
{
  sample_buffer.Create(4096);
  kodi::vfs::CFile file;
  file.OpenFile(strFile);
  if (!file.OpenFile(strFile, 0))
    return false;
  std::string temp = kodi::GetSettingString("__addonpath__") + "/resources/samples";
  tune = org_decoder_create(&file, temp.c_str(), 1);
  tune->state.sample_rate = 48000;
  totaltime = org_decoder_get_total_samples(tune)*1000/48000;
  length = totaltime/1000*48000*4;
  format = AE_FMT_S16NE;
  channelinfo = { AE_CH_FL, AE_CH_FR, AE_CH_NULL };
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = 48000;

  file.Close();
  Seek(0);

  return true;
}

int COrganyaCodec::ReadPCM(uint8_t* pBuffer, int size, int &actualsize)
{
  if (pos >= length*48000*4/1000)
    return 1;

  if (sample_buffer.getMaxReadSize() == 0) {
    int16_t temp[1024*2]; 
    int64_t written=1024;
    written = org_decode_samples(tune, temp, written);
    if (written == 0)
      return 1;
    sample_buffer.WriteData((const char*)temp, written*4);
  }

  int tocopy = std::min(size, (int)sample_buffer.getMaxReadSize());
  sample_buffer.ReadData((char*)pBuffer, tocopy);
  pos += tocopy;
  actualsize = tocopy;
  return 0;
}

int64_t COrganyaCodec::Seek(int64_t time)
{
  pos = time*48000*4/1000;

  org_decoder_seek_sample(tune, pos/4);
  
  return time;
}

COrganyaCodec::~COrganyaCodec()
{
  org_decoder_destroy(tune);
}

class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() { }
  virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance) override
  {
    addonInstance = new COrganyaCodec(instance);
    return ADDON_STATUS_OK;
  }

  virtual ~CMyAddon() {}
};

ADDONCREATOR(CMyAddon); // Don't touch this!
