#ifndef __VisualCrytographMC_H__
#define __VisualCrytographMC_H__
#include "common.h"
#define CLINE 64

typedef struct MCData{
	level *pShare1;
	level *pShare2;
	level *pImgData;
	int iTd;
	int iNumOfThreads;
	int iWidth;
	int iHeight;
	unsigned int *pSeeds;
	int iCodecPath;
}MCData;

void VCEncoderMC(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR);
void VCDecoderMC(ImageData *pcShare1, ImageData *pcShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR);
void ExtVCEncoderMC(ImageData *pcImageData1, ImageData *pcImageData2, ImageData *pcImageData3, ImageData *pcShareExt1, ImageData *pcShareExt2, TimeRecord *pTR);
#endif
