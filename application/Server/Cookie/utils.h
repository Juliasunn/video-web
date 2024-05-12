#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>

#include <iostream>

namespace string_utils {

using namespace std;

inline vector<string> split(const string_view strview, const string& delim)
{
    std::cout << "Initial string: |" << strview << "| "<<std::endl;
    vector<string> result;
    size_t start = 0;

    for (size_t found = strview.find(delim); found != string::npos; found = strview.find(delim, start))
    {
        result.emplace_back(strview.begin() + start, strview.begin() + found);
        start = found + delim.size();
    }
    if (start != strview.size())
        result.emplace_back(strview.begin() + start, strview.end());
    return result;      
}

//inline vector<string> split(const string_view& str, const string& delim)
//{
//    return split(str.data(), delim);      
//}

inline bool is_trimable_char(unsigned char ch) {
    return (std::iscntrl(ch) || std::isspace(ch)); 
}

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !is_trimable_char(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !is_trimable_char(ch);
    }).base(), s.end());
}

inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

};
