all: VisualCryptography

VisualCryptography: VisualCryptographyGPU.o VisualCryptographyMC.o ExtVisualCryptographyCPU.o ExtVisualCryptographyGPU.o ExtVisualCryptographyMC.o VisualCryptographyCPUTest.o
	g++ *.o  -L/usr/local/cuda/lib64 -lcudart -lpthread ./lib/libVC.a -o VisualCryptography
	rm -rf *.o

VisualCryptographyMC.o: ./source/VisualCryptographyMC.c
	g++ -lpthread -c ./source/VisualCryptographyMC.c

VisualCryptographyCPUTest.o: ./source/VisualCryptographyCPUTest.c
	g++ -lpthread -c ./source/VisualCryptographyCPUTest.c

VisualCryptographyGPU.o: ./source/VisualCryptographyGPU.cu
	/usr/local/cuda/bin/nvcc -c -arch=compute_20 -code=sm_20 -lm ./source/VisualCryptographyGPU.cu

ExtVisualCryptographyCPU.o: ./source/ExtVisualCryptographyCPU.c
	g++ -c ./source/ExtVisualCryptographyCPU.c

ExtVisualCryptographyGPU.o: ./source/ExtVisualCryptographyGPU.cu
	/usr/local/cuda/bin/nvcc -c -arch=compute_20 -code=sm_20 -lm ./source/ExtVisualCryptographyGPU.cu

ExtVisualCryptographyMC.o: ./source/ExtVisualCryptographyMC.c
	g++ -lpthread -c ./source/ExtVisualCryptographyMC.c

clean:
	rm -rf *.o VisualCryptography

makelib: main.o preprocess.o VisualCryptographyCPU.o
	ar rcs libVC.a main.o preprocess.o VisualCryptographyCPU.o
	rm -rf *.o


