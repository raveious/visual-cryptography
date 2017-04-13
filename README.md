# Basic Visual Cryptography
MTU - EE/CS5496 - GPU and Multi-core Programming - Visual Cryptography Project

## Full Compilation (Update the executable):

1. Make sure the current directory contains "Makefile", "lib" folder, and "source" folder.
2. To compile your code, simply type "make" (all lowercase letters). Makefile automatically
   detects changes to the source files and compiles only the modified source files.
3. The compilation will generate a single executable called "VisualCryptography".

## Partial Compilation (DO NOT update executable)

This will be useful for checking the syntax of your codes in "VisualCryptographyGPU.cu" and
   "VisualCryptographyMC.c" files.
1. Make sure "VisualCryptographyGPU.cu" is in "source" folder.
2. Make sure the current directory contains "source" folder.
3. For the compilation of your cuda code type
   "/usr/local/cuda-5.5/bin/nvcc -c -arch=compute_20 -code=sm_20 -lm ./source/VisualCryptographyGPU.cu".
   For the compilation of your multicore c code type
   "gcc  VisualCryptographyMC.c -c -lpthread" or "g++  VisualCryptographyMC.c -c -lpthread"

## Execution

1. The usage for the executable is "./VisualCryptography" [IMAGEFILES] [OPTIONS] Space separated Options are "(E)ncode (D)code, (C)PUTest (CPUD(F)AULT (G)PU (M)ulticore". A minimum of one option is needed. The order of options does not matter. The [IMAGESFILES] should be on the same directory that the VisualCryptography  executable resides. The number of files is one/(two) for encoding/(decoding).
2. For encryption only, type "./VisualCryptography [IMAGENAME].bmp (E)ncode (C)PUTest CPUDE(F)AULT (G)PU (M)ulticore". E option enables encoding on the CPU. "G" and/or "M" options enable the encoding on the GPU and/or multicore CPU.
3. For decryption only, type "./VisualCryptography Share1_[IMAGENAME].bmp Share2_[IMAGENAME].bmp (D)ncode (C)PUTest CPUDE(F)AULT (G)PU (M)ulticore".
   (Note: The share names must be in the form ShareN_[IMAGENAME]. The prefix "Share" and
   the `_` symbol are necessary. N must be single digit and usually is 1/2. It is
   recommended that two shares share the same name for the [IMAGENAME].)
4. For encryption followed by decryption, type
   "./VisualCryptography [IMAGENAME].bmp (E)ncode (D)ecode (C)PUTest CPUD(F)AULT (G)PU (M)ulticore".


## Makefile Mechanism (INFORMATIVE):

- "Makefile" compiles code and generates executable in three steps.
 1. Your GPU code inside "VisualCryptography.cu" is compiled and an object file is generated.
 2. The object file is linked with the pre built object files in the "lib/libVC.a" library file.
 3. Completion of Step #2 generates an executable called "VisualCryptography".
- The static library "libVC.a" contains "main.o" "VisualCryptographyCPU.o" and "preprocess.o".
  The function prototypes for "preprocess.c" are inside "/sourc/preprocess.h"

## Extension
1. There are three files with the prefix "Ext" that contain the empty functions that you can use for
   the extra work part of your project.
2. To run these programs you need to use options "S" along with the other options
   You will also need to specify three image file names.
