// Copyright 2026

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <string>
#include "textgen.h"

TextGenerator::TextGenerator(int npref, int maxgen)
    : NPREF(npref), MAXGEN(maxgen) {
    rng.seed(std::time(nullptr));
}

bool TextGenerator::buildTable(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия " << filename << std::endl;
        return false;
    }

    Prefix prefix;
    std::string word;
    for (int i = 0; i < NPREF; ++i) {
        prefix.push_back("");
    }
    while (file >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](char c) { return std::ispunct(static_cast<unsigned char>(c)); 
            }),
            word.end());

        statetab[prefix].push_back(word);

        prefix.pop_front();
        prefix.push_back(word);
    }

    file.close();
    return true;
}

std::string TextGenerator::generate() {
    if (statetab.empty()) {
        return "Ошибка: таблица префиксов пуста";
    }
    std::ostringstream output;
    Prefix prefix;
    for (int i = 0; i < NPREF; ++i) {
        prefix.push_back("");
    }
    auto it = statetab.begin();
    while (it != statetab.end() && it->first == prefix) {
        ++it;
    }

    if (it != statetab.end()) {
        prefix = it->first;
    }
    
    for (const auto& w : prefix) {
        if (!w.empty()) {
            output << w << " ";
        }
    }
    int generated = 0;
    while (generated < MAXGEN) {
        auto it = statetab.find(prefix);
        if (it == statetab.end() || it->second.empty()) {
            break;
        }
        std::uniform_int_distribution<> dist(0, it->second.size() - 1);
        std::string suffix = it->second[dist(rng)];

        if (suffix.empty()) {
            break;
        }

        output << suffix << " ";
        generated++;
        prefix.pop_front();
        prefix.push_back(suffix);
    }
    return output.str();
}

bool TextGenerator::saveToFile(const std::string& filename, const std::string& text) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
        return false;
    }
    file << text;
    file.close();
    return true;
}
