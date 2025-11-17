#include "lcs.h"

LCS::LCS() {

}


LCS::~LCS() {

}


string LCS::serialLCS( string& S1, string& S2 ) {

    matriz = vector< vector< int >>( S1.size() + 1, vector< int >( S2.size() + 1, 0 ) );

    for( int i = 1; i <= S1.size(); i++ ) {

        for( int j = 1; j <= S2.size(); j++ ) {

            if( S1[ i - 1 ] == S2[ j - 1 ]) {

                matriz[ i ][ j ] = matriz[ i - 1 ][ j - 1 ] + 1;
            } else {

                matriz[ i ][ j ] = max( matriz[ i - 1 ][ j ], matriz[ i ][ j - 1 ]);
            }
        }
    }

    string resultado = "";

    int i = S1.size();
    int j = S2.size();

    while( i > 0 && j > 0 ) {

        if( S1[ i - 1 ] == S2[ j - 1 ] ) {

            resultado = S1[ i - 1 ] + resultado;

            i--;
            j--;
        } else if( matriz[ i - 1 ][ j ] > matriz[ i ][ j - 1 ] ) {
            
            i--;
        } else {
            
            j--;
        }
    }

    return resultado;
}

string LCS::serialLCS( string& S1, string& S2 ) {

    // inicializo la matriz de la clase como un vector 2D
    matriz = vector< vector< int >>(      
        S1.size() + 1, // cantidad de filas = largo de S1 + 1
        vector< int >( S2.size() + 1, 0 ) // cada fila es un vector de enteros del tamaño de S2 + 1, todo lleno de ceros
    );

    // recorro las filas desde 1 hasta n -> tamaño de S1
    for( int i = 1; i <= S1.size(); i++ ) {

        // recorro las columnas desde 1 hasta m -> tamaño de S2
        for( int j = 1; j <= S2.size(); j++ ) {

            // si los caracteres en estas posiciones coinciden
            if( S1[ i - 1 ] == S2[ j - 1 ] ) {

                // tomo la diagonal superior izquierda y le sumo 1
                matriz[ i ][ j ] = matriz[ i - 1 ][ j - 1 ] + 1;
            } else {

                // tomo el valor maximo entre arriba y la izquierda
                matriz[ i ][ j ] = max( matriz[ i - 1 ][ j ], matriz[ i ][ j - 1 ] );
            }
        }
    }

    string resultado = ""; // aqui voy armando la subsecuencia mas larga final

    int i = S1.size();
    int j = S2.size();

    while( i > 0 && j > 0 ) {

         // si los caracteres actuales de S1 y S2 coinciden
        if( S1[ i - 1 ] == S2[ j - 1 ] ) {

            resultado = S1[ i - 1 ] + resultado;// agrego ese caracter al inicio de la respuesta de atras hacia adelante

            // me muevo en diagonal hacia arriba a la izquierda
            i--;
            j--;

        // si el valor de arriba es mayor que el de la izquierda
        } else if( matriz[ i - 1 ][ j ] > matriz[ i ][ j - 1 ] ) {

            i--; // me muevo hacia arriba

        // si el valor de la izquierda es mayor o igual
        } else {

            j--; // me muevo hacia la izquierda
        }
    }

    return resultado;
}
