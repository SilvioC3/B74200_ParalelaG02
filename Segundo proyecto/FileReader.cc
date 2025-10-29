#include "FileReader.h"

FileReader::FileReader( string file, int counters, int strat ) : eof( false ), numThreads( counters ), strategy( strat ) {

    filePath.open( file );

    if( !filePath.is_open() ) {

        cerr << "\nERROR! cant open file: " << file << endl;
        exit( 1 );
    }

    pthread_mutex_init( &mutex, NULL );
}

FileReader::~FileReader() {

    if( filePath.is_open() ) {

        filePath.close();
    }

    pthread_mutex_destroy( &mutex );
}

// cada vez que un hilo llama esta funcion aseguro exclusion con el mutex
bool FileReader::getNextLine( string &line ) {

    pthread_mutex_lock( &mutex );

    if( eof == true || !getline( filePath, line ) ) {
        eof = true;

        pthread_mutex_unlock( &mutex );
        return false;
    }

    pthread_mutex_unlock( &mutex );
    return true;
}
