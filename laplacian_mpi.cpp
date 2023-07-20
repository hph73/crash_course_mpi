#include <iostream>
#include <mpi.h>
#include <math.h>

// size of the plate
#define COLS 1000
#define ROWS 1000
#define NR 250
#define p0Cout if (my_PE_num == 0) std::cout
// largest permitted change in temp
#define MAX_TEMP_ERROR 0.01

double Temperature[NR+2][COLS+2]; // temperature grid
double Temperature_last[NR+2][COLS+2]; // temperature grid from last iteration

// help routines
void initialize(int numprocs, int my_PE_num);
void track_progress(int iter);

int main(int argc, char** argv) {
    int i, j;
    int max_iterations = 4000;;
    int iteration = 1;
    double dt = 100;
    double mydt;
    int numprocs, my_PE_num;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);

    // p0Cout << "Initializing..." << std::endl;
    initialize(numprocs, my_PE_num);
    // p0Cout << "numprocs=" << numprocs << std::endl;
    // p0Cout << "my_PE_num=" << my_PE_num << std::endl;

    // do until error is minimal or until max steps
    while (dt > MAX_TEMP_ERROR && iteration <= max_iterations) {
        p0Cout << "iteration=" << iteration << std::endl;
        // p0Cout << "Averaging..." << std::endl;
        for (i = 1; i <= NR; i++) {
            for (j = 1; j <= COLS; j++) {
                Temperature[i][j] = 0.25 * (Temperature_last[i+1][j] + Temperature_last[i-1][j] + 
                Temperature_last[i][j+1] + Temperature_last[i][j-1]);
            }
        }

        // p0Cout << "Calculating dt..." << std::endl;
        mydt = 0.0;
        for (i = 1; i <= NR; i++) {
            for (j = 1; j <= COLS; j++) {
                mydt = fmax(fabs(Temperature[i][j] - Temperature_last[i][j]), mydt);
                Temperature_last[i][j] = Temperature[i][j];
            }
        }
        // p0Cout << "Reducing..." << std::endl;
        MPI_Reduce(&mydt, &dt, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        // Broadcasting is important; otherwise other ranks will hang
        MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        p0Cout << "dt=" << dt << std::endl;

        // p0Cout << "Sending and receiving..." << std::endl;
        if (my_PE_num > 0) {
            MPI_Send(Temperature[1], COLS+2, MPI_DOUBLE, my_PE_num-1, 10, MPI_COMM_WORLD);
        }
        if (my_PE_num < numprocs-1) {
            MPI_Recv(Temperature_last[NR+1], COLS+2, MPI_DOUBLE, my_PE_num + 1, 10, MPI_COMM_WORLD, &status);
        }
        if (my_PE_num < numprocs-1) {
            MPI_Send(Temperature[NR], COLS+2, MPI_DOUBLE, my_PE_num+1, 10, MPI_COMM_WORLD);
        }
        if (my_PE_num > 0) {
            MPI_Recv(Temperature_last[0], COLS+2, MPI_DOUBLE, my_PE_num-1, 10, MPI_COMM_WORLD, &status);
        }
        // if (my_PE_num == 0 && (iteration % 100) == 0) track_progress(iteration);
        iteration++;
    }
    p0Cout << "iteration at end: " << iteration << std::endl;

    MPI_Finalize();
}



void initialize(int numprocs, int my_PE_num) {
    // initialize temperature
    int i, j;
    for (i = 0; i <= NR + 1; i++) {
        for (j = 0; j <= COLS+1; j++) {
            Temperature_last[i][j] = 0.0;
        }
    }

    // set boundary conditions; BC nerver changes in run
    if (my_PE_num == 0) {
        for (j = 0; j <= COLS+1; j++) {
            Temperature_last[0][j] = 0.0;
        }
    }
    if (my_PE_num == numprocs - 1) {
        for (j = 0; j <= COLS+1; j++) {
            Temperature_last[NR+1][j] = 100.0 * j / COLS;
        }
    }
    for (i = 0; i <= NR+1; i++) {
        Temperature_last[i][0] = 0.0;
        Temperature_last[i][COLS+1] = 100.0 * (NR * my_PE_num + i) / ROWS;
    }
}

void track_progress(int iteration) {
    int i;
    std::cout << "-- Iteration: " << iteration  << " --" << std::endl;
    for (i = ROWS-5; i <= ROWS; i++) {
        std::cout << "Temp[i][i]=" << Temperature[i][i] << std::endl;
    }
}