#include "lcs.h"

LCS::LCS() {

}


LCS::~LCS() {

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


string LCS::paralelaLCS( string& S1, string& S2 ) {

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int n = S1.size();
    int m = S2.size();

    int filas    = n + 1;
    int columnas = m + 1;

    // matriz compartida pero cada proceso tendra su propia copia
    matriz = vector< vector<int> >( filas, vector<int>( columnas, 0 ) );

    // indice lineal dentro de la matriz
    auto getCell = [&]( int i, int j ) -> int& {
        return matriz[ i ][ j ];
    };

    int totalDiag = n + m;

    // recorro diagonales k = i + j
    for( int k = 2; k <= totalDiag; k++ ) {

        // rango de i valido para esta diagonal
        int i_min = max( 1, k - m );
        int i_max = min( n, k - 1 );

        int diagLen = i_max - i_min + 1; // cantidad de celdas en esta diagonal

        if( diagLen <= 0 ) {
            // si no hay nada que hacer en esta diagonal
            MPI_Barrier( MPI_COMM_WORLD );
            continue;
        }

        // vectores local y global con los valores de la diagonal actual
        vector<int> diagLocal( diagLen, 0 );
        vector<int> diagGlobal( diagLen, 0 );

        // division simple de la diagonal entre procesos
        int chunk = ( diagLen + size - 1 ) / size;

        int beginIndex = rank * chunk;
        int endIndex   = min( diagLen, beginIndex + chunk ) - 1;

        if( beginIndex < diagLen ) {
            // si este proceso tiene al menos una celda que calcular
            for( int idx = beginIndex; idx <= endIndex; idx++ ) {

                int i = i_min + idx;
                int j = k - i;

                if( j <= 0 || j > m ) {
                    continue;
                }

                int arriba   = getCell( i - 1, j );
                int izquierda = getCell( i, j - 1 );
                int diag      = getCell( i - 1, j - 1 );

                if( S1[ i - 1 ] == S2[ j - 1 ] ) {
                    diagLocal[ idx ] = diag + 1;
                } else {
                    diagLocal[ idx ] = ( arriba > izquierda ? arriba : izquierda );
                }
            }
        }

        // combina los datos de todos los procesos en la diagonal actual
        MPI_Allreduce( diagLocal.data(), diagGlobal.data(), diagLen, MPI_INT, MPI_MAX, MPI_COMM_WORLD );

        // escribe la diagonal completa en la matriz local de cada proceso
        for( int idx = 0; idx < diagLen; idx++ ) {
            int i = i_min + idx;
            int j = k - i;

            if( j > 0 && j <= m ) {
                getCell( i, j ) = diagGlobal[ idx ];
            }
        }

        MPI_Barrier( MPI_COMM_WORLD );
    }

    // reconstruccion de la LCS resultado por el rank 0
    string resultado = "";

    if( rank == 0 ) {
        int i = n;
        int j = m;

        while( i > 0 && j > 0 ) {

            if( S1[ i - 1 ] == S2[ j - 1 ] ) {
                // este caracter forma parte de la subsecuencia
                resultado = S1[ i - 1 ] + resultado;
                i--;
                j--;
            } else {
                int arriba   = getCell( i - 1, j );
                int izquierda = getCell( i, j - 1 );

                if( arriba >= izquierda ) {
                    i--;
                } else {
                    j--;
                }
            }
        }
    }

    return resultado;
}
