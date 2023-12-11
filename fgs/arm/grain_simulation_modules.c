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

#include <arm_neon.h>
#include <string.h>
#include "fgs_api.h"
#include "grain_simulation_modules.h"

void fgs_copy_2d(uint8_t *p_src, int32_t src_stride, uint8_t *p_dst, int32_t dst_stride,
                    int32_t width, int32_t height, int32_t data_size)
{
  int32_t ctr;

  for(ctr = 0; ctr < height; ctr++)
  {
    memcpy(p_dst, p_src, width*data_size);
    p_src += src_stride;
    p_dst += dst_stride;
  }
}

void fgs_copy_2d_hbd(uint16_t *p_src, int32_t src_stride, uint16_t *p_dst, int32_t dst_stride,
                    int32_t width, int32_t height, int32_t data_size)
{
  int32_t ctr;

  for(ctr = 0; ctr < height; ctr++)
  {
    memcpy(p_dst, p_src, width*data_size);
    p_src += src_stride;
    p_dst += dst_stride;
  }
}


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

/* Function to calculate block average for 8x8 blocks of 10 bit depth */
uint32_t block_avg_8x8_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                             uint32_t xSize, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  uint16x8_t u16x8_blockAvg = vdupq_n_u16(0);   // vmovq_n_u16

  UNUSED(xSize);

  for (k = 0; k < ySize; k++)
  {
    uint16x8_t u16x8_decSample = vld1q_u16(decSampleBlk8);
    decSampleBlk8 += widthComp;
    u16x8_blockAvg = vaddq_u16(u16x8_blockAvg, u16x8_decSample);
  }
  blockAvg = vaddvq_u16(u16x8_blockAvg);

  blockAvg = blockAvg >> (BLK_8_shift + (bitDepth - BIT_DEPTH_8));

  // If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed
  *pNumSamples = BLK_AREA_8x8;

  return blockAvg;
}

/* Function to calculate block average for 16x16 blocks of 10 bit depth */
uint32_t block_avg_16x16_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                               uint32_t xSize, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  uint16x8_t u16x8_blockAvg1 = vdupq_n_u16(0);   // vmovq_n_u16
  uint16x8_t u16x8_blockAvg2 = vdupq_n_u16(0);

  UNUSED(xSize);

  for (k = 0; k < ySize; k++)
  {
    uint16x8_t u16x8_decSample1 = vld1q_u16(decSampleBlk8);
    decSampleBlk8 += BLK_8;
    uint16x8_t u16x8_decSample2 = vld1q_u16(decSampleBlk8);
    decSampleBlk8 += widthComp - BLK_8;
    u16x8_blockAvg1 = vaddq_u16(u16x8_blockAvg1, u16x8_decSample1);
    u16x8_blockAvg2 = vaddq_u16(u16x8_blockAvg2, u16x8_decSample2);
  }
  uint32x4_t u32x4_blockAvg1 = vpaddlq_u16(u16x8_blockAvg1);
  uint32x4_t u32x4_blockAvg2 = vpaddlq_u16(u16x8_blockAvg2);
  blockAvg                   = vaddvq_u32(u32x4_blockAvg1) + vaddvq_u32(u32x4_blockAvg2);

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

/* Function to calculate block average for 8x8 blocks of 8 bit depth */
uint32_t block_avg_8x8_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                            uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  uint16x8_t u16x8_blockAvg = vdupq_n_u16(0);

  UNUSED(xSize);
  UNUSED(bitDepth);

  for (k = 0; k < ySize; k += 2)
  {
    uint8x8_t u8x8_decSample1 = vld1_u8(decSampleBlk8);
    decSampleBlk8 += widthComp;
    uint8x8_t u8x8_decSample2 = vld1_u8(decSampleBlk8);
    decSampleBlk8 += widthComp;
    uint8x16_t u8x16_decSampl = vcombine_u8(u8x8_decSample1, u8x8_decSample2);
    u16x8_blockAvg            = vpadalq_u8(u16x8_blockAvg, u8x16_decSampl);
  }
  blockAvg = vaddvq_u16(u16x8_blockAvg);

  blockAvg = blockAvg >> shift_Size;

  // If BLK_8 is not used or changed BLK_AREA_8x8 has to be changed
  *pNumSamples = BLK_AREA_8x8;
  return blockAvg;
}

/* Function to calculate block average for 16x16 blocks of 8 bit depth */
uint32_t block_avg_16x16_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                              uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth)
{
  uint32_t blockAvg = 0;
  uint8_t  k;

  uint16x8_t u16x8_blockAvg = vdupq_n_u16(0);

  UNUSED(bitDepth);
  UNUSED(xSize);

  for (k = 0; k < ySize; k++)
  {
    uint8x16_t u8x16_decSampl = vld1q_u8(decSampleBlk8);
    decSampleBlk8 += widthComp;
    u16x8_blockAvg = vpadalq_u8(u16x8_blockAvg, u8x16_decSampl);
  }
  blockAvg = vaddvq_u16(u16x8_blockAvg);
  blockAvg = blockAvg >> shift_Size;

  // If BLK_16 is not used or changed BLK_AREA_16x16 has to be changed
  *pNumSamples = BLK_AREA_16x16;
  return blockAvg;
}

/* Function to blend the coefficient for stripe in the 10 bit case */
void blend_stripe_10bit(uint16_t *decSampleHbdOffsetY, uint16_t *outSampleHbdOffsetY, int16_t *grainStripe,
                        uint32_t widthComp, uint32_t decSampleStride, uint32_t outSampleStride,
                        uint32_t grainStripeWidth, uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;

  uint16_t   maxRange         = (1 << bitDepth) - 1;
  uint16x8_t v_u16x8_maxRange = vdupq_n_u16(maxRange);

  if ((widthComp & 0x7) == 0)
  {
    //*TODO : Handle widths that are not multiple of 16
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp; k += BLK_8)
      {
        uint16x8_t v_u16x8_decodeSample = vld1q_u16(decSampleHbdOffsetY);
        int16x8_t  v_16x8_grainStripe   = vld1q_s16(grainStripe);
        uint16x8_t v_u16x8_output       = vsqaddq_u16(v_u16x8_decodeSample, v_16x8_grainStripe);
        v_u16x8_output                  = vminq_u16(v_u16x8_output, v_u16x8_maxRange);
        vst1q_u16(outSampleHbdOffsetY, v_u16x8_output);
        decSampleHbdOffsetY += BLK_8;
        outSampleHbdOffsetY += BLK_8;
        grainStripe += BLK_8;
      }
      outSampleHbdOffsetY += outSampleStride - widthComp;
      decSampleHbdOffsetY += decSampleStride - widthComp;
      grainStripe += grainStripeWidth - widthComp;
    }
  }
  else
  {
    int32_t grainSample;

    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp - BLK_8; k += BLK_8)
      {
        uint16x8_t v_u16x8_decodeSample = vld1q_u16(decSampleHbdOffsetY);
        int16x8_t  v_16x8_grainStripe   = vld1q_s16(grainStripe);
        uint16x8_t v_u16x8_output       = vsqaddq_u16(v_u16x8_decodeSample, v_16x8_grainStripe);
        v_u16x8_output                  = vminq_u16(v_u16x8_output, v_u16x8_maxRange);
        vst1q_u16(outSampleHbdOffsetY, v_u16x8_output);
        decSampleHbdOffsetY += BLK_8;
        outSampleHbdOffsetY += BLK_8;
        grainStripe += BLK_8;
      }
      for (; k < widthComp; k++)
      {
        grainSample          = *grainStripe;
        grainSample          = CLIP3(0, maxRange, grainSample + *decSampleHbdOffsetY);
        *outSampleHbdOffsetY = (uint16_t) grainSample;
        decSampleHbdOffsetY++;
        outSampleHbdOffsetY++;
        grainStripe++;
      }
      outSampleHbdOffsetY += outSampleStride - widthComp;
      decSampleHbdOffsetY += decSampleStride - widthComp;
      grainStripe += grainStripeWidth - widthComp;
    }
  }
}

/* Function to blend the coefficient for stripe in the 8 bit case */
void blend_stripe_8bit(uint8_t *decSampleOffsetY, uint8_t *outSampleOffsetY, int8_t *grainStripe, uint32_t widthComp,
                       uint32_t decSampleStride, uint32_t outSampleStride, uint32_t grainStripeWidth,
                       uint32_t blockHeight, uint8_t bitDepth)
{
  uint32_t k, l;

  if ((widthComp & 0xF) == 0)
  {
    //*TODO : Handle widths that are not multiple of 16
    for (l = 0; l < blockHeight; l++)
    {
      for (k = 0; k < widthComp; k += BLK_16)
      {
        uint8x16_t v_u8x16_decodeSample = vld1q_u8(decSampleOffsetY);
        int8x16_t  v_8x16_grainStripe   = vld1q_s8(grainStripe);
        uint8x16_t v_u8x16_output       = vsqaddq_u8(v_u8x16_decodeSample, v_8x16_grainStripe);
        vst1q_u8(outSampleOffsetY, v_u8x16_output);
        decSampleOffsetY += BLK_16;
        outSampleOffsetY += BLK_16;
        grainStripe += BLK_16;
      }
      decSampleOffsetY += decSampleStride - widthComp;
      outSampleOffsetY += outSampleStride - widthComp;
      grainStripe += grainStripeWidth - widthComp;
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
        uint8x16_t v_u8x16_decodeSample = vld1q_u8(decSampleOffsetY);
        int8x16_t  v_8x16_grainStripe   = vld1q_s8(grainStripe);
        uint8x16_t v_u8x16_output       = vsqaddq_u8(v_u8x16_decodeSample, v_8x16_grainStripe);
        vst1q_u8(outSampleOffsetY, v_u8x16_output);
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
      grainStripe += grainStripeWidth - widthComp;
    }
  }
}

/* Function to perform deblocking on a stripe */
void deblock_grain_stripe(int8_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  uint32_t pos8;
  int32_t  vertCtr;

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

/* Function to perform deblocking on a stripe with high bit depth */
void deblock_grain_stripe_hbd(int16_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH)
{
  int32_t  left1, left0, right0, right1;
  uint32_t pos8;
  int32_t  vertCtr;

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

/* Simulate the 16x16 block 8 bit with the decided grain parameters */
void simulate_grain_blk16x16_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                  uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                  uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t l;

  int8_t   *database_h_v       = &grain_synt->dataBase[1][h][v][lOffset][kOffset];
  int16x8_t v_16x8_ShiftFactor = vdupq_n_s16(-(log2ScaleFactor + GRAIN_SCALE));

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++)
  {
    int8x16_t v_8x16_database = vld1q_s8(database_h_v);
    database_h_v += DATA_BASE_SIZE;
    int8x8_t v_8x8_database1 = vget_high_s8(v_8x16_database);
    int8x8_t v_8x8_database2 = vget_low_s8(v_8x16_database);

    int16x8_t v_16x8_database1 = vmovl_s8(v_8x8_database1);
    int16x8_t v_16x8_database2 = vmovl_s8(v_8x8_database2);

    int16x8_t v_16x8_grainStripe1 = vmulq_n_s16(v_16x8_database1, scaleFactor);

    v_16x8_grainStripe1           = vshlq_s16(v_16x8_grainStripe1, v_16x8_ShiftFactor);
    int16x8_t v_16x8_grainStripe2 = vmulq_n_s16(v_16x8_database2, scaleFactor);

    v_16x8_grainStripe2 = vshlq_s16(v_16x8_grainStripe2, v_16x8_ShiftFactor);

    int8x8_t  v_8x8_grainStripe1 = vmovn_s16(v_16x8_grainStripe1);
    int8x8_t  v_8x8_grainStripe2 = vmovn_s16(v_16x8_grainStripe2);
    int8x16_t v_8x16_grainStripe = vcombine_s8(v_8x8_grainStripe2, v_8x8_grainStripe1);
    vst1q_s8(grainStripe, v_8x16_grainStripe);
    grainStripe += width;
  }
}

/* Simulate the 8x8 block 8 bit with the decided grain parameters */
void simulate_grain_blk8x8_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t  l;
  int8_t   *database_h_v       = &grain_synt->dataBase[0][h][v][lOffset][kOffset];
  int16x8_t v_16x8_ShiftFactor = vdupq_n_s16(-(log2ScaleFactor + GRAIN_SCALE));

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;
  for (l = 0; l < BLK_8; l++)
  {
    int8x8_t v_8x8_database = vld1_s8(database_h_v);
    database_h_v += DATA_BASE_SIZE;

    //*TODO: Enable vmull_s8 & disable vmovl_s8 & vmulq_n_s16, when scaleFactor is constrained to 127
    int16x8_t v_16x8_database = vmovl_s8(v_8x8_database);
    // multiply
    // int16x8_t v_16x8_grainStripe = vmull_s8(v_8x8_database, v_8x8_Scalefactor);
    int16x8_t v_16x8_grainStripe = vmulq_n_s16(v_16x8_database, scaleFactor);

    // shiftright
    // v_16x8_grainStripe = vshrq_n_s16(v_16x8_grainStripe, rightshift_fact);
    v_16x8_grainStripe         = vshlq_s16(v_16x8_grainStripe, v_16x8_ShiftFactor);
    int8x8_t v_8x8_grainStripe = vmovn_s16(v_16x8_grainStripe);
    vst1_s8(grainStripe, v_8x8_grainStripe);
    grainStripe += width;
  }
}

/* Simulate the 8x8 block 10 bit with the decided grain parameters */
void simulate_grain_blk8x8_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                 uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                 uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize)
{
  uint32_t  l;
  int8_t   *database_h_v       = &grain_synt->dataBase[0][h][v][lOffset][kOffset];
  int32x4_t v_32x4_ShiftFactor = vdupq_n_s32(-(log2ScaleFactor + GRAIN_SCALE));

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_8; l++)
  {
    int8x8_t v_8x8_database = vld1_s8(database_h_v);
    database_h_v += DATA_BASE_SIZE;

    int16x8_t v_16x8_database  = vmovl_s8(v_8x8_database);
    int16x4_t v_16x4_database1 = vget_high_s16(v_16x8_database);
    int16x4_t v_16x4_database2 = vget_low_s16(v_16x8_database);

    int32x4_t v_32x4_grainStripe1 = vmull_n_s16(v_16x4_database1, scaleFactor);
    int32x4_t v_32x4_grainStripe2 = vmull_n_s16(v_16x4_database2, scaleFactor);

    v_32x4_grainStripe1           = vshlq_s32(v_32x4_grainStripe1, v_32x4_ShiftFactor);
    v_32x4_grainStripe2           = vshlq_s32(v_32x4_grainStripe2, v_32x4_ShiftFactor);
    int16x4_t v_16x4_grainStripe1 = vmovn_s32(v_32x4_grainStripe1);
    int16x4_t v_16x4_grainStripe2 = vmovn_s32(v_32x4_grainStripe2);
    int16x8_t v_16x8_grainStripe  = vcombine_s16(v_16x4_grainStripe2, v_16x4_grainStripe1);
    vst1q_s16(grainStripe, v_16x8_grainStripe);
    grainStripe += width;
  }
}

/* Simulate the 16x16 block 10 bit with the decided grain parameters */
void simulate_grain_blk16x16_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8,
                                   GrainSynthesisStruct *grain_synt, uint32_t width, uint8_t log2ScaleFactor,
                                   int32_t scaleFactor, uint32_t kOffset, uint32_t lOffset, uint32_t h, uint32_t v,
                                   uint32_t xSize)
{
  uint32_t l;

  int8_t   *database_h_v       = &grain_synt->dataBase[1][h][v][lOffset][kOffset];
  int32x4_t v_32x4_ShiftFactor = vdupq_n_s32(-(log2ScaleFactor + GRAIN_SCALE));

  UNUSED(xSize);

  grainStripe += grainStripeOffsetBlk8;

  for (l = 0; l < BLK_16; l++)
  {
    int8x16_t v_8x16_database = vld1q_s8(database_h_v);
    database_h_v += DATA_BASE_SIZE;
    int8x8_t v_8x8_database1 = vget_high_s8(v_8x16_database);
    int8x8_t v_8x8_database2 = vget_low_s8(v_8x16_database);

    int16x8_t v_16x8_database  = vmovl_s8(v_8x8_database2);
    int16x4_t v_16x4_database1 = vget_high_s16(v_16x8_database);
    int16x4_t v_16x4_database2 = vget_low_s16(v_16x8_database);

    int32x4_t v_32x4_grainStripe1 = vmull_n_s16(v_16x4_database1, scaleFactor);
    int32x4_t v_32x4_grainStripe2 = vmull_n_s16(v_16x4_database2, scaleFactor);

    v_32x4_grainStripe1           = vshlq_s32(v_32x4_grainStripe1, v_32x4_ShiftFactor);
    v_32x4_grainStripe2           = vshlq_s32(v_32x4_grainStripe2, v_32x4_ShiftFactor);
    int16x4_t v_16x4_grainStripe1 = vmovn_s32(v_32x4_grainStripe1);
    int16x4_t v_16x4_grainStripe2 = vmovn_s32(v_32x4_grainStripe2);
    int16x8_t v_16x8_grainStripe  = vcombine_s16(v_16x4_grainStripe2, v_16x4_grainStripe1);
    vst1q_s16(grainStripe, v_16x8_grainStripe);
    grainStripe += BLK_8;

    v_16x8_database  = vmovl_s8(v_8x8_database1);
    v_16x4_database1 = vget_high_s16(v_16x8_database);
    v_16x4_database2 = vget_low_s16(v_16x8_database);

    v_32x4_grainStripe1 = vmull_n_s16(v_16x4_database1, scaleFactor);
    v_32x4_grainStripe2 = vmull_n_s16(v_16x4_database2, scaleFactor);

    v_32x4_grainStripe1 = vshlq_s32(v_32x4_grainStripe1, v_32x4_ShiftFactor);
    v_32x4_grainStripe2 = vshlq_s32(v_32x4_grainStripe2, v_32x4_ShiftFactor);
    v_16x4_grainStripe1 = vmovn_s32(v_32x4_grainStripe1);
    v_16x4_grainStripe2 = vmovn_s32(v_32x4_grainStripe2);
    v_16x8_grainStripe  = vcombine_s16(v_16x4_grainStripe2, v_16x4_grainStripe1);
    vst1q_s16(grainStripe, v_16x8_grainStripe);
    grainStripe += width - BLK_8;
  }
}
