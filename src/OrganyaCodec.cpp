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

#include "libXBMC_addon.h"
#include "RingBuffer.h"

#include "decoder.h"
#include "organya.h"

#include <iostream>

extern "C" {
#include <stdio.h>
#include <stdint.h>

#include "kodi_audiodec_dll.h"

ADDON::CHelper_libXBMC_addon *XBMC           = NULL;

ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!XBMC)
    XBMC = new ADDON::CHelper_libXBMC_addon;

  if (!XBMC->RegisterMe(hdl))
  {
    delete XBMC, XBMC=NULL;
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  return ADDON_STATUS_OK;
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Destroy()
{
  XBMC=NULL;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  return ADDON_STATUS_OK;
}

struct OrgContext
{
  org_decoder_t* tune;
  int64_t length;
  int64_t pos;
  CRingBuffer sample_buffer;
};

void* Init(const char* strFile, unsigned int filecache, int* channels,
           int* samplerate, int* bitspersample, int64_t* totaltime,
           int* bitrate, AEDataFormat* format, const AEChannel** channelinfo)
{
  OrgContext* result = new OrgContext;
  result->sample_buffer.Create(4096);
  void* file = XBMC->OpenFile(strFile, 0);
  char temp[1024];
  XBMC->GetSetting("__addonpath__",temp);
  strcat(temp,"/resources/samples");
  result->tune = org_decoder_create(file, temp, 1);
  result->tune->state.sample_rate = 48000;
  *totaltime = org_decoder_get_total_samples(result->tune)*1000/48000;
  result->length = *totaltime/1000*48000*4;
  static enum AEChannel map[3] = {
    AE_CH_FL, AE_CH_FR, AE_CH_NULL
  };
  *format = AE_FMT_S16NE;
  *channelinfo = map;
  *channels = 2;
  *bitspersample = 16;
  *bitrate = 0.0;
  *samplerate = 48000;

  XBMC->CloseFile(file);
  Seek(result, 0);

  return result;
}

int ReadPCM(void* context, uint8_t* pBuffer, int size, int *actualsize)
{
  OrgContext* org = (OrgContext*)context;
  if (org->pos >= org->length*48000*4/1000)
    return 1;

  if (org->sample_buffer.getMaxReadSize() == 0) {
    int16_t temp[1024*2]; 
    int64_t written=1024;
    written = org_decode_samples(org->tune, temp, written);
    if (written == 0)
      return 1;
    org->sample_buffer.WriteData((const char*)temp, written*4);
  }

  int tocopy = std::min(size, (int)org->sample_buffer.getMaxReadSize());
  org->sample_buffer.ReadData((char*)pBuffer, tocopy);
  org->pos += tocopy;
  *actualsize = tocopy;
  return 0;
}

int64_t Seek(void* context, int64_t time)
{
  OrgContext* org = (OrgContext*)context;

  org->pos = time*48000*4/1000;

  org_decoder_seek_sample(org->tune, org->pos/4);
  
  return time;
}

bool DeInit(void* context)
{
  OrgContext* org = (OrgContext*)context;
  org_decoder_destroy(org->tune);
  delete org;
  return true;
}

bool ReadTag(const char* strFile, char* title, char* artist, int* length)
{
  return true;
}

int TrackCount(const char* strFile)
{
  return 1;
}
}
