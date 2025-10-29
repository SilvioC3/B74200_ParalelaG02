#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <regex>

#include "FileReader.h"

using namespace std;

void countTags( FileReader* reader, int id, map< string, int > &localCounts );
