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
#include <stdint.h>
#include "fgs_api.h"
#include "fgs_structs.h"
#include "grain_simulation_modules.h"

/* Function to calculate block average for bit depths > 8 */
uint32_t block_average_hbd(uint16_t *decSampleBlk, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                           uint32_t xSize, uint8_t blkSz, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;
  int16_t  xSizePad = blkSz - xSize + 1;
  int16_t  ySizePad = blkSz - ySize + 1;
  for (k = 0; k < ySize - 1; k++)
  {
    for (l = 0; l < xSize - 1; l++)
    {
      blockAvg += *decSampleBlk;
      decSampleBlk++;
    }
    blockAvg += xSizePad * (*decSampleBlk);
    decSampleBlk++;
    decSampleBlk += widthComp - xSize;
  }
  {
    for (l = 0; l < xSize - 1; l++)
    {
      blockAvg += ySizePad * (*decSampleBlk);
      decSampleBlk++;
    }
    blockAvg += ySizePad * xSizePad * (*decSampleBlk);
    decSampleBlk++;
  }

  blockAvg     = blockAvg >> (shift_Size + bitDepth - BIT_DEPTH_8);
  *pNumSamples = blkSz * blkSz;

  return blockAvg;
}

uint32_t block_avg_8x8_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                             uint32_t xSize, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;

  for (k = 0; k < ySize; k++)
  {
    for (l = 0; l < xSize; l++)
    {
      blockAvg += *decSampleBlk8;
      decSampleBlk8++;
    }
    decSampleBlk8 += widthComp - xSize;
  }

  blockAvg = blockAvg >> (BLK_8_shift + (bitDepth - BIT_DEPTH_8));

  // If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed
  *pNumSamples = BLK_AREA_8x8;

  return blockAvg;
}

uint32_t block_avg_16x16_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                               uint32_t xSize, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;

  for (k = 0; k < ySize; k++)
  {
    for (l = 0; l < xSize; l++)
    {
      blockAvg += *decSampleBlk8;
      decSampleBlk8++;
    }
    decSampleBlk8 += widthComp - xSize;
  }

  blockAvg = blockAvg >> (BLK_16_shift + (bitDepth - BIT_DEPTH_8));

  // If BLK_16 is not used or changed BLK_AREA_16x16 has to be changed
  *pNumSamples = BLK_AREA_16x16;
  return blockAvg;
}

/* Function to calculate block average for bit depths = 8 */
/* Function to calculate block average for bit depths > 8 */
uint32_t block_average(uint8_t *decSampleBlk, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize, uint32_t xSize,
                       uint8_t blkSz, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;
  int16_t  xSizePad = blkSz - xSize + 1;
  int16_t  ySizePad = blkSz - ySize + 1;

  UNUSED(bitDepth);

  for (k = 0; k < ySize - 1; k++)
  {
    for (l = 0; l < xSize - 1; l++)
    {
      blockAvg += *decSampleBlk;
      decSampleBlk++;
    }
    blockAvg += xSizePad * (*decSampleBlk);
    decSampleBlk++;
    decSampleBlk += widthComp - xSize;
  }
  {
    for (l = 0; l < xSize - 1; l++)
    {
      blockAvg += ySizePad * (*decSampleBlk);
      decSampleBlk++;
    }
    blockAvg += ySizePad * xSizePad * (*decSampleBlk);
    decSampleBlk++;
  }

  blockAvg     = blockAvg >> shift_Size;
  *pNumSamples = blkSz * blkSz;

  return blockAvg;
}

uint32_t block_avg_8x8_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                            uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;

  UNUSED(bitDepth);

  for (k = 0; k < ySize; k++)
  {
    for (l = 0; l < xSize; l++)
    {
      blockAvg += *decSampleBlk8;
      decSampleBlk8++;
    }
    decSampleBlk8 += widthComp - xSize;
  }

  blockAvg = blockAvg >> shift_Size;

  // If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed
  *pNumSamples = BLK_AREA_8x8;
  return blockAvg;
}

uint32_t block_avg_16x16_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                              uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;

  UNUSED(bitDepth);

  for (k = 0; k < ySize; k++)
  {
    for (l = 0; l < xSize; l++)
    {
      blockAvg += *decSampleBlk8;
      decSampleBlk8++;
    }
    decSampleBlk8 += widthComp - xSize;
  }

  blockAvg = blockAvg >> shift_Size;

  // If BLK_16 is not used or changed BLK_AREA_16x16 has to be changed
  *pNumSamples = BLK_AREA_16x16;
  return blockAvg;
}

void blend_stripe_10bit(uint16_t *decSampleHbdOffsetY, int16_t *grainStripe, uint32_t widthComp,
                        uint32_t grainStripeWidth, uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;
  uint16_t maxRange;
  maxRange = (1 << bitDepth) - 1;

  int32_t  grainSample;
  uint16_t decodeSampleHbd;
  for (l = 0; l < blockHeight; l++) /* y direction */
  {
    for (k = 0; k < widthComp; k++) /* x direction */
    {
      decodeSampleHbd      = *decSampleHbdOffsetY;
      grainSample          = *grainStripe;
      grainSample          = CLIP3(0, maxRange, grainSample + decodeSampleHbd);
      *decSampleHbdOffsetY = (uint16_t) grainSample;
      decSampleHbdOffsetY++;
      grainStripe++;
    }
    grainStripe += grainStripeWidth - widthComp;
  }
}

void blend_stripe_8bit(uint8_t *decSampleOffsetY, int8_t *grainStripe, uint32_t widthComp, uint32_t grainStripeWidth,
                       uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;
  int32_t  grainSample;
  uint8_t  decodeSample;
  uint16_t maxRange;
  maxRange = (1 << bitDepth) - 1;
  for (l = 0; l < blockHeight; l++) /* y direction */
  {
    for (k = 0; k < widthComp; k++) /* x direction */
    {
      decodeSample      = *decSampleOffsetY;
      grainSample       = *grainStripe;
      grainSample       = CLIP3(0, maxRange, (grainSample + decodeSample));
      *decSampleOffsetY = (uint8_t) grainSample;
      decSampleOffsetY++;
      grainStripe++;
    }
    grainStripe += grainStripeWidth - widthComp;
  }
}

void deblock_grain_stripe(int8_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  uint32_t pos8;
  int8_t   vertCtr;

  int8_t *grainStripe_const = grainStripe;
  for (vertCtr = 0; vertCtr < blkH; vertCtr++)
  {
    for (pos8 = 0; pos8 < (widthComp - blkW); pos8 += blkW)
    {
      left1                     = *(grainStripe + blkW - 2);
      left0                     = *(grainStripe + blkW - 1);
      right0                    = *(grainStripe + blkW + 0);
      right1                    = *(grainStripe + blkW + 1);
      *(grainStripe + blkW + 0) = (left0 + (right0 << 1) + right1) >> 2;
      *(grainStripe + blkW - 1) = (left1 + (left0 << 1) + right0) >> 2;
      grainStripe += blkW;
    }
    grainStripe_const += strideComp;
    grainStripe = grainStripe_const;
  }
}

void deblock_grain_stripe_hbd(int16_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  uint32_t pos8;
  int8_t   vertCtr;

  int16_t *grainStripe_const = grainStripe;
  for (vertCtr = 0; vertCtr < blkH; vertCtr++)
  {
    for (pos8 = 0; pos8 < (widthComp - blkW); pos8 += blkW)
    {
      left1                     = *(grainStripe + blkW - 2);
      left0                     = *(grainStripe + blkW - 1);
      right0                    = *(grainStripe + blkW + 0);
      right1                    = *(grainStripe + blkW + 1);
      *(grainStripe + blkW + 0) = (left0 + (right0 << 1) + right1) >> 2;
      *(grainStripe + blkW - 1) = (left1 + (left0 << 1) + right0) >> 2;
      grainStripe += blkW;
    }
    grainStripe_const += strideComp;
    grainStripe = grainStripe_const;
  }
}

void simulate_grain_blk16x16_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                  uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                  uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t k, l;
  int8_t  *database_h_v = &grain_synt->dataBase[1][h][v][lOffset][kOffset];
  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++) /* y direction */
  {
    for (k = 0; k < xSize; k++) /* x direction */
    {
      *grainStripe = ((scaleFactor * (*database_h_v)) >> (log2ScaleFactor + GRAIN_SCALE));
      grainStripe++;
      database_h_v++;
    }
    grainStripe += width - xSize;
    database_h_v += DATA_BASE_SIZE - xSize;
  }
}

void simulate_grain_blk8x8_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t k, l;
  int8_t  *database_h_v = &grain_synt->dataBase[0][h][v][lOffset][kOffset];
  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_8; l++) /* y direction */
  {
    for (k = 0; k < xSize; k++) /* x direction */
    {
      *grainStripe = ((scaleFactor * (*database_h_v)) >> (log2ScaleFactor + GRAIN_SCALE));
      grainStripe++;
      database_h_v++;
    }
    grainStripe += width - xSize;
    database_h_v += DATA_BASE_SIZE - xSize;
  }
}

void simulate_grain_blk8x8_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                 uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                 uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t k, l;
  int8_t  *database_h_v = &grain_synt->dataBase[0][h][v][lOffset][kOffset];
  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_8; l++) /* y direction */
  {
    for (k = 0; k < xSize; k++) /* x direction */
    {
      *grainStripe = ((scaleFactor * (*database_h_v)) >> (log2ScaleFactor + GRAIN_SCALE));
      grainStripe++;
      database_h_v++;
    }
    grainStripe += width - xSize;
    database_h_v += DATA_BASE_SIZE - xSize;
  }
}

void simulate_grain_blk16x16_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8,
                                   GrainSynthesisStruct *grain_synt, uint32_t width, uint8_t log2ScaleFactor,
                                   int32_t scaleFactor, uint32_t kOffset, uint32_t lOffset, uint32_t h, uint32_t v,
                                   uint32_t xSize)
{
  uint32_t k, l;
  int8_t  *database_h_v = &grain_synt->dataBase[1][h][v][lOffset][kOffset];
  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++) /* y direction */
  {
    for (k = 0; k < xSize; k++) /* x direction */
    {
      *grainStripe = (int16_t) (((int32_t) scaleFactor * (*database_h_v)) >> (log2ScaleFactor + GRAIN_SCALE));
      grainStripe++;
      database_h_v++;
    }
    grainStripe += width - xSize;
    database_h_v += DATA_BASE_SIZE - xSize;
  }
}
