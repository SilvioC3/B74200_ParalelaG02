/**
 *  Programa base para la construcción de centros en una muestra de datos
 *  La muestra de datos está representada por la variable "puntos", todos los puntos generados al azar
 *  Los centros están representados por la variable "centros", todos los puntos colocados en el origen
 *
 *  CI0117 Programación paralela y concurrente
 *  Tercera tarea programada, grupos 2 y 3
 *  2020-ii
 *
**/

#include <cstdio>
#include "VectorPuntos.h"
#include <cstring>
#include <omp.h>
#include <ctime>

#define HILOS 4
#define PUNTOS 100000
#define CLASES 17
#define MODO 0


int totalCambios = 0;	// Contabiliza la totalidad de los cambios realizados al grupo de puntos


/**
 *  Coloca a cada punto en una clase de manera aleatoria
 *  Utiliza el vector de clases para realizar la asignación
 *  
**/
void asignarPuntosAClases( long * clases, int modo, long muestras, long casillas, int hilos ) {
   long clase, pto;

   #pragma omp parallel num_threads( hilos ) private( clase, pto )
   {
      switch ( modo ) {
         case 0:	// Aleatorio
            unsigned int seed = omp_get_thread_num() + time( NULL );
            #pragma omp for
            for ( pto = 0; pto < muestras; pto++ ) {
               clase = rand_r( &seed ) % casillas;
               clases[ pto ] = clase;
            }
            break;
         case 1:	// A construir por los estudiantes
            #pragma omp for
            // round robin (los puntos se van asignando a los centros uno por uno como cartas repartidas en una mesa de poker)
            for( pto = 0; pto < muestras; pto++ ) {
               clases[ pto ] = pto % casillas;
            }

            break;
      }
   }
}

/**
 *  Recibe los centros, puntos y sus contadores, los reinicia, luego los suma de nuevo y promedia actualizando los centros
 *  
**/
void actualizarCentros( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long * contClases, int hilos ) {
   long clase, pto;

   #pragma omp parallel num_threads( hilos ) private( clase, pto ) // preguntar si se usa provate aqui o en los omp for!!!!!!!!!!!!!!!!!11
   {
        #pragma omp for
        // primero reinicio los centros y los contadores
        for( clase = 0; clase < centros->demeTamano(); clase++ ) {
            ( *centros )[ clase ]->ponga( 0, 0, 0 ); // para los ejes x, y, z
            contClases[ clase ] = 0;
        }

        #pragma omp for
        // sumar puntos de cada clase
        for( pto = 0; pto < puntos->demeTamano(); pto++ ) {
            clase = clases[ pto ];

            #pragma omp critical
            {
            ( *centros )[ clase ]->sume( ( * puntos )[ pto ] );
            contClases[ clase ]++;
            }
        }

        #pragma omp for
        // calcula el promedio dividiendo la suma de puntos entre su total
        for( clase = 0; clase < centros->demeTamano(); clase++ ) {

            // si existen clases promedio
            if( contClases[ clase ] > 0) {
                ( *centros )[ clase ]->divida( contClases[ clase ] );
            }
        }
   }
}


void actualizarPuntos( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long * contClases, long &cambios, int hilos ) {
   long pto, actual, nuevo;

   #pragma omp parallel for num_threads( hilos ) reduction( +:cambios ) private( actual, nuevo )
   // verifico si para cada punto...
   for( pto = 0; pto < puntos->demeTamano(); pto++ ) {
      actual = clases[ pto ];
      nuevo = centros->masCercano( ( *puntos )[ pto ] );

      // si su centro actual permanece como el mas cerano
      if( actual != nuevo ) {
        clases[ pto ] = nuevo;
        cambios++;
      }
   }
}


/**
 *  Programa muestra
 *  Variable: clases, almacena la clase a la que pertenece cada punto, por lo que debe ser del mismo tamaño que las muestras
 *  Variable: contClases, almacena los valores para la cantidad de puntos que pertenecen a un conjunto
**/
int main( int cantidad, char ** parametros ) {
   long cambios, clase, minimo, pto;
   Punto * punto;
   long casillas = CLASES;
   long muestras = PUNTOS;
   const char *nombreArchivo = "ci0117.eps";
   double start, finish, wused; // para tomar los tiempos
   int hilos = HILOS;
   int modo = MODO;

   // input del usuario con validacion de entrada

   if( cantidad > 1 ) { // numero de hilos
      hilos = atol( parametros[ 1 ] );

      if( hilos < 1 ) {
        hilos = HILOS;
        printf( "Cantidad de hilos invalida, usando valor por defecto %d\n", hilos );
      }
   }

   if( cantidad > 2 ) { // para los puntos
      muestras = atol( parametros[ 2 ] );

      if( muestras < 1 ) {
         muestras = PUNTOS;
         printf( "Cantidad de puntos invalida, usando valor por defecto %ld\n", muestras );
      }
   }

   if( cantidad > 3 ) { // para las clases
      casillas = atol( parametros[ 3 ] );

      if( casillas < 1 || casillas > muestras ) {
         casillas = CLASES;
         printf( "Cantidad de clases invalida, usando valor por defecto %ld\n", casillas );
      }
   }

   if( cantidad > 4 ) { // si el usuario le quiere dar un nombre al archivo ( nombres diferentes permiten comparar corridas con diversos parametros de entrada visualmente )
      nombreArchivo = parametros[ 4 ];

      // comprobar extension .eps correcta
      const char *extencion = strrchr( nombreArchivo, '.' );

      // si el nombre dle archivo no tiene extencion o no es .eps
      if( extencion == NULL || strcmp( extencion, ".eps" ) != 0 ) {
         printf( "Archivo sin extension .eps, usando ci0117.eps por defecto\n" );
         nombreArchivo = "ci0117.eps"; // uso el nombre que venia con el .zip del proyecto
      }
   }

   if( cantidad > 5 ) { // para el modo
      modo = atol( parametros[ 5 ] );

      if( modo != 0 && modo != 1 ) {
         modo = MODO;
         printf( "Modo invalido, usando modo por defecto %d\n", modo );
      }

      printf( "Modo %d\n", modo );
   }

   printf( "Usando %d hilos para generar %ld puntos, para %ld clases -> salida: %s\n", hilos, muestras, casillas, nombreArchivo );

// Procesar los parámetros del programa

   VectorPuntos * centros = new VectorPuntos( casillas );
   VectorPuntos * puntos  = new VectorPuntos( muestras, 10 );	// Genera un conjunto de puntos limitados a un círculo de radio 10
   long clases[ muestras ];		// Almacena la clase a la que pertenece cada punto
   long contClases[ casillas ];

   start = omp_get_wtime();

   asignarPuntosAClases( clases, modo, muestras, casillas, hilos );	// Asigna los puntos a las clases establecidas

   do {
	// Coloca todos los centros en el origen
	// Promedia los elementos del conjunto para determinar el nuevo centro

      actualizarCentros( centros, puntos, clases, contClases, hilos );

      cambios = 0;	// Almacena la cantidad de puntos que cambiaron de conjunto
	// Cambia la clase de cada punto al centro más cercano

      actualizarPuntos( centros, puntos, clases, contClases, cambios, hilos );

      totalCambios += cambios;

   } while ( cambios > 0 );	// Si no hay cambios el algoritmo converge

   finish = omp_get_wtime();
   wused = finish - start;

   printf( "Valor de la disimilaridad en la solución encontrada %g, con un total de %ld cambios\n", centros->disimilaridad( puntos, clases ), totalCambios );

   printf( "OpenMP wall time: %.6f s\n", wused );

// Con los valores encontrados genera el archivo para visualizar los resultados
   puntos->genEpsFormat( centros, clases, (char *)nombreArchivo );

}
