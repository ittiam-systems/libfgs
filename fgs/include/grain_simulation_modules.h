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

#ifndef _GRAIN_SIMULATION_MODULES_H_
#define _GRAIN_SIMULATION_MODULES_H_

#define UNUSED(x) ((void) x)
#include "fgs_structs.h"

extern uint32_t block_average_hbd(uint16_t *decSampleBlk, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                                  uint32_t xSize, uint8_t blkSz, uint8_t shift_Size, uint8_t bitDepth);

extern uint32_t block_average(uint8_t *decSampleBlk, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                              uint32_t xSize, uint8_t blkSz, uint8_t shift_Size, uint8_t bitDepth);

extern uint32_t block_avg_8x8_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                                    uint32_t xSize, uint8_t bitDepth);

extern uint32_t block_avg_16x16_10bit(uint16_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples,
                                      uint32_t ySize, uint32_t xSize, uint8_t bitDepth);

extern uint32_t block_avg_8x8_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                                   uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth);

extern uint32_t block_avg_16x16_8bit(uint8_t *decSampleBlk8, uint32_t widthComp, uint16_t *pNumSamples, uint32_t ySize,
                                     uint32_t xSize, uint8_t shift_Size, uint8_t bitDepth);

void blend_stripe_10bit(uint16_t *decSampleHbdOffsetY, uint16_t *outSampleHbdOffsetY,  int16_t *grainStripe, uint32_t widthComp, 
                        uint32_t decSampleStride, uint32_t outSampleStride, uint32_t grainStripeWidth, 
						uint32_t blockHeight, uint8_t bitDepth);

void blend_stripe_8bit(uint8_t *decSampleOffsetY, uint8_t *outSampleOffsetY, int8_t *grainStripe, uint32_t widthComp,
                       uint32_t decSampleStride, uint32_t outSampleStride, uint32_t grainStripeWidth,
                       uint32_t blockHeight, uint8_t bitDepth);

void deblock_grain_stripe(int8_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH);

void deblock_grain_stripe_hbd(int16_t *grainStripe, uint32_t widthComp, uint32_t strideComp, int8_t blkW, int8_t blkH);

void simulate_grain_blk16x16_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                  uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                  uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize);

void simulate_grain_blk8x8_8bit(int8_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize);

void simulate_grain_blk8x8_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8, GrainSynthesisStruct *grain_synt,
                                 uint32_t width, uint8_t log2ScaleFactor, int32_t scaleFactor, uint32_t kOffset,
                                 uint32_t lOffset, uint32_t h, uint32_t v, uint32_t xSize);

void simulate_grain_blk16x16_10bit(int16_t *grainStripe, uint32_t grainStripeOffsetBlk8,
                                   GrainSynthesisStruct *grain_synt, uint32_t width, uint8_t log2ScaleFactor,
                                   int32_t scaleFactor, uint32_t kOffset, uint32_t lOffset, uint32_t h, uint32_t v,
                                   uint32_t xSize);

#endif
