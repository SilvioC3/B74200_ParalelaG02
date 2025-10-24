/**
 * Represent a shopping cart
 *
 * @author	Programacion Concurrente
 * @version	2025/Oct/10
 *
 * Primer examen parcial
 *
 * Grupo 3
 *
**/

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "tienda.h"

#define K 10

long total;

/*
 *
 */
void startTimer( struct timeval * timerStart) {
   gettimeofday( timerStart, NULL );
}


/*
 *  time elapsed in ms
 */
double getTimer( struct timeval timerStart ) {
   struct timeval timerStop, timerElapsed;

   gettimeofday(&timerStop, NULL);
   timersub(&timerStop, &timerStart, &timerElapsed);
   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}



int main() {
   int items = 6;
   int capacidadCarrito = K;
   int v[] = { 20, 5, 10, 40, 15, 25 };
   int w[] = { 1,  2,  3,  8,  7,  4 };
   Tienda * t = new Tienda( capacidadCarrito, items, w, v );
   Tienda * t1 = new Tienda( 45, 100 );

    clock_t start, finish;
    struct timeval timerStart;
    double used, wused;

   std::cout << t->toString();
   std::cout << t1->toString();


    startTimer( & timerStart );
    start = clock();
    total = 0;
   t->llenarCarrito();		// Encuentra la mayor compra sin sobrepasar el límite de peso
    finish = clock();
    used = ((double) (finish - start)) / CLOCKS_PER_SEC;
    wused = getTimer( timerStart );
    printf( "\nSerial version wall time %g \n", wused );

   return 0;

}