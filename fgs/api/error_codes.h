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
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY
   , OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
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

#ifndef _ERROR_CODES_H_
#define _ERROR_CODES_H_

#define FAILURE_RET 1
#define SUCCESS_RET 0
/* Error start codes for various classes of errors */
#define FGS_FILE_IO_ERROR 0x0010
#define FGS_PARAM_ERROR 0x0020

/* Error codes for various errors in SMPTE-RDD5 standalone grain synthesizer */
typedef enum
{
  /* No error */
  FGS_SUCCESS = 0,
  /* Invalid input width */
  FGS_INVALID_WIDTH = FGS_FILE_IO_ERROR + 0x01,
  /* Invalid input height */
  FGS_INVALID_HEIGHT = FGS_FILE_IO_ERROR + 0x02,
  /* Invalid Chroma format idc */
  FGS_INVALID_CHROMA_FORMAT = FGS_FILE_IO_ERROR + 0x03,
  /* Invalid bit depth */
  FGS_INVALID_BIT_DEPTH = FGS_FILE_IO_ERROR + 0x04,
  /* Invalid number of threads */
  FGS_INVALID_NUM_THREAD = FGS_FILE_IO_ERROR + 0x05,
  /* Invalid Film grain characteristic cancel flag */
  FGS_INVALID_FGC_CANCEL_FLAG = FGS_PARAM_ERROR + 0x01,
  /* Invalid film grain model id */
  FGS_INVALID_GRAIN_MODEL_ID = FGS_PARAM_ERROR + 0x02,
  /* Invalid separate color description present flag */
  FGS_INVALID_SEP_COL_DES_FLAG = FGS_PARAM_ERROR + 0x03,
  /* Invalid blending mode */
  FGS_INVALID_BLEND_MODE = FGS_PARAM_ERROR + 0x04,
  /* Invalid log_2_scale_factor value */
  FGS_INVALID_LOG2_SCALE_FACTOR = FGS_PARAM_ERROR + 0x05,
  /* Invalid component model present flag */
  FGS_INVALID_COMP_MODEL_PRESENT_FLAG = FGS_PARAM_ERROR + 0x06,
  /* Invalid number of model values */
  FGS_INVALID_NUM_MODEL_VALUES = FGS_PARAM_ERROR + 0x07,
  /* Invalid bound values, overlapping boundaries */
  FGS_INVALID_INTENSITY_BOUNDARY_VALUES = FGS_PARAM_ERROR + 0x08,
  /* Invalid standard deviation */
  FGS_INVALID_STANDARD_DEVIATION = FGS_PARAM_ERROR + 0x09,
  /* Invalid cut off frequencies */
  FGS_INVALID_CUT_OFF_FREQUENCIES = FGS_PARAM_ERROR + 0x0A,
  /* Invalid number of cut off frequency pairs */
  FGS_INVALID_NUM_CUT_OFF_FREQ_PAIRS = FGS_PARAM_ERROR + 0x0B,
  /* Invalid film grain characteristics repetition period */
  FGS_INVALID_FGC_REPETETION_PERIOD = FGS_PARAM_ERROR + 0x0C,
  /* Invalid blockSize value */
  FGS_INVALID_BLOCK_SIZE = FGS_PARAM_ERROR + 0x0D,
  /* Failure error code */
  FGS_FAIL = 0xFF
} FGS_ERROR_T;

#endif /* _ERROR_CODES_H_ */
