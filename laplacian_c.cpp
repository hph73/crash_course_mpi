#include <iostream>
#include <mpi.h>
#include <math.h>

// size of the plate
#define COLS 1000
#define ROWS 1000

// largest permitted change in temp
#define MAX_TEMP_ERROR 0.01

double Temperature[ROWS+2][COLS+2]; // temperature grid
double Temperature_last[ROWS+2][COLS+2]; // temperature grid from last iteration

// help routines
void initialize();
void track_progress(int iter);

int main(int argc, char** argv) {
    int i, j;
    int max_iterations = 4000;;
    int iteration = 1;
    double dt = 100;

    initialize();
    // do until error is minimal or until max steps
    while (dt > MAX_TEMP_ERROR && iteration <= max_iterations) {
        for (i = 1; i <= ROWS; i++) {
            for (j = 1; j <= COLS; j++) {
                Temperature[i][j] = 0.25 * (Temperature_last[i+1][j] + Temperature_last[i-1][j] + 
                Temperature_last[i][j+1] + Temperature_last[i][j-1]);
            }
        }

        dt = 0.0;
        for (i = 1; i <= ROWS; i++) {
            for (j = 1; j <= COLS; j++) {
                dt = fmax(fabs(Temperature[i][j] - Temperature_last[i][j]), dt);
                Temperature_last[i][j] = Temperature[i][j];
            }
        }
        // if ((iteration % 100) == 0) track_progress(iteration);
        if (iteration <= 10) {
            std::cout << "dt=" << dt << std::endl;
        }
        iteration++;
    }
    std::cout << "iteration at end: " << iteration << std::endl;
}



void initialize() {
    // initialize temperature
    int i, j;
    for (i = 0; i <= ROWS+1; i++) {
        for (j = 0; j <= COLS+1; j++) {
            Temperature_last[i][j] = 0.0;
        }
    }

    // set boundary conditions; BC nerver changes in run
    for (i = 0; i <= ROWS+1; i++) {
        Temperature_last[i][0] = 0.0;
        Temperature_last[i][COLS+1] = 100.0 * i / ROWS;
    }
    for (j = 0; j <= COLS+1; j++) {
        Temperature_last[0][j] = 0.0;
        Temperature_last[ROWS+1][j] = 100.0 * j / COLS;
    }
}

void track_progress(int iteration) {
    int i;
    std::cout << "-- Iteration: " << iteration  << " --" << std::endl;
    for (i = ROWS-5; i <= ROWS; i++) {
        std::cout << "Temp[i][i]=" << Temperature[i][i] << std::endl;
    }
}