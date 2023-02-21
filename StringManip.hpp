#ifndef StiltFox_UniversalLibrary_StringManipulation
#define StiltFox_UniversalLibrary_StringManipulation
#include <string>
#include <unordered_map>

std::string parseLine(std::string);
std::string parseToDelim(std::string toParse, std::string delim, bool matchAny = false);
std::unordered_map<std::string,std::string> parseMap(std::string toParse, std::string valueDelim, std::string entryDelim);
#endif