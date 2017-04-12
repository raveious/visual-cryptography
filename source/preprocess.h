#ifndef __PREPROCESS_H__
#define __PREPROCESS_H__

#include "common.h"

#define CHKEXIT(A) if(A!=1){printf("Image Allocation Failed"); assert(0);}


void InitTimeRecord(TimeRecord *pTR);
void PackPixelForBitMap(ImageData* pcImageData);
void writeBitMap(ImageData* pcImageData);
void ParseBitMap(char *ImageName, ImageData *pcImageData);
void CheckCorrectness(ImageData *pInputImage, ImageData *pcOutputImage, int iPlatform);
int ImageAlloc(ImageData **pImageData);
void ImageFree(ImageData *pImageData);

#endif