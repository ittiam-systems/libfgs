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

#ifndef _FGS_API_H_
#define _FGS_API_H_

/* Macro definitions */
#define FGS_MAX_NUM_COMP 3
#define FGS_MAX_NUM_INTENSITIES 256
#define FGS_MAX_NUM_MODEL_VALUES 6
#define FGS_MAX_NUM_THREADS 16

/* film grain initialization parameters */
typedef struct FGSInitParams
{
  /* max resolution that needs to be supported*/
  uint32_t maxWidth;
  /* height of the frame */
  uint32_t maxHeight;
  /* 8,10,12,14 and 16 bits */
  uint8_t maxbitDepth;
  /* Number of threads used to process a single Frame*/
  int32_t numThreads;
  /* Return error from Intializer*/
  uint32_t errorCode;

} FGSInitParams;

/* film grain characteristics SEI */
typedef struct FilmGrainCharacteristicsStruct_t
{
  /* To be 0 : to perform film grain synthesis */
  uint8_t filmGrainCharacteristicsCancelFlag;
  /* To be 0 : frequency filtering model */
  uint8_t filmGrainModelId;
  /* To be 0 : Decoded samples and grain to be in same color space */
  uint8_t separateColourDescriptionPresentFlag;
  uint8_t filmGrainBitDepthLumaMinus8;
  uint8_t filmGrainBitDepthChromaMinus8;
  uint8_t filmGrainFullRangeFlag;
  uint8_t filmGrainColourPrimaries;
  uint8_t filmGrainTransferCharacteristics;
  uint8_t filmGrainMatrixCoefficients;
  /* To be 0 : additive blending */
  uint8_t blendingModeId;
  /* To be in range of 2-7 : scale factor used in film grain simulation*/
  uint8_t log2ScaleFactor;
  /* To be either 8 or 16 : blockSize used in film grain simulation*/
  uint8_t blockSize;
  /* To disable chroma components or not */
  uint8_t disableFGSforChroma;
  /* Control for component model for each component*/
  uint8_t compModelPresentFlag[FGS_MAX_NUM_COMP];
  /* Number of intensity intervals in each component */
  uint8_t numIntensityIntervalsMinus1[FGS_MAX_NUM_COMP];
  /* Number of model values in each component */
  uint8_t numModelValuesMinus1[FGS_MAX_NUM_COMP];
  /* Lower bound of intensity interval */
  uint8_t intensityIntervalLowerBound[FGS_MAX_NUM_COMP][FGS_MAX_NUM_INTENSITIES];
  /* Upper bound of intensity interval */
  uint8_t intensityIntervalUpperBound[FGS_MAX_NUM_COMP][FGS_MAX_NUM_INTENSITIES];
  /* Component model values for each intensity interval */
  uint32_t compModelValue[FGS_MAX_NUM_COMP][FGS_MAX_NUM_INTENSITIES][FGS_MAX_NUM_MODEL_VALUES];
  /* To be 0:  Persistence of the film grain characteristics */
  uint32_t filmGrainCharacteristicsRepetitionPeriod;
} FilmGrainCharacteristicsStruct;

/* Structure holding the grain characteristics */
typedef struct GrainCharacteristicApi_t
{
  /* Input samples Y */
  void *inpBufY;
  /* Input samples U */
  void *inpBufU;
  /* Input samples V */
  void *inpBufV;
  /* film grain blended output Y */
  void *outBufY;
  /* film grain blended output U */
  void *outBufU;
  /* film grain blended output V */
  void *outBufV;
  /* width of the frame */
  uint32_t width;
  /* height of the frame */
  uint32_t height;
  /* 0 : monochrome; 1: 420; 2: 422; 3: 444 */
  uint8_t chromaFormat;
  /* Stride of input buffer Y */
  uint32_t inpStrideY;
  /* Stride of input buffer U */
  uint32_t inpStrideU;
  /* Stride of input buffer V */
  uint32_t inpStrideV;
  /* Stride of output buffer Y */
  uint32_t outStrideY;
  /* Stride of output buffer U */
  uint32_t outStrideU;
  /* Stride of output buffer V */
  uint32_t outStrideV;
  /* 8,10,12,14 and 16 bits */
  uint8_t bitDepth;
  /* decoded frame picture order count */
  uint32_t poc;
  /* 0: non-IDR pics */
  uint32_t idrPicId;
  /* Return error from Film grain synthesizer */
  uint32_t errorCode;
  /* Film grain chacracteristics */
  FilmGrainCharacteristicsStruct fgcParameters;
} GrainCharacteristicApi;

/* Structure for FGS handle */
/* List of functions defined as API */
void    *fgs_create(FGSInitParams *initParams);
void     fgs_delete(void *psFgsHandle);
uint32_t fgs_process(GrainCharacteristicApi fgsParamStruct, void *psFgsHandle);
uint32_t fgs_validate_input_params(GrainCharacteristicApi fgsParamStruct);

#endif /* _FGS_API_H_ */
