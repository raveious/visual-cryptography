#ifndef __VisualCrytograph_H__
#define __VisualCrytograph_H__

void VCEncoderCPUDefault(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR);
void VCDecoderCPUDefault(ImageData *pShare1, ImageData *pShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR);
#endif
