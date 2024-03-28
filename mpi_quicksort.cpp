#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#define N 100000

// Function to swap two elements in an array
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i+1], &arr[high]);

    return i+1;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi-1);
        quicksort(arr, pi+1, high);
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int arr[N];
    srand(time(NULL));

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % 1000;
        }
    }

    MPI_Bcast(arr, N, MPI_INT, 0, MPI_COMM_WORLD);

    int chunksize = N / size;
    int buffer[chunksize];
    MPI_Scatter(arr, chunksize, MPI_INT, buffer, chunksize, MPI_INT, 0, MPI_COMM_WORLD);

    quicksort(buffer, 0, chunksize-1);

    MPI_Gather(buffer, chunksize, MPI_INT, arr, chunksize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int temp[N];
        int i, j, k;

        for (i = 0; i < size; i++) {
            for (j = 0; j < chunksize; j++) {
                temp[i*chunksize+j] = arr[i+j*size];
            }
        }

        for (k = 0, i = 0, j = chunksize; k < N; k++) {
            if (i == chunksize) {
                arr[k] = temp[j++];
            } else if (j == 2*chunksize) {
                arr[k] = temp[i++];
            } else {
                arr[k] = (temp[i] < temp[j]) ? temp[i++] : temp[j++];
            }
        }
    }

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();

    return 0;
}