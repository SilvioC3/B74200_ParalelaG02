#include "hilolector.h"

struct CounterArgs
{
    HiloLector* lector;
    int counterID;
};

HiloLector::HiloLector( string file, int counters, int strat) : filePath( file ), numThreads( counters ), strategy( strat ) {
    
    // printf("IM IN!\n");

    reader = new FileReader( filePath, numThreads, strategy );

    pthread_mutex_init( &mutex, NULL );
}

HiloLector::~HiloLector() {
    delete reader;

    pthread_mutex_destroy( &mutex );
}

FileReader* HiloLector::getReader() {
    return reader;
}

string HiloLector::getFilePath() {
    return filePath;
}

void* executeCounters( void* arg ) {

    CounterArgs* args = ( CounterArgs* ) arg;

    // print para verificar contadores
    // pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
    // pthread_mutex_lock(&printMutex);
    // printf("  Soy el contador %d del lector %s\n", args->counterID, args->lector->getFilePath().c_str());
    // fflush(stdout); // fuerza a vaciar el buffer
    // pthread_mutex_unlock(&printMutex);

    // mapa para conteo local de etiquetas
    map< string, int > localCounts;

    countTags( args->lector->getReader(), args->counterID, localCounts );

    args->lector->updateTagCounts( localCounts );

    pthread_exit( NULL );
}

// version de prueba para imprimir por lector
// void HiloLector::counters() {

//     counterThreads.resize( numThreads );
//     vector< CounterArgs > args( numThreads );

//     // inicio los contadores que me pide le usuario
//     for( int i = 0; i < numThreads; i++ ) {
//         args[ i ] = { this, i };
//         pthread_create( &counterThreads[ i ], NULL, executeCounters, &args[ i ]);
//     }

//     // espero que terminen
//     for( auto& counter : counterThreads ) {
//         pthread_join( counter, NULL );
//     }

//     // imprimir resultados
//     printTagCount();
// }

map< string, int > HiloLector::counters() {

    counterThreads.resize( numThreads );
    vector< CounterArgs > args( numThreads );

    // inicio los contadores que me pide le usuario
    for( int i = 0; i < numThreads; i++ ) {
        args[ i ] = { this, i };
        pthread_create( &counterThreads[ i ], NULL, executeCounters, &args[ i ]);
    }

    // espero que terminen
    for( auto& counter : counterThreads ) {
        pthread_join( counter, NULL );
    }

    return tagCounts;
}

void HiloLector::updateTagCounts( const map< string, int > &localCounts ) {

    // bloquea la lectura
    pthread_mutex_lock( &mutex );

    for( const auto& tag : localCounts ) {
        tagCounts[ tag.first ] += tag.second;
    }

    // desbloquea el hilo
    pthread_mutex_unlock( &mutex );

}

void HiloLector::printTagCount() {

    std::cout << "\nCount for: " << filePath << ":\n";

    for(const auto& tag : tagCounts) {
        std::cout << tag.first << " : " << tag.second << endl;
    }

    cout << endl;
}
