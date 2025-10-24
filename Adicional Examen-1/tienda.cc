/**
 * Represent a shopping store
 *
 * @author	Programacion Concurrente
 * @version	2025/Oct/10
 *
 * Primer examen parcial
 *
 * Grupo 3
 *
**/


#include <string>		// std::string
#include <unistd.h>		// calloc


#include "tienda.h"

/**
 * Default contructor
 * Create a store with random filled values for prices and weights
 *
**/
Tienda::Tienda( int items, int capacidadCarrito ) {
   int item;

   srand( getpid() );
 
   this->articulos = items;
   this->capacidadCarrito = capacidadCarrito;
   this->pesos   = (int *) calloc( items, sizeof( int ) );
   this->precios = (int *) calloc( items, sizeof( int ) );
   for ( item = 0; item < items; item++ ) {
      this->pesos[ item ] = 1 + (random() & 15);
      this->precios[ item ] = 10 + (random() & 15);
   }

}


/**
 * Parameter constructor
 * @param	int	items quantity
 * @param	int *	array with items' weights
 * @param	int *	array with items' values
 *
**/
Tienda::Tienda( int capacidadCarrito, int items, const int * pesos, const int * precios ) {
   int item;

   this->capacidadCarrito = capacidadCarrito;
   this->articulos = items;
   this->pesos   = (int *) calloc( items, sizeof( int ) );
   this->precios = (int *) calloc( items, sizeof( int ) );
   for ( item = 0; item < items; item++ ) {
      this->pesos[ item ] = pesos[ item ];
      this->precios[ item ] = precios[ item ];
   }

}


/**
 *
 *
**/
Tienda::~Tienda() {

   free( this->pesos );
   free( this->precios );

}


/**
 * Return a String with store items
**/
std::string Tienda::toString() {
   std::string result = "";
   int item;

   result += "Tienda con " + std::to_string( this->articulos ) + " artículos y capacidad de carrito " 
          + std::to_string( this->capacidadCarrito ) + "\n";
   result += "Pesos = { ";
   for ( item = 0; item < this->articulos; item++ ) {
      result += std::to_string( this->pesos[ item ] );
      if ( item < this->articulos - 1 ) {
         result += ", ";
      }
   }

   result += " }\nPrecios = { ";
   for ( item = 0; item < articulos; item++ ) {
      result += std::to_string( this->precios[ item ] );
      if ( item < this->articulos - 1 ) {
         result += ", ";
      }
   }
   result += " }\n";

   return result;

}


/**
 *
 *   Coloca artículos en el carrito para maximizar ganancias sin pasarse de un peso establecido
 *
 **/
void Tienda::llenarCarrito() {

    // canasta generica para ordenar por precios
    int canasta [this->articulos];

    // variables auxiliares
    int priciest = 0;
    int remainingCapacity;
    int optimalCapacity;
    int currentProfit = 0;
    int highestProfit = 0;

    // ordenar la canasta
    for( int item = 0; item < this->articulos; item++ ) {
        
        for( int item2 = 0; item2 < this->articulos; item2++ ) {

            if( precios[ item ] > precios[ item2 ]) {

                priciest = item;
            } else if( precios[ item ] < item2 ) {

                priciest = item2;
            } else {

                priciest = item;
            }
        }
        
        // termina como un arreglo que guarda el indice de los articulos ordenado de mas caro al mas barato
        canasta[ item ] = priciest;
    }


   // por cada articulo que tengo, desde el mas caro al mas barato
   for( int item = 0; item < this->articulos; item++ ) {
        
        remainingCapacity = this->capacidadCarrito;
        currentProfit = 0;

        // para el primer item de cada version del carrito verifico que su peso no exeda el limite del carrito y lo agrego
        if( pesos[ canasta[ item ] ] < remainingCapacity ) {

            remainingCapacity = this->capacidadCarrito - pesos[ canasta[ item ] ];
            currentProfit = currentProfit + precios[ canasta[ item ] ];

            // con el articulo mas caro dentro del carrito verifico los articulos restantes y meto los que pueda hasta llenar mi capacidad o agotar existencias
            for( int item2 = 0; item2 < this->articulos; item2++ ) {
                
                if(item2 == item){

                    // no pasa nada porque no se pueden repetir articulos
                } else {

                    // para cada articulo que encuentro que pueda entrar al carrito le sumo su precio a la ganancia y substrigo su peso de la capacidad restante
                    if( pesos[ canasta[ item2 ] ] < remainingCapacity ){

                        remainingCapacity = remainingCapacity - pesos[ canasta[ item2 ] ];
                        currentProfit = currentProfit + precios[ canasta[ item2] ];
                    }

                }
            }

            // si la ganancia actual es mayor que la ganancia maxima actualizo la ganancia maxima
            if( currentProfit > highestProfit) {
                highestProfit = currentProfit;

                // y tambien actualizo la capacidad que utilice que seria la optima
                optimalCapacity = this->capacidadCarrito - remainingCapacity;

            }


        }  else if( pesos[ canasta[ item ] ] > remainingCapacity ) {
            printf("Este item es demasiado pesado para el carrito\n");

        }
   }

   // imprimo los resultados obtenidos
   printf( "La ganancia total es de: [ %d ], usando [ %d ] kilos de capacidad", highestProfit, optimalCapacity );

}