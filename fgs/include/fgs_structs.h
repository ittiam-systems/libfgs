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

#ifndef _FGS_STRUCTS_H_
#define _FGS_STRUCTS_H_

/* Function Macro definitions */
#define CLIP3(min, max, x) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MSB16(x) ((x & 0xFFFF0000) >> 16)
#define LSB16(x) (x & 0x0000FFFF)
#define BIT0(x) (x & 0x1)
#define POS_30 (1 << 30)
#define POS_2 (1 << 2)

/* Macro definitions */
#define MAX_ALLOWED_MODEL_VALUES_MINUS1 2
#define MIN_LOG2SCALE_VALUE 2
#define MAX_LOG2SCALE_VALUE 7
#define FILM_GRAIN_MODEL_ID_VALUE 0
#define BLENDING_MODE_VALUE 0
#define MAX_STANDARD_DEVIATION 255
#define MIN_CUT_OFF_FREQUENCY 2
#define MAX_CUT_OFF_FREQUENCY 14
#define DEFAULT_HORZ_CUT_OFF_FREQUENCY 8
#define MAX_ALLOWED_COMP_MODEL_PAIRS 10

#define SCALE_DOWN_422 181 /* in Q-format of 8 : 1/sqrt(2) */
#define Q_FORMAT_SCALING 8
#define GRAIN_SCALE 6

#define MIN_WIDTH 128
#define MAX_WIDTH 7680
#define MIN_HEIGHT 128
#define MAX_HEIGHT 4320
#define MAX_THREADS 4

#define MIN_CHROMA_FORMAT_IDC 0
#define MAX_CHROMA_FORMAT_IDC 3
#define MIN_BIT_DEPTH 8
#define MAX_BIT_DEPTH 16
#define BIT_DEPTH_8 8
#define NUM_8x8_BLKS_16x16 4
#define BLK_8 8
#define BLK_8_shift 6 /* 2^6 is 64*/
#define BLK_16_shift 8
#define BLK_AREA_8x8 64
#define BLK_AREA_16x16 256
#define BLK_16 16
#define INTENSITY_INTERVAL_MATCH_FAIL -1
#define COLOUR_OFFSET_LUMA 0
#define COLOUR_OFFSET_CR 85
#define COLOUR_OFFSET_CB 170

#define NUM_CUT_OFF_FREQ 13
#define DATA_BASE_SIZE 64
#define BLOCK_IDX 2 /* 0 = blksize 8, 1 = blksize 16*/

/* Structure defining the grain synthesis */
typedef struct GrainSynthesisStruct_t
{
  int8_t  dataBase[BLOCK_IDX][NUM_CUT_OFF_FREQ][NUM_CUT_OFF_FREQ][DATA_BASE_SIZE][DATA_BASE_SIZE];
  int16_t intensityInterval[FGS_MAX_NUM_COMP][FGS_MAX_NUM_INTENSITIES];

} GrainSynthesisStruct;

/* Structure for passing FGS process arguments */
typedef struct fgsProcessArgs
{
  uint8_t                        numComp;
  uint32_t                      *fgsOffsets[FGS_MAX_NUM_COMP];
  void                          *decComp[FGS_MAX_NUM_COMP];
  uint32_t                       widthComp[FGS_MAX_NUM_COMP];
  uint32_t                       heightComp[FGS_MAX_NUM_COMP];
  uint32_t                       strideComp[FGS_MAX_NUM_COMP];
  FilmGrainCharacteristicsStruct fgcParams;
  GrainSynthesisStruct          *grainSynt;
  uint8_t                        grainSynthesisType;
  uint8_t                        bitDepth;
  void                          *scratchMem;

} fgsProcessArgs;

/* Structure for creating thread with relevant context parameters */
typedef struct threadCntxt
{
  fgsProcessArgs  fgsArgs;
  void           *thrdHandle;
  void           *mutexVar;
  void           *condVar;
  void           *semVar;
  volatile int8_t processComplete;
  int8_t          terminateCmd;
  uint32_t        processErrorCode;

} threadCntxt;

/* Structure for FGS handle */
typedef struct fgsHandle
{
  FGSInitParams        initParams;
  GrainSynthesisStruct grainSynt;
  int8_t               grainSynthesisType;
  threadCntxt          threadCntxArr[FGS_MAX_NUM_THREADS];
  void                *scratchMem;
  uint32_t            *offsetArr[FGS_MAX_NUM_COMP];
  int32_t              numMBX[FGS_MAX_NUM_COMP];
  int32_t              numMBY[FGS_MAX_NUM_COMP];
  /*frameInfo                      curFrame;*/
  FilmGrainCharacteristicsStruct curfgcParams;
} fgsHandle;

#endif /* _FGS_STRUCTS_H_ */
