#include "StringManip.hpp"

using namespace std;

string parseLine(const string& inputStr)
{
    return parseToDelim(inputStr, "\r\n", true);
}

string parseToDelim(const string& toParse, const string& delim, bool matchAny)
{
    int pos = matchAny ? toParse.find_first_of(delim) : toParse.find(delim);
    return pos > -1 ? toParse.substr(0,pos) : toParse;
}

unordered_map<string,string> parseMap(const string& toParse, const string& valueDelim, const string& entryDelim)
{
    unordered_map<string,string> output;

    int currentPos = 0;

    while(currentPos < toParse.length())
    {
        string value;
        string key = parseToDelim(toParse.substr(currentPos), valueDelim);
        currentPos += key.length() + valueDelim.length();
        if (currentPos < toParse.length()) 
        {
            value = parseToDelim((const string)toParse.substr(currentPos), entryDelim);
            currentPos += value.length() + entryDelim.length();
        } 
        output[key] = value;
    }

    return output;
}