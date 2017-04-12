#ifndef __VisualCrytographCPU_H__
#define __VisualCrytographCPU_H__
#include "common.h"

void VCEncoderCPUTest(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR);
void VCDecoderCPUTest(ImageData *pShare1, ImageData *pShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR);
void ExtVCEncoderCPU(ImageData *pcImageData1, ImageData *pcImageData2, ImageData *pcImageData3, ImageData *pcShareExt1, ImageData *pcShareExt2, TimeRecord *pTR);
#endif
