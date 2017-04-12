#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <assert.h>

#define BLACK 0
#define WHITE 1

#define ENCODE 0
#define DECODE 1

typedef unsigned char level;


typedef union FourByte{
	uint32_t ui32_;
	unsigned char uc4_[4]; 
}FourByte;
		

typedef struct TimeRecord{
	double MemTransferTime;
	double MemTransferTimeDecode;
	double EncryptionTime;
	double DecodeTime;
}TimeRecord;


//-----------Bitmap format content starts-----------//

typedef struct __attribute__((__packed__)) 
{                                                                                                                                                                                                                             
    uint16_t bfType;
    uint32_t bfSize;                                                                                                                                                                                                                
    uint16_t unused1;                                                                                                                                                                                                                        
    uint16_t unused2;                                                                                                                                                                                                                        
    uint32_t bfOffBits;                                                                                                                                                            
} BitMapFileHeader;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) 
{                                                                                                                                                                                                                             
    unsigned int   biSize;                                                                                                                                                                                                                   
    int            width;                                                                                                                                                                
    int            height;                                                                                                                                                                     
    uint16_t 	   planes;                                                                                                                                                                                                                         
    uint16_t       bitPix;                                                                                                                                                                                                                         
    unsigned int   biCompression;                                                                                                                                                                                                            
    unsigned int   biSizeImage;                                                                                                                                                                                                              
    int            biXPelsPerMeter;                                                                                                                                                                                                          
    int            biYPelsPerMeter;                                                                                                                                                                                                          
    unsigned int   biClrUsed;                                                                                                                                                                                                                
    unsigned int   biClrImportant;                                                                                                                                                                                                           
} BitMapInfoHeader;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) 
{                                                                                                                                                                                                                             
    unsigned char  b;                                                                                                                                                                                                                        
    unsigned char  g;                                                                                                                                                                                                                        
    unsigned char  r;   
    unsigned char  rgbReserved;                                                                                                                                                                                                                     
} BitMapImage;
//-----------Bitmap format content ends-----------//

//Parsed image data
typedef struct ImageData
{
	BitMapFileHeader cBmpFH;
	BitMapInfoHeader cBmpIH;
	BitMapImage		 cBmpImage[2];
	int iWidth;
	int iHeight;
	level *imgData;
	level *imgDataPadForBmp;
	int isPacked;
	char imageName[50];
}ImageData;


#endif

 
