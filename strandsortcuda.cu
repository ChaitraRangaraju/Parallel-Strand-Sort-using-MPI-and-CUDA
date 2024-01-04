%%writefile cuda_example2.cu
#include <iostream>
#include <fstream>
#include <vector>
#include <cuda_runtime.h>

#define SIZE 1010228

__device__ void strandSort(int* ip, int* op, int size) {
    int tid = threadIdx.x;
    int step = blockDim.x;

    for (int i = tid; i < size; i += step) {
        int current = ip[i];
        int j = i - 1;

        // Move elements greater than current to the right
        while (j >= 0 && op[j] > current) {
            op[j + 1] = op[j];
            j--;
        }

        // Insert the current element in the correct position
        op[j + 1] = current;
    }
}

__global__ void kernelStrandSort(int* ip, int* op, int size) {
    extern __shared__ int sublist[];

    int tid = threadIdx.x;

    for (int i = 0; i < size; ++i) {
        sublist[tid] = ip[i];
        __syncthreads();  // Synchronize threads after updating sublist

        strandSort(sublist, op, i + 1);

        // Wait for all threads to finish sorting before updating sublist
        __syncthreads();
    }
}

int main() {
    // Read input from a file
    std::ifstream inputFile("/content/drive/MyDrive/T10I4D100K.dat.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the input file." << std::endl;
        return 1;
    }

    std::vector<int> inputVector;
    int inputValue;
    while (inputFile >> inputValue) {
        inputVector.push_back(inputValue);
    }
    if (inputVector.size() != SIZE) {
        std::cerr << "Input size does not match the defined size (" << SIZE << ")." << std::endl;
        return 1;
    }

    int* ip;
    cudaMallocManaged(&ip, SIZE * sizeof(int));
    for (int i = 0; i < SIZE; ++i) {
        ip[i] = inputVector[i];
    }

    int* op;
    cudaMallocManaged(&op, SIZE * sizeof(int));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    int blockSize = 256;
    int gridSize = (SIZE + blockSize - 1) / blockSize;

    kernelStrandSort<<<blockSize, gridSize, SIZE * sizeof(int)>>>(ip, op, SIZE);
    cudaDeviceSynchronize();

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float seconds = 0;
    cudaEventElapsedTime(&seconds, start, stop);


    std::cout << "Time taken by the kernel: " << seconds << " s" << std::endl;

    cudaFree(ip);
    cudaFree(op);

    return 0;
}

