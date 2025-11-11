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

#define PUNTOS 100000
#define CLASES 17
#define MODO 0


int totalCambios = 0;	// Contabiliza la totalidad de los cambios realizados al grupo de puntos


/**
 *  Coloca a cada punto en una clase de manera aleatoria
 *  Utiliza el vector de clases para realizar la asignación
 *  
**/
void asignarPuntosAClases( long * clases, int modo, long casillas, long muestras ) {  // CAMBIOSSSSSSSSSSS!!!!!!!!!
   long clase, pto;

   switch ( modo ) {
      case 0:	// Aleatorio
         for ( pto = 0; pto < muestras; pto++ ) {
            clase = rand() % casillas;
            clases[ pto ] = clase;
         }
         break;
      case 1:	// A construir por los estudiantes

         // round robin (los puntos se van asignando a los centros uno por uno como cartas repartidas en una mesa de poker)
         for( pto = 0; pto < muestras; pto++ ) {
            clases[ pto ] = pto % casillas;
         }

         break;
   }

}

/**
 *  Recibe los centros, puntos y sus contadores, los reinicia, luego los suma de nuevo y promedia actualizando los centros
 *  
**/
void actualizarCentros( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long * contClases ) {
   long clase, pto;

   // primero reinicio los centros y los contadores
   for( clase = 0; clase < centros->demeTamano(); clase++ ) {
      ( *centros )[ clase ]->ponga( 0, 0, 0 ); // para los ejes x, y, z
      contClases[ clase ] = 0;
   }

   // sumar puntos de cada clase
   for( pto = 0; pto < puntos->demeTamano(); pto++ ) {
      clase = clases[ pto ];
      ( *centros )[ clase ]->sume( ( * puntos )[ pto ] );
      contClases[ clase ]++;
   }

   // calcula el promedio dividiendo la suma de puntos entre su total
   for( clase = 0; clase < centros->demeTamano(); clase++ ) {

      // si existen clases promedio
      if( contClases[ clase ] > 0) {
         ( *centros )[ clase ]->divida( contClases[ clase ] );
      }
   }
}


void actualizarPuntos( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long * contClases, long &cambios ) {
   long pto, actual, nuevo;

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
   int modo = MODO;

   // input del usuario con validacion de entrada
   if( cantidad > 1 ) { // para los puntos
      muestras = atol( parametros[ 1 ] );

      if( muestras < 1 ) {
         muestras = PUNTOS;
         printf( "Cantidad de puntos invalida, usando valor por defecto %ld\n", muestras );
      }
   }

   if( cantidad > 2 ) { // para las clases
      casillas = atol( parametros[ 2 ] );

      if( casillas < 1 ) {
         casillas = CLASES;
         printf( "Cantidad de clases invalida, usando valor por defecto %ld\n", casillas );
      }
   }

   if( cantidad > 3 ) { // si el usuario le quiere dar un nombre al archivo ( nombres diferentes permiten comparar corridas con diversos parametros de entrada visualmente )
      nombreArchivo = parametros[ 3 ];

      // comprobar extension .eps correcta
      const char *extencion = strrchr( nombreArchivo, '.' );

      // si el nombre dle archivo no tiene extencion o no es .eps
      if( extencion == NULL || strcmp( extencion, ".eps" ) != 0 ) {
         printf( "Archivo sin extension .eps, usando ci0117.eps por defecto\n" );
         nombreArchivo = "ci0117.eps"; // uso el nombre que venia con el .zip del proyecto
      }
   }

   if( cantidad > 4 ) { // para el modo
      modo = atol( parametros[ 4 ] );

      if( modo != 0 && modo != 1 ) {
         modo = MODO;
         printf( "Modo invalido, usando modo por defecto %d\n", modo );
      }

      printf( "Modo %d\n", modo );
   }

   printf( "Generando %ld puntos, para %ld clases -> salida: %s\n", muestras, casillas, nombreArchivo );

// Procesar los parámetros del programa

   VectorPuntos * centros = new VectorPuntos( casillas );
   VectorPuntos * puntos  = new VectorPuntos( muestras, 10 );	// Genera un conjunto de puntos limitados a un círculo de radio 10
   long clases[ muestras ];		// Almacena la clase a la que pertenece cada punto
   long contClases[ casillas ];

   asignarPuntosAClases( clases, modo, muestras, casillas );	// Asigna los puntos a las clases establecidas

   do {
	// Coloca todos los centros en el origen
	// Promedia los elementos del conjunto para determinar el nuevo centro

      actualizarCentros( centros, puntos, clases, contClases );

      cambios = 0;	// Almacena la cantidad de puntos que cambiaron de conjunto
	// Cambia la clase de cada punto al centro más cercano

      actualizarPuntos( centros, puntos, clases, contClases, cambios );

      totalCambios += cambios;

   } while ( cambios > 0 );	// Si no hay cambios el algoritmo converge

   printf( "Valor de la disimilaridad en la solución encontrada %g, con un total de %ld cambios\n", centros->disimilaridad( puntos, clases ), totalCambios );

// Con los valores encontrados genera el archivo para visualizar los resultados
   puntos->genEpsFormat( centros, clases, (char *)nombreArchivo );

}

// FALTA TOMAR EL TIEMPO DE LA VERSION SERIAL