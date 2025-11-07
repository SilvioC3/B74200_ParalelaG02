/**
 *  Producto de una matriz por un vector en paralelo (version OpenMP)
 *  Compilar con:
 *     make omp_mat_vec.out
 *  Ejecutar con:
 *     ./
 *  [DEBUG]: Compilar con: 
 *     g++ -fopenmp -DDEBUG omp_mat_vec.cc -o omp_mat_vec.out
 *  [DEBUG]: Ejecutar con:
 *     ./
 */

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

using namespace std;

