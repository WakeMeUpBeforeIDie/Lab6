// Copyright 2026

#ifndef TEXTGEN_H
#define TEXTGEN_H

#include <deque>
#include <map>
#include <vector>
#include <string>
#include <random>

class TextGenerator {
private:
    typedef std::deque<std::string> Prefix;
    typedef std::map<Prefix, std::vector<std::string>> Statetab;
    Statetab statetab;
    const int NPREF;
    const int MAXGEN;
    std::mt19937 rng;
    
public:
    TextGenerator(int npref = 2, int maxgen = 1000);
    bool buildTable(const std::string& filename);
    std::string generate();
    bool saveToFile(const std::string& filename, const std::string& text);
};

#endif // TEXTGEN_H
