#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>

namespace string_utils {

using namespace std;

inline vector<string> split(const string& str, const string& delim)
{
    vector<string> result;
    size_t start = 0;

    for (size_t found = str.find(delim); found != string::npos; found = str.find(delim, start))
    {
        result.emplace_back(str.begin() + start, str.begin() + found);
        start = found + delim.size();
    }
    if (start != str.size())
        result.emplace_back(str.begin() + start, str.end());
    return result;      
}

//inline vector<string> split(const string_view& str, const string& delim)
//{
//    return split(str.data(), delim);      
//}


// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

};
