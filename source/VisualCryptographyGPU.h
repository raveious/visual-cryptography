#ifndef __VisualCrytographGPU_H__
#define __VisualCrytographGPU_H__
#include "common.h"

void CheckCUDAError(const char *msg);
void VCEncoderGPU(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR);
void VCDecoderGPU(ImageData *pShare1, ImageData *pShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR);
void ExtVCEncoderGPU(ImageData *pcImageData1, ImageData *pcImageData2, ImageData *pcImageData3, ImageData *pcShareExt1, ImageData *pcShareExt2, TimeRecord *pTR);
#endif
