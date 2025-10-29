#include "hilocontador.h"

void countTags( FileReader* reader, int id, std::map<std::string, int> &localCounts ) {

    string line;

    regex tagPattern( R"(<\s*/?\s*([a-zA-Z0-9]+)[^>]*>)" ); 

    while( reader->getNextLine( line ) ) {

        smatch matches;
        string::const_iterator searchStart( line.cbegin() );

        // busca todas las coincidencias de etiquetas en la linea
        while ( regex_search( searchStart, line.cend(), matches, tagPattern ) ) {
            
            string tag = matches[ 1 ]; // el 1 es el nombre de la etiqueta

            // todo en lowercase
            transform( tag.begin(), tag.end(), tag.begin(), ::tolower );

            // contador local
            localCounts[ tag ]++;
            
            searchStart = matches.suffix().first;
        }
    }
}
