#include <mpi.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  __int32_t rank, np;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  double start_time, finish_time, proc_time, answer_time;
  std::fstream in_a{argv[1], std::fstream::in | std::fstream::binary},
      in_b{argv[2], std::fstream::in | std::fstream::binary};

  __int32_t n;
  in_a.read(reinterpret_cast<char*>(&n), sizeof(__int32_t));
  in_b.read(reinterpret_cast<char*>(&n), sizeof(__int32_t));

  __int32_t rows = n / np;
  __int32_t elements_proc = n * n / np;
  __int32_t offset = rank * elements_proc;
  in_a.seekg(offset * sizeof(__int32_t), in_a.cur);
  __int32_t* a = new __int32_t[elements_proc];
  __int32_t* b = new __int32_t[n];
  __int32_t* c = new __int32_t[n];
  in_a.read(reinterpret_cast<char*>(a), elements_proc * sizeof(__int32_t));
  in_b.read(reinterpret_cast<char*>(b), n * sizeof(__int32_t));

  start_time = MPI_Wtime();
  __int32_t* proc_c = new __int32_t[rows];

  for (__int32_t i = 0; i < rows; ++i) {
    __int32_t sum = 0;
    for (__int32_t j = 0; j < n; ++j)
      sum += a[i * n + j] * b[j];
    proc_c[i] = sum;
  }

  finish_time = MPI_Wtime();
  proc_time = finish_time - start_time;
  MPI_Reduce(&proc_time, &answer_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Gather(proc_c, rows, MPI_INT32_T, c, rows,
             MPI_INT32_T, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    std::cout << "Elapsed time = " << answer_time << std::endl;
    std::fstream out_c{argv[3], std::fstream::out | std::fstream::binary};
    out_c.write(reinterpret_cast<char*>(&n), sizeof(__int32_t));
    out_c.write(reinterpret_cast<char*>(c), n * sizeof(__int32_t));
  }

  delete[] proc_c;
  delete[] a;
  delete[] b;
  delete[] c;
  MPI_Finalize();
  return 0;
}