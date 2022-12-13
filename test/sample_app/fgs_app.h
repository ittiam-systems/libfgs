/* 	Copyright (c) [2020]-[2023] Ittiam Systems Pvt. Ltd.
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted (subject to the limitations in the
   disclaimer below) provided that the following conditions are met:
   •	Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   •	Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   •	None of the names of Ittiam Systems Pvt. Ltd., its affiliates,
   investors, business partners, nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
   BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   This Software is an implementation of the AVC/H.264
   standard by Ittiam Systems Pvt. Ltd. (“Ittiam”).
   Additional patent licenses may be required for this Software,
   including, but not limited to, a license from MPEG LA’s AVC/H.264
   licensing program (see https://www.mpegla.com/programs/avc-h-264/).

   NOTWITHSTANDING ANYTHING TO THE CONTRARY, THIS DOES NOT GRANT ANY
   EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS OF ANY AFFILIATE
   (TO THE EXTENT NOT IN THE LEGAL ENTITY), INVESTOR, OR OTHER
   BUSINESS PARTNER OF ITTIAM. You may only use this software or
   modifications thereto for purposes that are authorized by
   appropriate patent licenses. You should seek legal advice based
   upon your implementation details.

---------------------------------------------------------------
*/

#ifndef __FGS_APP_H__
#define __FGS_APP_H__

#define MAX_QUEUE_NAME_SIZE 64
#define MAX_STRING_LENGTH 4096
#define MAX_LOG_LENGTH MAX_STRING_LENGTH

#define UNIT_APP_MAX_LINE_LEN 1024
#define UNIT_APP_MAX_NUM_PARAM 30
#define UNIT_APP_MAX_PARAM_LEN 1024
#define MAX_FILE_NAME_STR_SIZE 1024

/*---------------------------------------------------------------------------*/
/*                           Structure Definitions                           */
/*---------------------------------------------------------------------------*/

/* Film Grain Synthesizer application context */
typedef struct fgs_app_params_t
{
  /* Frame num from FGC params are applicable*/
  uint32_t startFrameNum;
  /* Frame num till FGC params are applicable*/
  uint32_t endFrameNum;
  /* Total number of frames to be processed */
  uint32_t numFrames;
  /* Configuration file */
  char initParamsCfgFile[MAX_LOG_LENGTH];
  /* Input YUV file name */
  char inputFileName[MAX_LOG_LENGTH];
  /* Output YUV file name */
  char outputFile[MAX_LOG_LENGTH];
  /* Input width */
  uint32_t frameWidth;
  /* Input height */
  uint32_t frameHeight;
  /* Input bit depth */
  uint8_t bitDepth;
  /* Input chroma format */
  uint8_t chromaFmt;
  /* Number of threads*/
  int32_t numThreads;
  /* Number of FGS profiles*/
  int32_t numProfiles;
} fgs_app_params;

#endif /* __FGS_APP_H__ */
