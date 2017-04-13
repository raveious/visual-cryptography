
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "common.h"
#include "VisualCryptographyCPUTest.h"

const level WhiteShare1[2][2] = {0,0,1,1};
const level WhiteShare2[2][2] = {0,0,1,1};
const level BlackShare1[2][2] = {0,0,1,1};
const level BlackShare2[2][2] = {1,1,0,0};

void GenerateShareCPU(level cColor, level share1[][2], level share2[][2])
{
// ************************************************************************************
// DO NOT CHANGE ANYTHINNG BEFORE THIS POINT in YOUR CODE                             *
// This is a function that randomly generates share pixels for each image pixel.      *                                                                                                  *
// You do not need to use this function. But you can complete this function and call  *
// it from VCEncoderCPU for the processing of each image pixel.                       *
// Four pixel sets for WHITE  and BLACK pixels are in array Share1[2][2] and          *
// Share2[2][2]                                                                       *
// ************************************************************************************
//                                        ^
//                                        |
//                                        |
//                                        |
//                                        |
//                                        |
//                                        |
//                                        |
//                                        |
//                                        v
// ************************************************************************************
//                                                                                    *
// Pixel generating function ends here.                                               *
// Four pixel sets for WHITE  and BLACK pixels are in array share1[2][2] and          *
// share2[2][2]                                                                       *
// DO NOT CHANGE ANYTHINNG AFTER THIS POINT in YOUR CODE                              *
// ************************************************************************************
}

void VCEncoderCPUTest(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR)
{
	int i,j;
	level *pShare1, *pShare2;
	level *pShare1NxtRow, *pShare2NxtRow;
	level share1[2][2];
	level share2[2][2];
	struct timeval start,stop;

	printf("CPUTest Encoding... \n");

	//------Generate shares pixels---------//
	pcShare1->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
    memset(pcShare1->imgData,0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	pcShare2->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	memset(pcShare2->imgData,0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	pShare1 = pcShare1->imgData;
	pShare2 = pcShare2->imgData;

	gettimeofday(&start,0);
  // **********************************************************************************
  // DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                            *
  // Your Program should go here.                                                     *
  // At this point the input image of size (iHeight x iWidth) pixels (type level)     *
  // is stored at address pcImageData->imgData. The share images of size              *
  // (2*(iHeight x 2*iWidth)) will go into addresses pShare1 and pShare2.             *
  // **********************************************************************************
  //                                        ^
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        v
  // ********************************************************************************
  // Your Program ends here.                                                        *
  // At this point the share images of size (2*iHeight x 2*iWidth) should have      *
  // been stored into addresses pShare1 and pShare2.                                *
  // DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
  // ********************************************************************************

	gettimeofday(&stop,0);
	pTR->EncryptionTime = ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	//----------Fill in shares----------//
	pcShare1->iWidth  = 2 * pcImageData->iWidth;
	pcShare1->iHeight = 2 * pcImageData->iHeight;
	pcShare2->iWidth  = 2 * pcImageData->iWidth;
	pcShare2->iHeight = 2 * pcImageData->iHeight;

	//Fill in file header
	memcpy(&(pcShare1->cBmpFH), &(pcImageData->cBmpFH), sizeof(BitMapFileHeader));
	memcpy(&(pcShare2->cBmpFH), &(pcImageData->cBmpFH), sizeof(BitMapFileHeader));

	//Fill in info header
	memcpy(&(pcShare1->cBmpIH), &(pcImageData->cBmpIH), sizeof(BitMapInfoHeader));
	memcpy(&(pcShare2->cBmpIH), &(pcImageData->cBmpIH), sizeof(BitMapInfoHeader));

	//Fill in color table
	memcpy(pcShare1->cBmpImage, pcImageData->cBmpImage, 2*sizeof(BitMapImage));
	memcpy(pcShare2->cBmpImage, pcImageData->cBmpImage, 2*sizeof(BitMapImage));

	//----------Adjust shares----------//
	sprintf(pcShare1->imageName, "Share1T_%s",pcImageData->imageName); //adjust name
	sprintf(pcShare2->imageName, "Share2T_%s",pcImageData->imageName);
	pcShare1->cBmpIH.width  = pcShare1->iWidth; //adjust width
	pcShare2->cBmpIH.width  = pcShare2->iWidth;
	pcShare1->cBmpIH.height = pcShare1->iHeight;  //adjust height
	pcShare2->cBmpIH.height = pcShare2->iHeight;
	pcShare1->cBmpIH.biSizeImage = pcShare1->cBmpIH.height * (((pcShare1->cBmpIH.bitPix * pcShare1->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare2->cBmpIH.biSizeImage = pcShare2->cBmpIH.height * (((pcShare2->cBmpIH.bitPix * pcShare2->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare1->cBmpFH.bfSize = pcShare1->cBmpIH.biSizeImage + pcShare1->cBmpFH.bfOffBits;
	pcShare2->cBmpFH.bfSize = pcShare2->cBmpIH.biSizeImage + pcShare2->cBmpFH.bfOffBits;
}

void VCDecoderCPUTest(ImageData *pcShare1, ImageData *pcShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR)
{
	int i,j;
	level cSum;
	struct timeval start,stop;

	gettimeofday(&start,0);
	printf("CPUTest Decoding ...\n");

	memcpy(pcImageData, pcShare1, sizeof(ImageData));
	pcImageData->imgData = (level *)malloc(pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	memset(pcImageData->imgData,0, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	sprintf(pcImageData->imageName, "ReconT_%s",pInputImageName);

  // ********************************************************************************
  // DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
  // Your Program should go here.                                                   *
  // At this point the input share files of of size (iHeight x iWidth) pixels       *
  // of (type level) are stored at address pcShare1->imgData and pcShare2->imgData. *
  // The reconstructed image of size (iHeight x iWidth) will go into address        *
  // pcImageData->imgData.                                                          *
  // ********************************************************************************
  //                                        ^
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        |
  //                                        v
  // ********************************************************************************
  // Your Program ends here.                                                        *
  // At this point the reconstructed images of size (iHeight x iWidth) should       *
  // been stored into address pImage                                                *
  // DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
  // ********************************************************************************

	gettimeofday(&stop,0);
	pTR->DecodeTime = ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

}
