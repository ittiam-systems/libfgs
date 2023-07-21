#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "../../fgs/api/error_codes.h"
#include "../../fgs/api/fgs_api.h"
#include "fgs-module-priv.h"

GrainCharacteristicApi fgcCTx     = { 0 };
FGSInitParams          initParams = { 0 };
void                  *psFgsHandle;

/* Main function */
void create_fgs(uint32_t width, uint32_t height, uint8_t bits)
{
  /*Init call to generate handlew with data base*/
  fgcCTx.width           = width;
  fgcCTx.height          = height;
  fgcCTx.bitDepth        = 8;
  initParams.maxWidth    = width;
  initParams.maxHeight   = height;
  initParams.maxbitDepth = bits;
  initParams.numThreads  = 1;

  /*Init call to generate handlew with data base*/
  psFgsHandle = fgs_create(&initParams);
  if (NULL == psFgsHandle)
  {
    printf("Create failed");
    return;
  }
}

void setFGSparams(FilmGrainCharacteristicsStruct *sei)
{
  fgcCTx.fgcParameters.filmGrainModelId                     = sei->filmGrainModelId;
  fgcCTx.fgcParameters.filmGrainCharacteristicsCancelFlag   = sei->filmGrainCharacteristicsCancelFlag;
  fgcCTx.fgcParameters.separateColourDescriptionPresentFlag = sei->separateColourDescriptionPresentFlag;

  if (fgcCTx.fgcParameters.separateColourDescriptionPresentFlag)
  {
    fgcCTx.fgcParameters.filmGrainBitDepthLumaMinus8      = sei->filmGrainBitDepthLumaMinus8;
    fgcCTx.fgcParameters.filmGrainBitDepthChromaMinus8    = sei->filmGrainBitDepthChromaMinus8;
    fgcCTx.fgcParameters.filmGrainFullRangeFlag           = sei->filmGrainFullRangeFlag;
    fgcCTx.fgcParameters.filmGrainColourPrimaries         = sei->filmGrainColourPrimaries;
    fgcCTx.fgcParameters.filmGrainTransferCharacteristics = sei->filmGrainTransferCharacteristics;
    fgcCTx.fgcParameters.filmGrainMatrixCoefficients      = sei->filmGrainMatrixCoefficients;
  }
  fgcCTx.fgcParameters.blendingModeId  = sei->blendingModeId;
  fgcCTx.fgcParameters.log2ScaleFactor = sei->log2ScaleFactor;

  fgcCTx.fgcParameters.filmGrainCharacteristicsRepetitionPeriod = sei->filmGrainCharacteristicsRepetitionPeriod;
  for (int c = 0; c < 3; c++)
  {
    fgcCTx.fgcParameters.compModelPresentFlag[c]        = sei->compModelPresentFlag[c];
    fgcCTx.fgcParameters.numIntensityIntervalsMinus1[c] = sei->numIntensityIntervalsMinus1[c];
    fgcCTx.fgcParameters.numModelValuesMinus1[c]        = sei->numModelValuesMinus1[c];
    if (fgcCTx.fgcParameters.compModelPresentFlag[c])
    {
      for (int i = 0; i <= fgcCTx.fgcParameters.numIntensityIntervalsMinus1[c]; i++)
      {
        fgcCTx.fgcParameters.intensityIntervalLowerBound[c][i] = sei->intensityIntervalLowerBound[c][i];
        fgcCTx.fgcParameters.intensityIntervalUpperBound[c][i] = sei->intensityIntervalUpperBound[c][i];

        for (int n = 0; n <= fgcCTx.fgcParameters.numModelValuesMinus1[c]; n++)
        {
          if (!n)
          {
            fgcCTx.fgcParameters.compModelValue[c][i][n] = sei->compModelValue[c][i][n];
          }
          else
          {
            fgcCTx.fgcParameters.compModelValue[c][i][n] = sei->compModelValue[c][i][n];
          }
        }
      }
    }
  }
}

void FGS_process(void *Y, void *U, void *V, unsigned int strideY, unsigned int strideUV)
{
  uint8_t *pdecPelFrmOut = NULL;
  pdecPelFrmOut          = malloc(sizeof(uint8_t) * fgcCTx.width * fgcCTx.height);
  fgcCTx.poc             = 0;
  fgcCTx.idrPicId        = 0;
  fgcCTx.inpBufY         = Y;
  fgcCTx.inpBufU         = U;
  fgcCTx.inpBufV         = V;
  fgcCTx.outBufY         = pdecPelFrmOut;
  fgcCTx.outBufU         = U;
  fgcCTx.outBufV         = V;
  fgcCTx.inpStrideY      = strideY;
  fgcCTx.inpStrideU      = strideUV;
  fgcCTx.inpStrideV      = strideUV;
  fgcCTx.outStrideY      = strideY;
  fgcCTx.outStrideU      = strideUV;
  fgcCTx.outStrideV      = strideUV;

  fgcCTx.fgcParameters.blockSize = 8;

  /* Function call to Film Grain Synthesizer */
  fgcCTx.errorCode = fgs_process(fgcCTx, psFgsHandle);
  if (FGS_SUCCESS != fgcCTx.errorCode)
  {
    printf("\nGrain synthesis in not performed. Error code: 0x%x \n", fgcCTx.errorCode);
    return;
  }
  if (NULL != pdecPelFrmOut)
  {
    free(pdecPelFrmOut);
  }
}

void FGS_delete()
{
  fgs_delete(psFgsHandle);
}

void *get_outBufY()
{
  if (fgcCTx.outBufY == NULL)
  {
    return NULL;
  }
  return fgcCTx.outBufY;
}

void *get_outBufU()
{
  if (fgcCTx.outBufU == NULL)
  {
    return NULL;
  }
  return fgcCTx.outBufU;
}

void *get_outBufV()
{
  if (fgcCTx.outBufV == NULL)
  {
    return NULL;
  }
  return fgcCTx.outBufV;
}

void *get_inpBufY()
{
  if (fgcCTx.inpBufY == NULL)
  {
    return NULL;
  }
  return fgcCTx.inpBufY;
}

void *get_inpBufU()
{
  if (fgcCTx.inpBufU == NULL)
  {
    return NULL;
  }
  return fgcCTx.inpBufU;
}

void *get_inpBufV()
{
  if (fgcCTx.inpBufV == NULL)
  {
    return NULL;
  }
  return fgcCTx.inpBufV;
}