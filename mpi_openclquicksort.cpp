#include <mpi.h>
#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

void quickSort(std::vector<int> &data, int left, int right) {
    int pivot, i, j;
    if(left < right) {
        pivot = left;
        i = left;
        j = right;
        while(i < j) {
            while(data[i] <= data[pivot] && i < right)
                i++;
            while(data[j] > data[pivot])
                j--;
            if(i < j){
                std::swap(data[i], data[j]);
            }
        }
        std::swap(data[pivot], data[j]);
        quickSort(data, left, j-1);
        quickSort(data, j+1, right);
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int n = 8; // size of the array
    std::vector<int> A(n);

    std::generate(A.begin(), A.end(), [](){ return rand() % 100; });

    double start_time = MPI_Wtime();

    quickSort(A, 0, n-1);

    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    if(rank == 0)
        std::cout << "Elapsed time: " << elapsed_time << " seconds" << std::endl;

    MPI_Finalize();
    return 0;
}