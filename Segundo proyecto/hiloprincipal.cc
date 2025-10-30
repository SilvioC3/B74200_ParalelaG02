#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "hilolector.h"

using namespace std;

// estructura para pasar parametros
struct LectorArgs
{
    string filePath;
    int numThreads;
    int strategy;
};

void startTimer( struct timeval * timerStart ) {
    gettimeofday( timerStart, NULL );
}

double getTimer( struct timeval timerStart ) {
    struct timeval timerStop, timerElapsed;
    gettimeofday( &timerStop, NULL );
    timersub( &timerStop, &timerStart, &timerElapsed );
    return timerElapsed.tv_sec * 1000.0 + timerElapsed.tv_usec / 1000.0;
}

void* executeLectors( void* arg ) {

    // convierto el puntero void generico para sacar datos de mi struct
    LectorArgs* args = ( LectorArgs* ) arg;

    // print para verificar lectores
    // static int lectorID = 0; // contador de lectores
    // int myID;
    // pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER; // para que no se mezclen prints
    // pthread_mutex_lock( &printMutex );
    // myID = ++lectorID;
    // pthread_mutex_unlock( &printMutex );
    // printf( "Soy el lector %d ( archivo: %s )\n", myID, args->filePath.c_str() );

    // objeto lector con sus respectivos parametros
    HiloLector lector( args->filePath, args->numThreads, args->strategy );

    // para la version de prueba donde cada lector imprime sus resultados
    // llamo al metodo counters para contar las etiquetas en cada archivo HTML
    // lector.counters();
    // pthread_exit( NULL );

    // acumulo en un map los conteos de mis lectores
    map< string, int >* tagResult = new map< string, int > ( lector.counters() );

    pthread_exit( ( void* ) tagResult );
}

int main( int argc, char* argv[] ) {

    // verifica que el usuario haga el input correcto
    if ( argc < 2 ) {
        cout << "Uso: ./bin/contar -t=<num_hilos> archivo1.html archivo2.html etc... -e=<estrategias>\n";
        return 1;
    }

    vector< string > files;
    int workerCount = 1; // contadores pedidos por el usuario 1 por defecto
    vector < int > strategies;

    // mapa para almacenar los resultados del conteo global
    map< string, int > globalTagCount;

    // para argumentos de entrada de la forma: contar -t=10 a.html b.html c.html -e=2,3,4
    for( int i = 1; i < argc; i++ ) {

        string arg = argv[ i ];

        // comprueba que el argumento actual del input inicie con -t=
        if( arg.rfind( "-t=", 0 ) == 0 ) {

            workerCount = stoi( arg.substr( 3 ) ); // extrae el 4to caracter o sea el numero y lo asigna a workerCount

        // lo mismo pero con -e=
        } else if( arg.rfind( "-e=", 0 ) == 0 ) {

            // string para manejar la linea de estrategias
            string strat = arg.substr( 3 );
            size_t pos = 0;

            // busca las comas y extrae los numeros a la izquierda
            while( ( pos = strat.find( "," ) ) != string::npos ) {

                strategies.push_back( stoi( strat.substr( 0, pos ) ) );

                // y borro la coma
                strat.erase( 0, pos + 1 );
            }

            // cuando ya no hay comas guardo le ultimo numero que no entraria en el while
            strategies.push_back( stoi( strat ) );
        } else {

            // guardo los paths de los archivos
            files.push_back( arg );
        }
    }

    // imprimo lo que tengo hasta ahora
    cout << "Sorting...\n" << "Files: " << files.size() << "\nUsing: " << workerCount << " counters per file" << endl;

    // para procesarlos asigno un hilo por archivo
    vector< pthread_t > lectorThreads( files.size() );
    vector< LectorArgs > lectorArgs( files.size() );

    // para medir le tiempo
    clock_t start, finish;
    struct timeval timerStart;
    double used, wused;

    startTimer( &timerStart );
    start = clock();

    // arranco los hilos
    for( size_t i = 0; i < files.size(); i++ ) {

        int strategy = 1;

        // si una estrategia no fue asignada se queda con la 1
        if( i < strategies.size()) {

            strategy = strategies[ i ];
        } else {
            printf("WARNING: cannot find strategy for '%s'. Assigning default strategy 1...\n", files[ i ].c_str());

        }

        lectorArgs[ i ] = { files[ i ], workerCount, strategy };
        pthread_create( &lectorThreads[ i ], NULL, executeLectors, &lectorArgs[ i ] );
    }

    // para pruebas anteriores
    // espero a que los hilos terminen
    // for( auto& lector : lectorThreads ) {

    //     pthread_join( lector, NULL );
    // }

    // acumulo los tags cuando los hilos lectores termian su ejecucion
    for( size_t i = 0; i < lectorThreads.size(); i++ ) {

        void* tagReturn;

        pthread_join( lectorThreads[ i ], &tagReturn );

        map< string, int >* tagPackage = static_cast< map< string, int >* >( tagReturn );

        // acumulo los tags que me retonaron en el acumulador global
        for( const auto& tag : *tagPackage ) {
            
            globalTagCount[ tag.first ] += tag.second;
        }

        // libero la mameria dinamica
        delete tagPackage;
    }

    finish = clock();
    used = ( ( double ) ( finish - start ) ) / CLOCKS_PER_SEC; // tiempo del CPU del hilo principal
    wused = getTimer( timerStart ); // tiempo de pared

    // imprimo los resultados una vez termino
    printf( "\nGlobal tag count:\n" );

    for( const auto& tag : globalTagCount ) {
        // printf( "IM IN");
        cout << tag.first << " : " << tag.second << endl;
    }

    printf( "\nWall time: %.3f ms\n", wused );
    printf( "Main thread CPU time: %.3f s\n", used);

    return 0;
}
