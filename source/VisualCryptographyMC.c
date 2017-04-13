#include <pthread.h>
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
#include "VisualCryptographyMC.h"

static const level WhiteShare1[2][2] = {0,0,1,1};
static const level WhiteShare2[2][2] = {0,0,1,1};

static const level BlackShare1[2][2] = {0,0,1,1};
static const level BlackShare2[2][2] = {1,1,0,0};

void GenerateShareMC(level cColor, level share1[][2], level share2[][2], unsigned int r) {
// ************************************************************************************
// DO NOT CHANGE ANYTHINNG BEFORE THIS POINT in YOUR CODE                             *
// This is a function that randomly generates share pixels for each image pixel.      *                                                                                                  *
// You do not need to use this function. But you can complete this function and call  *
// it from PartialCodec for the processing of each image pixel.                       *
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
//                                        v
// ************************************************************************************
// Pixel geneates function ends here.                                                 *
// Four pixel sets for WHITE  and BLACK pixels are in array Share1[2][2] and          *
// Share2[2][2]                                                                       *
// DO NOT CHANGE ANYTHINNG AFTER THIS POINT in YOUR CODE                              *
// ************************************************************************************
}

void *PartialCodec(void *pcMCDataArg) {

	MCData *pcMCData = (MCData *)pcMCDataArg;

	// ********************************************************************************
	// DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
	// Your thread function should go here.                                           *
	// ********************************************************************************

	int segment_size = (pcMCData->iHeight * pcMCData->iWidth) / pcMCData->iNumOfThreads;
	int offset = segment_size * pcMCData->iTd;

	// ********************************************************************************
	// 1) If pcMCData->iCodecPath=ENCODE, the code performs encoding. In that case    *
	//    input image of size (iHeight x iWidth) pixels of (type level) is stored at  *
	//    address pcMCData->pImgData. The share images of size (2*iHeight x 2*iWidth) *
	//    are stored at addresses pcMCData->pShare1 and pcMCData->pShare2.            *
	// ********************************************************************************

	if (pcMCData->iCodecPath == ENCODE) {
		int invert = 0;
		int share_offset = 0;
		int alt_share_offset = 0;

		// loop over this threads segment
		for (int i = offset; i < (offset + segment_size); i++) {
			invert = rand_r(pcMCData->pSeeds) % 2;
			share_offset = (i * 2) + ((i / pcMCData->iWidth) * (pcMCData->iWidth * 2));
			alt_share_offset = share_offset + (pcMCData->iWidth * 2);

			// Select White or Black shares
			if (pcMCData->pImgData[i]) {
				pcMCData->pShare1[share_offset]         = WhiteShare1[0][0] ^ invert;
				pcMCData->pShare1[share_offset + 1]     = WhiteShare1[0][1] ^ invert;
				pcMCData->pShare1[alt_share_offset]     = WhiteShare1[1][0] ^ invert;
				pcMCData->pShare1[alt_share_offset + 1] = WhiteShare1[1][1] ^ invert;

				pcMCData->pShare2[share_offset]         = WhiteShare2[0][0] ^ invert;
				pcMCData->pShare2[share_offset + 1]     = WhiteShare2[0][1] ^ invert;
				pcMCData->pShare2[alt_share_offset]     = WhiteShare2[1][0] ^ invert;
				pcMCData->pShare2[alt_share_offset + 1] = WhiteShare2[1][1] ^ invert;
			} else {
				pcMCData->pShare1[share_offset]         = BlackShare1[0][0] ^ invert;
				pcMCData->pShare1[share_offset + 1]     = BlackShare1[0][1] ^ invert;
				pcMCData->pShare1[alt_share_offset]     = BlackShare1[1][0] ^ invert;
				pcMCData->pShare1[alt_share_offset + 1] = BlackShare1[1][1] ^ invert;

				pcMCData->pShare2[share_offset]         = BlackShare2[0][0] ^ invert;
				pcMCData->pShare2[share_offset + 1]     = BlackShare2[0][1] ^ invert;
				pcMCData->pShare2[alt_share_offset]     = BlackShare2[1][0] ^ invert;
				pcMCData->pShare2[alt_share_offset + 1] = BlackShare2[1][1] ^ invert;
			}
		}
	}
	// ********************************************************************************
	// 2) If pcMCData->iCodecPath==DECODE, the code performs decoding. In that case   *
	//    output image of size (iHeight x iWidth) pixels of (type level) is stored    *
	//    at address pcMCData->pImgData. The share images of size (iHeight x iWidth)  *
	//	  are stored at addresses pcMCData->pShare1 and pcMCData->pShare2.            *
	// ********************************************************************************
	else { // DECODE
		// loop over this threads segment
		for (int i = offset; i < (offset + segment_size); i++) {
			// Take logical and of all the pixels to generate defined response
			pcMCData->pImgData[i] = pcMCData->pShare1[i] & pcMCData->pShare2[i];

			// NOTE: The suppied solution uses logical NOR operation instead of
			// logical AND, but this doesn't yield the same result as what the
			// project description describes
		}
	}
	// ********************************************************************************
	// Your thread function code ends here.                                           *
	// DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
	// ********************************************************************************
}

void VCEncoderMC(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR)
{
	int iNumOfThreads, iTd;
	pthread_t *pcPthread;
	MCData    *pcMCData;
	struct timeval start,stop;
	unsigned int *pSeeds;

	printf("MC Encoding... \n");

	//------Generate shares pixels---------//
	pcShare1->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	memset(pcShare1->imgData,0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	pcShare2->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	memset(pcShare2->imgData,0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));

	// ********************************************************************************
	// DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
	// Number of threads goes in here.                                                *
	// ********************************************************************************

	iNumOfThreads = 64;

 	printf("|--Number of Multicore Encoding Threads: \%d \n",iNumOfThreads);

	// ********************************************************************************
	// End Number of threads                              														*
	// DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
	// ********************************************************************************

     printf("|--Number of Multi-Core Threads: %d\n",iNumOfThreads);

	if((pcImageData->iHeight % iNumOfThreads)!=0)
	{
		printf("Warning: the height of the original image is not divisible by number of threads\n");
	}

	if((pcImageData->iWidth % iNumOfThreads)!=0)
	{
		printf("Warning: the width of the original image is not divisible by number of threads\n");
	}

	pcPthread = (pthread_t *)malloc(sizeof(pthread_t)*iNumOfThreads);
	pcMCData  = (MCData *)malloc(sizeof(MCData)*iNumOfThreads);
	pSeeds     = (unsigned int *)malloc(sizeof(unsigned int)*iNumOfThreads*CLINE);

	//Collect data
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
		pcMCData[iTd].iWidth  = pcImageData->iWidth;
		pcMCData[iTd].iHeight = pcImageData->iHeight;
		pcMCData[iTd].iTd = iTd;
		pcMCData[iTd].iNumOfThreads = iNumOfThreads;
		pcMCData[iTd].pShare1 = pcShare1->imgData;
		pcMCData[iTd].pShare2 = pcShare2->imgData;
		pcMCData[iTd].pImgData = pcImageData->imgData;
		pSeeds[iTd] = time(NULL);
		pcMCData[iTd].pSeeds = &pSeeds[iTd*CLINE];
		pcMCData[iTd].iCodecPath = 0;
	}
		gettimeofday(&start,0);

	//Launch threads
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
		pthread_create(&(pcPthread[iTd]), NULL, PartialCodec, (void *)&pcMCData[iTd]);
	}


	//Wait all threads to finish
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
			pthread_join(pcPthread[iTd],NULL);
	}

	free(pcMCData);
	free(pcPthread);
	free(pSeeds);

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
	sprintf(pcShare1->imageName, "Share1M_%s",pcImageData->imageName); //adjust name
	sprintf(pcShare2->imageName, "Share2M_%s",pcImageData->imageName);
	pcShare1->cBmpIH.width  = pcShare1->iWidth; //adjust width
	pcShare2->cBmpIH.width  = pcShare2->iWidth;
	pcShare1->cBmpIH.height = pcShare1->iHeight;  //adjust height
	pcShare2->cBmpIH.height = pcShare2->iHeight;
	pcShare1->cBmpIH.biSizeImage = pcShare1->cBmpIH.height * (((pcShare1->cBmpIH.bitPix * pcShare1->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare2->cBmpIH.biSizeImage = pcShare2->cBmpIH.height * (((pcShare2->cBmpIH.bitPix * pcShare2->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare1->cBmpFH.bfSize = pcShare1->cBmpIH.biSizeImage + pcShare1->cBmpFH.bfOffBits;
	pcShare2->cBmpFH.bfSize = pcShare2->cBmpIH.biSizeImage + pcShare2->cBmpFH.bfOffBits;
}

void VCDecoderMC(ImageData *pcShare1, ImageData *pcShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR)
{

	int iNumOfThreads, iTd;
	pthread_t *pcPthread;
	MCData    *pcMCData;
	struct timeval start,stop;

	gettimeofday(&start,0);
	printf("MC Decoding ...\n");

	memcpy(pcImageData, pcShare1, sizeof(ImageData));
	pcImageData->imgData = (level *)malloc(pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
    memset(pcImageData->imgData,0, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	sprintf(pcImageData->imageName, "ReconM_%s",pInputImageName);


	// ********************************************************************************
	// DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
	// Number of threads goes in here.                                                *
	// ********************************************************************************

	iNumOfThreads = 16;

 	printf("|--Number of Multicore Decoding Threads: \%d \n",iNumOfThreads);

	// ********************************************************************************
	// End Number of threads                              							 							*
	// DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
	// ********************************************************************************

    printf("|--Number of Multi-Core Threads: %d\n",iNumOfThreads);

	pcPthread = (pthread_t *)malloc(sizeof(pthread_t)*iNumOfThreads);
	pcMCData  = (MCData *)malloc(sizeof(MCData)*iNumOfThreads);

	if((pcImageData->iHeight % iNumOfThreads)!=0)
	{
		printf("Warning: the height of the original image is not divisible by number of threads\n");
	}

	if((pcImageData->iWidth % iNumOfThreads)!=0)
	{
		printf("Warning: the width of the original image is not divisible by number of threads\n");
	}

	//Collect data
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
		pcMCData[iTd].iWidth  = pcImageData->iWidth;
		pcMCData[iTd].iHeight = pcImageData->iHeight;
		pcMCData[iTd].iTd = iTd;
		pcMCData[iTd].iNumOfThreads = iNumOfThreads;
		pcMCData[iTd].pShare1 = pcShare1->imgData;
		pcMCData[iTd].pShare2 = pcShare2->imgData;
		pcMCData[iTd].pImgData = pcImageData->imgData;
		pcMCData[iTd].iCodecPath = 1;
	}

	//Launch threads
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
		pthread_create(&(pcPthread[iTd]), NULL, PartialCodec, (void *)&pcMCData[iTd]);
	}


	//Wait all threads to finish
	for(iTd=0;iTd < iNumOfThreads;iTd++)
	{
		pthread_join(pcPthread[iTd],NULL);
	}

	free(pcMCData);
	free(pcPthread);

	gettimeofday(&stop,0);
	pTR->DecodeTime = ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

}
