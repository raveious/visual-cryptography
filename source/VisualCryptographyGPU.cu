
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <curand_kernel.h>
#include <curand.h>
#include "common.h"
#include "VisualCryptographyGPU.h"

#define ENCODE_TILE_SIZE 512
#define DECODE_TILE_SIZE 256

__constant__ level WhiteShare1[2][2] = {0,0,1,1};
__constant__ level WhiteShare2[2][2] = {0,0,1,1};

__constant__ level BlackShare1[2][2] = {0,0,1,1};
__constant__ level BlackShare2[2][2] = {1,1,0,0};

void CheckCUDAError(const char *msg) {
  cudaError_t code = cudaGetLastError();
  if(code!=cudaSuccess)
  {
      fprintf(stderr,"Cuda Error: %s: %s.\n",msg,cudaGetErrorString(code));
      exit(EXIT_FAILURE);
  }
}

__global__ void CodecKernel(level *pImage_d, level *pShare1_d, level *pShare2_d, int iWidth, int iHeight, int iCodecPath) {
  // ********************************************************************************
  // DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
  // Your CUDA Kernel should go here.                                               *
  // ********************************************************************************

  // ********************************************************************************
  // 1) If iCodecPath=ENCODE, the code performs encoding. In that case              *
  //    input image of size (iHeight x iWidth) pixels of (type level) stored at GPU *
  //    address pImage_d. The share images of size (2*iHeight x 2*iWidth) are stored*
  //    at GPU addresses pShare1_d and pShare2_d.                                   *
  // ********************************************************************************
  if (iCodecPath == ENCODE) {
    int offset = (iWidth * ENCODE_TILE_SIZE * blockIdx.y) + (blockDim.x * blockIdx.x) + threadIdx.x;
    int invert = 0;
    int share_offset = 0;
    int alt_share_offset = 0;
    int temp = 0;
    curandState_t state;
    curand_init(offset, 0, 0, &state);

    // loop over this threads segment
    for (int i = 0; i < ENCODE_TILE_SIZE * iWidth; i += iWidth) {
      invert = curand(&state) % 2;
      temp = i + offset;

      share_offset = (temp * 2) + ((temp / iWidth) * (iWidth * 2));
      alt_share_offset = share_offset + (iWidth * 2);

      // Select White or Black shares
      if (pImage_d[temp]) {
        pShare1_d[share_offset]         = WhiteShare1[0][0] ^ invert;
        pShare1_d[share_offset + 1]     = WhiteShare1[0][1] ^ invert;
        pShare1_d[alt_share_offset]     = WhiteShare1[1][0] ^ invert;
        pShare1_d[alt_share_offset + 1] = WhiteShare1[1][1] ^ invert;

        pShare2_d[share_offset]         = WhiteShare2[0][0] ^ invert;
        pShare2_d[share_offset + 1]     = WhiteShare2[0][1] ^ invert;
        pShare2_d[alt_share_offset]     = WhiteShare2[1][0] ^ invert;
        pShare2_d[alt_share_offset + 1] = WhiteShare2[1][1] ^ invert;
      } else {
        pShare1_d[share_offset]         = BlackShare1[0][0] ^ invert;
        pShare1_d[share_offset + 1]     = BlackShare1[0][1] ^ invert;
        pShare1_d[alt_share_offset]     = BlackShare1[1][0] ^ invert;
        pShare1_d[alt_share_offset + 1] = BlackShare1[1][1] ^ invert;

        pShare2_d[share_offset]         = BlackShare2[0][0] ^ invert;
        pShare2_d[share_offset + 1]     = BlackShare2[0][1] ^ invert;
        pShare2_d[alt_share_offset]     = BlackShare2[1][0] ^ invert;
        pShare2_d[alt_share_offset + 1] = BlackShare2[1][1] ^ invert;
      }
    }
  }
  // ********************************************************************************
  // 2) If iCodecPath=DECODE, the code performs decoding. In that case              *
  //    output image of size (iHeight x iWidth) pixels of (type level) is stored    *
  //    at GPU address pImage_d. The share images of size (iHeight x iWidth) are    *
  //    stored at GPU addresses pShare1_d and pShare2_d.                            *
  // ********************************************************************************
  else {
    int offset = (iWidth * DECODE_TILE_SIZE * blockIdx.y) + (blockDim.x * blockIdx.x) + threadIdx.x;
    int temp = 0;

    for (int i = 0; i < DECODE_TILE_SIZE * iWidth; i += iWidth) {
      temp = offset + i;
      // Safety stuff so threads don't run off the end
      if (temp < iHeight * iWidth) {
        // Take logical and of all the pixels to generate defined response
        pImage_d[temp] = pShare1_d[temp] & pShare2_d[temp];
      }

			// NOTE: The suppied solution uses logical NOR operation instead of
			// logical AND, but this doesn't yield the same result as what the
			// project description describes
    }
  }
  // ********************************************************************************
  // Your CUDA code ends here.                                                      *
  // DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
  // ********************************************************************************
}

void VCEncoderGPU(ImageData *pcShare1, ImageData *pcShare2, ImageData *pcImageData, TimeRecord *pTR)
{
 	level *pShare1, *pShare2;
	level *pShare1_d, *pShare2_d;
	level *pImage_d;
	struct timeval start,stop;
	int blockSizeX, blockSizeY, gridSizeX, gridSizeY;

	printf("GPU Encoding... \n");

	//----------Fill in shares----------//
	pcShare1->iWidth  = 2 * pcImageData->iWidth;
	pcShare1->iHeight = 2 * pcImageData->iHeight;
	pcShare2->iWidth  = 2 * pcImageData->iWidth;
	pcShare2->iHeight = 2 * pcImageData->iHeight;
    cudaDeviceReset();
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
	sprintf(pcShare1->imageName, "Share1G_%s",pcImageData->imageName); //adjust name
	sprintf(pcShare2->imageName, "Share2G_%s",pcImageData->imageName);
	pcShare1->cBmpIH.width  = pcShare1->iWidth; //adjust width
	pcShare2->cBmpIH.width  = pcShare2->iWidth;
	pcShare1->cBmpIH.height = pcShare1->iHeight;  //adjust height
	pcShare2->cBmpIH.height = pcShare2->iHeight;
	pcShare1->cBmpIH.biSizeImage = pcShare1->cBmpIH.height * (((pcShare1->cBmpIH.bitPix * pcShare1->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare2->cBmpIH.biSizeImage = pcShare2->cBmpIH.height * (((pcShare2->cBmpIH.bitPix * pcShare2->cBmpIH.width + 31) / 32) * 4); //adjust image size
	pcShare1->cBmpFH.bfSize = pcShare1->cBmpIH.biSizeImage + pcShare1->cBmpFH.bfOffBits;
	pcShare2->cBmpFH.bfSize = pcShare2->cBmpIH.biSizeImage + pcShare2->cBmpFH.bfOffBits;

	//------Generate shares pixels---------//
	pcShare1->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	pcShare2->imgData = (level *)malloc(4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	pShare1 = pcShare1->imgData;
	pShare2 = pcShare2->imgData;

	gettimeofday(&start,0);

	//------GPU Memory Preparation-------//
	cudaMalloc( (void**)&pImage_d, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Original Image GPU Memory Allocation Failed");
	cudaMalloc( (void**)&pShare1_d, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share1 GPU Memory Allocation Failed");
	cudaMemset(pShare1_d, 0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share1 Image GPU Memory Set Failed");
	cudaMalloc( (void**)&pShare2_d, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share2 GPU Memory Allocation Failed");
	cudaMemset(pShare2_d, 0, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share2 Image GPU Memory Set Failed");

	//-------Transfer orignal image-------//
	cudaMemcpy(pImage_d, pcImageData->imgData, pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyHostToDevice);
	CheckCUDAError("Copy Original Image to GPU Failed");

	gettimeofday(&stop,0);
	pTR->MemTransferTime += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	gettimeofday(&start,0);
	//-----GPU Kernel Launch-----//
  // ********************************************************************************
  // DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
  // Your CUDA block size and grid size parameters go in here.                      *
  // ********************************************************************************
	//Fill in here

  // Each block is going to have ENCODE_TILE_SIZE threads that will run in seq. mem. blocks,
  // then do the line underneath that for ENCODE_TILE_SIZE lines.
	blockSizeX = ENCODE_TILE_SIZE;
  blockSizeY = 1;

  // Note: This means that the smallest image that can be decoded is
  // ENCODE_TILE_SIZE x ENCODE_TILE_SIZE
	gridSizeX  = pcImageData->iWidth / ENCODE_TILE_SIZE;
	gridSizeY  = pcImageData->iHeight / ENCODE_TILE_SIZE;

  if (gridSizeX < 1)
    gridSizeX = 1;

  if (gridSizeY < 1)
    gridSizeY = 1;
  // ********************************************************************************
  // End of CUDA block size and grid size parameters                                *
  // DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
  // ********************************************************************************

	printf("|--Block Config: %d x %d\n",blockSizeX,blockSizeY);
	printf("|--Grid  Config: %d x %d\n",gridSizeX,gridSizeY);
	dim3 blocksInGrid(gridSizeX,gridSizeY);
	dim3 threadsInBlock(blockSizeX,blockSizeY);
	CodecKernel<<<blocksInGrid, threadsInBlock>>>(pImage_d,pShare1_d,pShare2_d,pcImageData->iWidth, pcImageData->iHeight, ENCODE);
	cudaDeviceSynchronize();
	CheckCUDAError("Encryption Kernel Failed");

	gettimeofday(&stop,0);
	pTR->EncryptionTime += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	gettimeofday(&start,0);

	//------Transfer back shares------//
	cudaMemcpy(pShare1,pShare1_d, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyDeviceToHost);
	CheckCUDAError("Copy Share1 to CPU Failed");
	cudaMemcpy(pShare2,pShare2_d, 4 * pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyDeviceToHost);
	CheckCUDAError("Copy Share2 to CPU Failed");

	gettimeofday(&stop,0);
	pTR->MemTransferTime += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	cudaFree(pImage_d);
	cudaFree(pShare1_d);
	cudaFree(pShare2_d);
}

void VCDecoderGPU(ImageData *pcShare1, ImageData *pcShare2, char *pInputImageName, ImageData *pcImageData, TimeRecord *pTR)
{
 	level *pShare1, *pShare2;
	level *pShare1_d, *pShare2_d;
	level *pImage_d;
	struct timeval start,stop;
	int blockSizeX, blockSizeY, gridSizeX, gridSizeY;

	printf("GPU Decoding ...\n");
	pShare1 = pcShare1->imgData;
	pShare2 = pcShare2->imgData;
    cudaDeviceReset();

	//------GPU Memory Preparation-------//
	memcpy(pcImageData, pcShare1, sizeof(ImageData));
	pcImageData->imgData = (level *)malloc(pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	sprintf(pcImageData->imageName, "ReconG_%s",pInputImageName);
	//Memory Allocation
	cudaMalloc( (void**)&pImage_d, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Reconstructed Image GPU Memory Allocation Failed");
	cudaMemset(pImage_d, 0, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Reconstructed Image GPU Memory Set Failed");
	cudaMalloc( (void**)&pShare1_d, pcImageData->iHeight *  pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share1 GPU Memory Allocation Failed");
	cudaMalloc( (void**)&pShare2_d, pcImageData->iHeight * pcImageData->iWidth * sizeof(level));
	CheckCUDAError("Share2 GPU Memory Allocation Failed");

	//Transfer shares
	gettimeofday(&start,0);
	cudaMemcpy(pShare1_d, pShare1, pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyHostToDevice);
	CheckCUDAError("Copy Share1 to GPU Failed");
	cudaMemcpy(pShare2_d, pShare2, pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyHostToDevice);
	CheckCUDAError("Copy Share2 to GPU Failed");
	gettimeofday(&stop,0);
	pTR->MemTransferTimeDecode += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	//----------GPU Kernel Launch----------//
	gettimeofday(&start,0);
  // ********************************************************************************
  // DO NOT CHANGE ANYTHING BEFORE THIS POINT in YOUR CODE                          *
  // Your CUDA block size and grid size parameters go in here.                      *
  // ********************************************************************************
	//Fill in here

  // Each block is going to have DECODE_TILE_SIZE threads that will run in seq. mem. blocks,
  // then do the line underneath that for DECODE_TILE_SIZE lines.
	blockSizeX = DECODE_TILE_SIZE;
  blockSizeY = 1;

  // Note: This means that the smallest image that can be decoded is
  // DECODE_TILE_SIZE x DECODE_TILE_SIZE
	gridSizeX  = pcImageData->iWidth / DECODE_TILE_SIZE;
	gridSizeY  = pcImageData->iHeight / DECODE_TILE_SIZE;

  if (gridSizeX < 1)
    gridSizeX = 1;

  if (gridSizeY < 1)
    gridSizeY = 1;
  // ********************************************************************************
  // End of CUDA block size and grid size parameters                                *
  // DO NOT CHANGE ANYTHING AFTER THIS POINT in YOUR CODE                           *
  // ********************************************************************************
	printf("|--Block Config: %d x %d\n",blockSizeX,blockSizeY);
	printf("|--Grid  Config: %d x %d\n",gridSizeX,gridSizeY);
	dim3 blocksInGrid(gridSizeX,gridSizeY);
	dim3 threadsInBlock(blockSizeX,blockSizeY);
	CodecKernel<<<blocksInGrid, threadsInBlock>>>(pImage_d,pShare1_d,pShare2_d,pcImageData->iWidth, pcImageData->iHeight, DECODE);
	cudaDeviceSynchronize();
	CheckCUDAError("Decryption Kernel Failed");
	gettimeofday(&stop,0);
	pTR->DecodeTime += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	//------Transfer back reconstructed image------//
	gettimeofday(&start,0);
	cudaMemcpy(pcImageData->imgData,pImage_d, pcImageData->iHeight * pcImageData->iWidth * sizeof(level), cudaMemcpyDeviceToHost);
	CheckCUDAError("Copy Reconstructed image to CPU Failed");
	gettimeofday(&stop,0);
	pTR->MemTransferTimeDecode += ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000;

	//----------Free memory----------//
 	cudaFree(pImage_d);
	cudaFree(pShare1_d);
	cudaFree(pShare2_d);

}
