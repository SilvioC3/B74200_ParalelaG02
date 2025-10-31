#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <regex>
#include <algorithm>

#include "FileReader.h"

using namespace std;

void countTagsBuffer( const string &buffer, map< string, int > &localCounts, string &carryOver );
