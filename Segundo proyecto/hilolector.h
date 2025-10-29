#pragma once

#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <map>

#include "hilocontador.h"
#include "FileReader.h"

using namespace std;

class HiloLector {
    private:
        string filePath;
        int numThreads;
        int strategy;

        FileReader* reader;

        vector< pthread_t > counterThreads;

        pthread_mutex_t mutex;

        // acumulador de etiquetas
        map< string, int > tagCounts;

    public:
        HiloLector( string file, int counters, int strat );
        ~HiloLector();

        FileReader* getReader();

        string getFilePath();

        // crea hilos contadores y junta resultados
        void counters();

        // para que los contadores actualizen resultados
        void updateTagCounts( const map< string, int > &localCounts);

        void printTagCount();
};
