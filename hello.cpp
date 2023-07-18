#include <iostream>
#include <mpi.h>
#include <math.h>

// // Example 1: hello
// int main(int argc, char** argv) {
//     int my_PE_num;
//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);
//     std::cout << "Hello from " << my_PE_num << std::endl;
//     MPI_Finalize();
// }

// // Example2: send and receive messages
// int main(int argc, char** argv) {
//     int my_PE_num, numberToReceive, numberToSend=42;
//     MPI_Status status;

//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);
//     std::cout << "Hello from " << my_PE_num << std::endl;
//     if (my_PE_num == 0) {
//         MPI_Recv(&numberToReceive, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//         std::cout << "Number received is " << numberToReceive << std::endl;
//     } else MPI_Send(&numberToSend, 1, MPI_INT, 0, 10, MPI_COMM_WORLD);
//     MPI_Finalize();
// }

// // Example 3: Synchronization
// int main(int argc, char** argv) {
//     int my_PE_num, numberToReceive, numberToSend=4, index, result=0;
//     MPI_Status status;

//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);
//     if (my_PE_num == 0) {
//         for (index=1; index<4; index++) {
//             MPI_Send(&numberToSend, 1, MPI_INT, index, 10, MPI_COMM_WORLD);
//         }
//     } else {
//         MPI_Recv(&numberToReceive, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
//         std::cout <<"my_PE_num " << my_PE_num << " received is " << numberToReceive << std::endl;
//         result = numberToReceive * my_PE_num;
//     }

//     for (index=1; index<4; index++) {
//         MPI_Barrier(MPI_COMM_WORLD);
//         if (index==my_PE_num) std::cout << "PE " << my_PE_num << "'s result is " << result << std::endl;
//     }

//     if (my_PE_num==0) {
//         for (index=1; index<4; index++) {
//             MPI_Recv(&numberToReceive, 1, MPI_INT, index, 10, MPI_COMM_WORLD, &status);
//             result += numberToReceive;
//         }
//         std::cout << "Total is " << result << std::endl;
//     } else MPI_Send(&result, 1, MPI_INT, 0, 10, MPI_COMM_WORLD);

//     MPI_Finalize();
// }

// Example 4: Compute PI
int main(int argc, char** argv) {
    int n, my_PE_num, numprocs, index;
    float mypi, pi, h, x, start, end;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);

    if (my_PE_num == 0) {
        std::cout << "How many intervals?" << std::endl;
        std::cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    mypi = 0;
    h = (float)2 / n; 
    start = (my_PE_num * 2 / numprocs) - 1;
    end = (my_PE_num + 1) * 2 / numprocs - 1;

    for (x = start; x < end; x = x + h)
        mypi = mypi + h * 2 * sqrt(1 - x * x);
    
    MPI_Reduce(&mypi, &pi, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_PE_num == 0) {
        std::cout << "Pi is approximately " << pi << std::endl;
    }
    
    MPI_Finalize();
}