// Copyright 2026

#include <iostream>
#include <string>
#include <algorithm>
#include "textgen.h"

int main(int argc, char* argv[]) {
    std::string inputFile = "input.txt";
    std::string outputFile = "gen.txt";
    const int NPREF = 2;
    const int MAXGEN = 1000;
    std::cout << "Генератор текста на основе цепи Маркова" << std::endl;
    std::cout << "Размер префикса: " << NPREF << " слов" << std::endl;
    std::cout << "Объем генерации: " << MAXGEN << " слов" << std::endl;
    std::cout << "Входной файл: " << inputFile << std::endl;

    TextGenerator generator(NPREF, MAXGEN);
    std::cout << "Чтение файла и построение таблицы..." << std::endl;
    if (!generator.buildTable(inputFile)) {
        std::cerr << "Ошибка при чтении входного файла!" << std::endl;
        return 1;
    }
    std::cout << "Генерация текста..." << std::endl;
    std::string generatedText = generator.generate();
    if (generator.saveToFile(outputFile, generatedText)) {
        std::cout << "Текст сохранен в " << outputFile << std::endl;
    } else {
        std::cerr << "Ошибка при сохранении результата!" << std::endl;
        return 1;
    }
    return 0;
}
