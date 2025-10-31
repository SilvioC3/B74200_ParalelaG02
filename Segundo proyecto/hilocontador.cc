#include "hilocontador.h"

// 🔹 Procesa un buffer parcial o completo ≤512 bytes
void countTagsBuffer( const string &buffer, map< string, int > &localCounts, string &carryOver ) {

    smatch matches;
    regex tagPattern( R"(<\s*/?\s*([a-zA-Z0-9]+)[^>]*>)" );

    //  acumular el fragmento de etiqueta cortada con el buffer
    string text = carryOver + buffer;
    carryOver.clear();

    string::const_iterator searchStart( text.cbegin() );

    while( regex_search( searchStart, text.cend(), matches, tagPattern ) ) {

        string tag = matches[ 1 ];
        transform( tag.begin(), tag.end(), tag.begin(), ::tolower ); // todo a lowercase por si las dudas
        localCounts[ tag ]++;
        searchStart = matches.suffix().first;
    }

    // verifica si existe una etiqueta cortada al final de mi bloque
    size_t lastOpen = text.rfind( '<' );
    size_t lastClose = text.rfind( '>' );

    if( lastOpen != string::npos && ( lastClose == string::npos || lastOpen > lastClose ) ) {
        // agrego el sobrante al carryOver
        carryOver = text.substr( lastOpen );
    }
}
