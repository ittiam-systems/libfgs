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

#include <emmintrin.h>
#include <immintrin.h>
#include <stdint.h>
#include "fgs_api.h"
#include "grain_simulation_modules.h"

/* Function to calculate block average for bit depths > 8 */
uint32_t block_average_hbd(uint16_t *decSampleBlk, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                           uint32_t xSize, uint8_t blkSz, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k, l;
  int32_t  xSizePad = blkSz - xSize + 1;
  int32_t  ySizePad = blkSz - ySize + 1;
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
  uint8_t  k;

  __m128i __m128i_sumRegister = _mm_loadu_si128((__m128i *) decSampleBlk8);
  __m128i __m128i_zeros       = _mm_set1_epi16(0);
  __m128i_sumRegister         = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);
  __m128i_sumRegister         = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);

  UNUSED(xSize);

  decSampleBlk8 += widthComp;

  for (k = 1; k < ySize; k++)
  {
    __m128i __m128i_decData = _mm_loadu_si128((__m128i *) decSampleBlk8);
    decSampleBlk8 += widthComp;
    __m128i_sumRegister = _mm_add_epi16(__m128i_sumRegister, __m128i_decData);
  }

  /* __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros); */
  blockAvg =
    ((uint32_t) _mm_extract_epi16(__m128i_sumRegister, 0)) + ((uint32_t) _mm_extract_epi16(__m128i_sumRegister, 1));

  blockAvg = blockAvg >> (BLK_8_shift + (bitDepth - BIT_DEPTH_8));

  /* If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed */
  *pNumSamples = BLK_AREA_8x8;

  return blockAvg;
}

uint32_t block_avg_16x16_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                               uint32_t xSize, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  __m128i __m128i_decData1;
  __m128i __m128i_decData2;
  __m128i __m128i_sumRegister;
  __m128i __m128i_sumRegister1 = _mm_loadu_si128((__m128i *) decSampleBlk8);
  __m128i __m128i_sumRegister2 = _mm_loadu_si128((__m128i *) (decSampleBlk8 + BLK_8));
  __m128i __m128i_zeros        = _mm_set1_epi16(0);
  __m128i_sumRegister1         = _mm_hadds_epi16(__m128i_sumRegister1, __m128i_zeros);
  __m128i_sumRegister2         = _mm_hadds_epi16(__m128i_sumRegister2, __m128i_zeros);
  __m128i_sumRegister1         = _mm_cvtepi16_epi32(__m128i_sumRegister1);
  __m128i_sumRegister2         = _mm_cvtepi16_epi32(__m128i_sumRegister2);
  __m128i_sumRegister          = _mm_hadd_epi32(__m128i_sumRegister1, __m128i_sumRegister2);
  __m128i_zeros                = _mm_set1_epi32(0);
  __m128i_sumRegister          = _mm_hadd_epi32(__m128i_sumRegister, __m128i_zeros);
  __m128i_sumRegister          = _mm_hadd_epi32(__m128i_sumRegister, __m128i_zeros);

  UNUSED(xSize);

  decSampleBlk8 += widthComp;

  for (k = 1; k < ySize; k++)
  {
    __m128i_decData1 = _mm_loadu_si128((__m128i *) decSampleBlk8);
    __m128i_decData2 = _mm_loadu_si128((__m128i *) (decSampleBlk8 + BLK_8));
    decSampleBlk8 += widthComp;
    __m128i_sumRegister1 = _mm_add_epi16(__m128i_sumRegister1, __m128i_decData1);
    __m128i_sumRegister2 = _mm_add_epi16(__m128i_sumRegister2, __m128i_decData2);
  }

  blockAvg = (uint32_t) _mm_extract_epi32(__m128i_sumRegister, 0);

  blockAvg = blockAvg >> (BLK_16_shift + (bitDepth - BIT_DEPTH_8));

  /* If BLK_16 is not used or changed BLK_AREA_16x16 has to be changed */
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
  int32_t  xSizePad = blkSz - xSize + 1;
  int32_t  ySizePad = blkSz - ySize + 1;

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
  uint8_t  k;

  __m128i __m128i_zeros;
  __m128i __m128i_decData     = _mm_loadu_si128((__m128i *) decSampleBlk8);
  __m128i __m128i_sumRegister = _mm_cvtepu8_epi16(__m128i_decData);

  UNUSED(xSize);
  UNUSED(bitDepth);

  decSampleBlk8 += widthComp;

  for (k = 1; k < ySize; k++)
  {
    __m128i_decData = _mm_loadu_si128((__m128i *) decSampleBlk8);
    decSampleBlk8 += widthComp;
    __m128i_decData     = _mm_cvtepu8_epi16(__m128i_decData);
    __m128i_sumRegister = _mm_add_epi16(__m128i_sumRegister, __m128i_decData);
  }
  __m128i_zeros       = _mm_set1_epi16(0);
  __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);
  __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);
  __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);
  blockAvg            = ((uint32_t) _mm_extract_epi16(__m128i_sumRegister, 0));

  blockAvg = blockAvg >> shift_Size;

  /* If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed */
  *pNumSamples = BLK_AREA_8x8;
  return blockAvg;
}

uint32_t block_avg_16x16_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                              uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  __m128i __m128i_zeros;
  __m128i __m128i_decData1    = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) decSampleBlk8));
  __m128i __m128i_decData2    = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) (decSampleBlk8 + BLK_8)));
  __m128i __m128i_sumRegister = _mm_add_epi16(__m128i_decData1, __m128i_decData2);

  UNUSED(bitDepth);
  UNUSED(xSize);

  decSampleBlk8 += widthComp;

  for (k = 1; k < ySize; k++)
  {
    __m128i_decData1    = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) decSampleBlk8));
    __m128i_decData2    = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) (decSampleBlk8 + BLK_8)));
    __m128i_sumRegister = _mm_add_epi16(__m128i_sumRegister, __m128i_decData1);
    __m128i_sumRegister = _mm_add_epi16(__m128i_sumRegister, __m128i_decData2);
    decSampleBlk8 += widthComp;
  }
  __m128i_zeros       = _mm_set1_epi16(0);
  __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);
  __m128i_sumRegister = _mm_hadds_epi16(__m128i_sumRegister, __m128i_zeros);

  blockAvg =
    ((uint32_t) _mm_extract_epi16(__m128i_sumRegister, 0)) + ((uint32_t) _mm_extract_epi16(__m128i_sumRegister, 1));

  blockAvg = blockAvg >> shift_Size;

  /* If BLK_16 is not used or changed BLK_AREA_16x16 has to be changed */
  *pNumSamples = BLK_AREA_16x16;
  return blockAvg;
}

void blend_stripe_10bit(uint16_t *decSampleHbdOffsetY, uint16_t *outSampleHbdOffsetY, int16_t *grainStripe,
                        uint32_t widthComp, uint32_t decSampleStride, uint32_t outSampleStride,
                        uint32_t grainStripeWidth, uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;
  uint16_t maxRange;
  __m128i  __m128i_maxrange;
  __m128i  __m128i_minZero;
  maxRange = (1 << bitDepth) - 1;

  __m128i_maxrange = _mm_set1_epi16(maxRange);
  __m128i_minZero  = _mm_set1_epi16(0);
  if ((widthComp & 0x7) == 0)
  {
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp; k += BLK_8)
      {
        __m128i __m128i_decData = _mm_loadu_si128((__m128i *) decSampleHbdOffsetY);
        __m128i __m128i_grainSt = _mm_loadu_si128((__m128i *) grainStripe);
        __m128i __m128i_sum     = _mm_add_epi16(__m128i_decData, __m128i_grainSt);
        __m128i_sum             = _mm_min_epi16(__m128i_sum, __m128i_maxrange);
        __m128i_sum             = _mm_max_epi16(__m128i_sum, __m128i_minZero);
        _mm_store_si128((__m128i *) outSampleHbdOffsetY, __m128i_sum);
        decSampleHbdOffsetY += BLK_8;
		outSampleHbdOffsetY += BLK_8;
        grainStripe += BLK_8;
      }
	  decSampleHbdOffsetY += decSampleStride - widthComp;
	  outSampleHbdOffsetY += outSampleStride - widthComp;
      grainStripe         += grainStripeWidth - widthComp;
    }
  }
  else
  {
    int32_t  grainSample;
    uint16_t decodeSample;
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp - BLK_8; k += BLK_8)
      {
        __m128i __m128i_decData = _mm_loadu_si128((__m128i *) decSampleHbdOffsetY);
        __m128i __m128i_grainSt = _mm_loadu_si128((__m128i *) grainStripe);
        __m128i __m128i_sum     = _mm_add_epi16(__m128i_decData, __m128i_grainSt);
        __m128i_sum             = _mm_min_epi16(__m128i_sum, __m128i_maxrange);
        __m128i_sum             = _mm_max_epi16(__m128i_sum, __m128i_minZero);
        _mm_storeu_si128((__m128i *) outSampleHbdOffsetY, __m128i_sum);
        decSampleHbdOffsetY += BLK_8;
		outSampleHbdOffsetY += BLK_8;
        grainStripe += BLK_8;
      }
      for (; k < widthComp; k++)
      {
        decodeSample         = *decSampleHbdOffsetY;
        grainSample          = *grainStripe;
        grainSample          = CLIP3(0, maxRange, (grainSample + decodeSample));
        *outSampleHbdOffsetY = (uint16_t) grainSample;
        decSampleHbdOffsetY++;
		outSampleHbdOffsetY++;
        grainStripe++;
      }
	  decSampleHbdOffsetY += decSampleStride - widthComp;
	  outSampleHbdOffsetY += outSampleStride - widthComp;
      grainStripe         += grainStripeWidth - widthComp;
    }
  }
}

void blend_stripe_8bit(uint8_t *decSampleOffsetY, uint8_t *outSampleOffsetY, int8_t *grainStripe, 
                       uint32_t widthComp, uint32_t decSampleStride, uint32_t outSampleStride,
					   uint32_t grainStripeWidth, uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;

  if ((widthComp & 0xF) == 0)
  {
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp; k += BLK_16)
      {
        __m128i __m128i_decData = _mm_loadu_si128((__m128i *) decSampleOffsetY);
        __m128i __m128i_grainSt = _mm_loadu_si128((__m128i *) grainStripe);

        __m128i __m128i_decData1 = _mm_cvtepu8_epi16(__m128i_decData);
        __m128i __m128i_decData2 = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) (decSampleOffsetY + BLK_8)));

        __m128i __m128i_grainSt1 = _mm_cvtepi8_epi16(__m128i_grainSt);
        __m128i __m128i_grainSt2 = _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *) (grainStripe + BLK_8)));

        __m128i __m128i_sum1 = _mm_add_epi16(__m128i_decData1, __m128i_grainSt1);
        __m128i __m128i_sum2 = _mm_add_epi16(__m128i_decData2, __m128i_grainSt2);
        __m128i __m128i_sum  = _mm_packus_epi16(__m128i_sum1, __m128i_sum2);
        _mm_storeu_si128((__m128i *) outSampleOffsetY, __m128i_sum);
        decSampleOffsetY += BLK_16;
		outSampleOffsetY += BLK_16;
        grainStripe += BLK_16;
      }
	  decSampleOffsetY += decSampleStride - widthComp;
	  outSampleOffsetY += outSampleStride - widthComp;
      grainStripe      += grainStripeWidth - widthComp;
    }
  }
  else
  {
    int32_t  grainSample;
    uint8_t  decodeSample;
    uint16_t maxRange;
    maxRange = (1 << bitDepth) - 1;
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp - BLK_16; k += BLK_16)
      {
        __m128i __m128i_decData = _mm_loadu_si128((__m128i *) decSampleOffsetY);
        __m128i __m128i_grainSt = _mm_loadu_si128((__m128i *) grainStripe);

        __m128i __m128i_decData1 = _mm_cvtepu8_epi16(__m128i_decData);
        __m128i __m128i_decData2 = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *) (decSampleOffsetY + BLK_8)));

        __m128i __m128i_grainSt1 = _mm_cvtepi8_epi16(__m128i_grainSt);
        __m128i __m128i_grainSt2 = _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *) (grainStripe + BLK_8)));

        __m128i __m128i_sum1 = _mm_add_epi16(__m128i_decData1, __m128i_grainSt1);
        __m128i __m128i_sum2 = _mm_add_epi16(__m128i_decData2, __m128i_grainSt2);
        __m128i __m128i_sum  = _mm_packus_epi16(__m128i_sum1, __m128i_sum2);
        _mm_storeu_si128((__m128i *) outSampleOffsetY, __m128i_sum);
        decSampleOffsetY += BLK_16;
		outSampleOffsetY += BLK_16;
        grainStripe += BLK_16;
      }
      for (; k < widthComp; k++)
      {
        decodeSample      = *decSampleOffsetY;
        grainSample       = *grainStripe;
        grainSample       = CLIP3(0, maxRange, (grainSample + decodeSample));
        *outSampleOffsetY = (uint8_t) grainSample;
        decSampleOffsetY++;
		outSampleOffsetY++;
        grainStripe++;
      }
	  decSampleOffsetY += decSampleStride - widthComp;
	  outSampleOffsetY += outSampleStride - widthComp;
      grainStripe      += grainStripeWidth - widthComp;
    }
  }
}

void deblock_grain_stripe(int8_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  int32_t  vertCtr;
  uint32_t pos8;

  int8_t *grainStripe_const = grainStripe;

  for (vertCtr = 0; vertCtr < blkH; vertCtr++)
  {
    for (pos8 = 0; pos8 < (widthComp - blkW); pos8 += blkW)
    {
      left1                     = *(grainStripe + blkW - 2);
      left0                     = *(grainStripe + blkW - 1);
      right0                    = *(grainStripe + blkW + 0);
      right1                    = *(grainStripe + blkW + 1);
      *(grainStripe + blkW + 0) = (int8_t) ((left0 + (right0 << 1) + right1) >> 2);
      *(grainStripe + blkW - 1) = (int8_t) ((left1 + (left0 << 1) + right0) >> 2);
      grainStripe += blkW;
    }
    grainStripe_const += strideComp;
    grainStripe = grainStripe_const;
  }
}

void deblock_grain_stripe_hbd(int16_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  int32_t  vertCtr;
  uint32_t pos8;

  int16_t *grainStripe_const = grainStripe;

  for (vertCtr = 0; vertCtr < blkH; vertCtr++)
  {
    for (pos8 = 0; pos8 < (widthComp - blkW); pos8 += blkW)
    {
      left1                     = *(grainStripe + blkW - 2);
      left0                     = *(grainStripe + blkW - 1);
      right0                    = *(grainStripe + blkW + 0);
      right1                    = *(grainStripe + blkW + 1);
      *(grainStripe + blkW + 0) = (int8_t) ((left0 + (right0 << 1) + right1) >> 2);
      *(grainStripe + blkW - 1) = (int8_t) ((left1 + (left0 << 1) + right0) >> 2);
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
  uint32_t l;
  int8_t  *database_h_v = &grain_synt->dataBase[1][h][v][lOffset][kOffset];

  __m128i __m128i_scaleFactor = _mm_set1_epi16((int16_t) scaleFactor);
  int16_t shiftFactor         = log2ScaleFactor + GRAIN_SCALE;

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++)
  {
    __m128i __m128i_database_h_v2;
    __m128i __m128i_grainStripe1;
    __m128i __m128i_grainStripe2;
    __m128i __m128i_grainStripe;
    __m128i __m128i_database_h_v  = _mm_loadu_si128((__m128i *) database_h_v);
    __m128i __m128i_database_h_v1 = _mm_cvtepi8_epi16(__m128i_database_h_v);
    __m128i_database_h_v          = _mm_loadu_si128((__m128i *) (database_h_v + BLK_8));
    database_h_v += DATA_BASE_SIZE;
    __m128i_database_h_v2 = _mm_cvtepi8_epi16(__m128i_database_h_v);

    __m128i_grainStripe1 = _mm_mullo_epi16(__m128i_database_h_v1, __m128i_scaleFactor);
    __m128i_grainStripe1 = _mm_srai_epi16(__m128i_grainStripe1, shiftFactor);
    __m128i_grainStripe2 = _mm_mullo_epi16(__m128i_database_h_v2, __m128i_scaleFactor);
    __m128i_grainStripe2 = _mm_srai_epi16(__m128i_grainStripe2, shiftFactor);

    __m128i_grainStripe = _mm_packs_epi16(__m128i_grainStripe1, __m128i_grainStripe2);

    _mm_storeu_si128((__m128i *) grainStripe, __m128i_grainStripe);
    grainStripe += width;
  }
}

void simulate_grain_blk8x8_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t l;
  int8_t  *database_h_v = &grain_synt->dataBase[0][h][v][lOffset][kOffset];

  __m128i __m128i_scaleFactor = _mm_set1_epi16((int16_t) scaleFactor);
  __m128i __m128i_zerovals    = _mm_set1_epi16(0);
  int16_t shiftFactor         = log2ScaleFactor + GRAIN_SCALE;

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_8; l += 2)
  {
    __m128i __m128i_grainStripe1;
    __m128i __m128i_database_h_v2;
    __m128i __m128i_grainStripe2;
    __m128i __m128i_grainStripe;
    __m128i __m128i_database_h_v1 = _mm_loadu_si128((__m128i *) database_h_v);
    database_h_v += DATA_BASE_SIZE;
    __m128i_database_h_v1 = _mm_cvtepi8_epi16(__m128i_database_h_v1);

    __m128i_grainStripe1 = _mm_mullo_epi16(__m128i_database_h_v1, __m128i_scaleFactor);
    __m128i_grainStripe1 = _mm_srai_epi16(__m128i_grainStripe1, shiftFactor);

    __m128i_database_h_v2 = _mm_loadu_si128((__m128i *) database_h_v);
    database_h_v += DATA_BASE_SIZE;
    __m128i_database_h_v2 = _mm_cvtepi8_epi16(__m128i_database_h_v2);

    __m128i_grainStripe2 = _mm_mullo_epi16(__m128i_database_h_v2, __m128i_scaleFactor);
    __m128i_grainStripe2 = _mm_srai_epi16(__m128i_grainStripe2, shiftFactor);
    __m128i_grainStripe  = _mm_packs_epi16(__m128i_grainStripe1, __m128i_zerovals);

    _mm_storel_epi64((__m128i *) grainStripe, __m128i_grainStripe);
    grainStripe += width;
    __m128i_grainStripe = _mm_packs_epi16(__m128i_grainStripe2, __m128i_zerovals);
    _mm_storel_epi64((__m128i *) grainStripe, __m128i_grainStripe);
    grainStripe += width;
  }
}

void simulate_grain_blk8x8_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                 uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                 uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t l;
  int8_t  *database_h_v = &grain_synt->dataBase[0][h][v][lOffset][kOffset];

  __m128i __m128i_scaleFactor = _mm_set1_epi32(scaleFactor);
  int16_t shiftFactor         = log2ScaleFactor + GRAIN_SCALE;

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_8; l++)
  {
    __m128i __m128i_grainStripe1;
    __m128i __m128i_grainStripe2;
    __m128i __m128i_grainStripe;
    __m128i __m128i_database_h_v1 = _mm_loadu_si128((__m128i *) database_h_v);
    __m128i __m128i_database_h_v2 = _mm_loadu_si128((__m128i *) (database_h_v + 4));
    database_h_v += DATA_BASE_SIZE;
    __m128i_database_h_v1 = _mm_cvtepi8_epi32(__m128i_database_h_v1);
    __m128i_database_h_v2 = _mm_cvtepi8_epi32(__m128i_database_h_v2);
    __m128i_grainStripe1  = _mm_mullo_epi32(__m128i_database_h_v1, __m128i_scaleFactor);
    __m128i_grainStripe2  = _mm_mullo_epi32(__m128i_database_h_v2, __m128i_scaleFactor);
    __m128i_grainStripe1  = _mm_srai_epi32(__m128i_grainStripe1, shiftFactor);
    __m128i_grainStripe2  = _mm_srai_epi32(__m128i_grainStripe2, shiftFactor);
    __m128i_grainStripe   = _mm_packs_epi32(__m128i_grainStripe1, __m128i_grainStripe2);
    _mm_storeu_si128((__m128i *) grainStripe, __m128i_grainStripe);
    grainStripe += width;
  }
}

void simulate_grain_blk16x16_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8,
                                   GrainSynthesisStruct *grain_synt, uint32_t width, uint8_t log2ScaleFactor,
                                   int32_t scaleFactor, uint32_t kOffset, uint32_t lOffset, uint32_t h, uint32_t v,
                                   uint32_t xSize)
{
  uint32_t l;
  int8_t  *database_h_v = &grain_synt->dataBase[1][h][v][lOffset][kOffset];

  __m128i __m128i_scaleFactor = _mm_set1_epi32((int32_t) scaleFactor);
  int16_t shiftFactor         = log2ScaleFactor + GRAIN_SCALE;

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++)
  {
    __m128i __m128i_grainStripe1;
    __m128i __m128i_grainStripe2;
    __m128i __m128i_grainStripe3;
    __m128i __m128i_grainStripe4;
    __m128i __m128i_grainStripe_st1;
    __m128i __m128i_grainStripe_st2;
    __m128i __m128i_database_h_v1 = _mm_loadu_si128((__m128i *) database_h_v);
    __m128i __m128i_database_h_v2 = _mm_loadu_si128((__m128i *) (database_h_v + 4));
    __m128i __m128i_database_h_v3 = _mm_loadu_si128((__m128i *) (database_h_v + 8));
    __m128i __m128i_database_h_v4 = _mm_loadu_si128((__m128i *) (database_h_v + 12));
    database_h_v += DATA_BASE_SIZE;
    __m128i_database_h_v1 = _mm_cvtepi8_epi32(__m128i_database_h_v1);
    __m128i_database_h_v2 = _mm_cvtepi8_epi32(__m128i_database_h_v2);
    __m128i_database_h_v3 = _mm_cvtepi8_epi32(__m128i_database_h_v3);
    __m128i_database_h_v4 = _mm_cvtepi8_epi32(__m128i_database_h_v4);

    __m128i_grainStripe1    = _mm_mullo_epi32(__m128i_database_h_v1, __m128i_scaleFactor);
    __m128i_grainStripe2    = _mm_mullo_epi32(__m128i_database_h_v2, __m128i_scaleFactor);
    __m128i_grainStripe3    = _mm_mullo_epi32(__m128i_database_h_v3, __m128i_scaleFactor);
    __m128i_grainStripe4    = _mm_mullo_epi32(__m128i_database_h_v4, __m128i_scaleFactor);
    __m128i_grainStripe1    = _mm_srai_epi32(__m128i_grainStripe1, shiftFactor);
    __m128i_grainStripe2    = _mm_srai_epi32(__m128i_grainStripe2, shiftFactor);
    __m128i_grainStripe3    = _mm_srai_epi32(__m128i_grainStripe3, shiftFactor);
    __m128i_grainStripe4    = _mm_srai_epi32(__m128i_grainStripe4, shiftFactor);
    __m128i_grainStripe_st1 = _mm_packs_epi32(__m128i_grainStripe1, __m128i_grainStripe2);
    __m128i_grainStripe_st2 = _mm_packs_epi32(__m128i_grainStripe3, __m128i_grainStripe4);
    _mm_storeu_si128((__m128i *) grainStripe, __m128i_grainStripe_st1);
    _mm_storeu_si128((__m128i *) (grainStripe + BLK_8), __m128i_grainStripe_st2);
    grainStripe += width;
  }
}
