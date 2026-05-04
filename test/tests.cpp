// Copyright 2026

#include <gtest/gtest.h>
#include <deque>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <algorithm>
#include <ctime>
#include "textgen.h"

typedef std::deque<std::string> Prefix;
typedef std::map<Prefix, std::vector<std::string>> Statetab;
class MarkovChainTester {
 private:
    Statetab statetab;
    const int NPREF;
    std::mt19937 rng;
 public:
    explicit MarkovChainTester(int npref = 2) : NPREF(npref) {
        rng.seed(42);
    }
    void addEntry(const Prefix& prefix, const std::string& suffix) {
        statetab[prefix].push_back(suffix);
    }
    Prefix makePrefix(const std::vector<std::string>& words) {
        Prefix prefix;
        for (size_t i = 0;
         i < words.size() && i < static_cast<size_t>(NPREF); ++i) {
            prefix.push_back(words[i]);
        }
        while (prefix.size() < static_cast<size_t>(NPREF)) {
            prefix.push_back("");
        }
        return prefix;
    }
    std::vector<std::string> getSuffixes(const Prefix& prefix) {
        auto it = statetab.find(prefix);
        if (it != statetab.end()) {
            return it->second;
        }
        return std::vector<std::string>();
    }
    std::string selectRandomSuffix(const std::vector<std::string>& suffixes) {
        if (suffixes.empty()) {
            return "";
        }
        std::uniform_int_distribution<> dist(0, suffixes.size() - 1);
        return suffixes[dist(rng)];
    }
    std::string generateText(int maxWords, const Prefix& startPrefix) {
        if (statetab.empty()) {
            return "";
        }  
        std::ostringstream output;
        Prefix prefix = startPrefix;
        for (const auto& w : prefix) {
            if (!w.empty()) {
                output << w << " ";
            }
        }
        int generated = 0;
        while (generated < maxWords) {
            auto it = statetab.find(prefix);
            if (it == statetab.end() || it->second.empty()) {
                break;
            }
            std::string suffix = selectRandomSuffix(it->second);
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
    int countWords(const std::string& text) {
        if (text.empty()) return 0;
        std::istringstream iss(text);
        std::string word;
        int count = 0;
        while (iss >> word) {
            count++;
        }
        return count;
    }
    void reset() {
        statetab.clear();
    }
};
TEST(MarkovChainTest, PrefixFormationWithTwoWords) {
    MarkovChainTester tester(2);
    std::vector<std::string> words = {"hello", "world"};
    Prefix prefix = tester.makePrefix(words);
    EXPECT_EQ(prefix.size(), 2);
    EXPECT_EQ(prefix[0], "hello");
    EXPECT_EQ(prefix[1], "world");
}
TEST(MarkovChainTest, PrefixFormationWithInsufficientWords) {
    MarkovChainTester tester(3);
    std::vector<std::string> words = {"only", "two"};
    Prefix prefix = tester.makePrefix(words);
    EXPECT_EQ(prefix.size(), 3);
    EXPECT_EQ(prefix[0], "only");
    EXPECT_EQ(prefix[1], "two");
    EXPECT_EQ(prefix[2], "");
}
TEST(MarkovChainTest, PrefixSuffixEntryCreation) {
    MarkovChainTester tester(2);
    Prefix prefix;
    prefix.push_back("quick");
    prefix.push_back("brown");
    tester.addEntry(prefix, "fox");
    std::vector<std::string> suffixes = tester.getSuffixes(prefix);
    EXPECT_EQ(suffixes.size(), 1);
    EXPECT_EQ(suffixes[0], "fox");
}
TEST(MarkovChainTest, SingleSuffixSelection) {
    MarkovChainTester tester(2);
    std::vector<std::string> suffixes = {"only_suffix"};
    for (int i = 0; i < 10; ++i) {
        std::string selected = tester.selectRandomSuffix(suffixes);
        EXPECT_EQ(selected, "only_suffix");
    }
}
TEST(MarkovChainTest, MultipleSuffixesRandomSelection) {
    MarkovChainTester tester(2);
    std::vector<std::string> suffixes = {"suffix1", "suffix2", "suffix3", 
    "suffix4", "suffix5"};
    std::map<std::string, int> selectionCount;
    for (int i = 0; i < 100; ++i) {
        std::string selected = tester.selectRandomSuffix(suffixes);
        selectionCount[selected]++;
    }
    for (const auto& suffix : suffixes) {
        EXPECT_GT(selectionCount[suffix], 0) << "Suffix " << suffix;
    }
    EXPECT_EQ(selectionCount.size(), suffixes.size());
}
TEST(MarkovChainTest, TextGenerationWithFixedLength) {
    MarkovChainTester tester(2);
    Prefix prefix1;
    prefix1.push_back("the");
    prefix1.push_back("quick");
    tester.addEntry(prefix1, "brown");
    Prefix prefix2;
    prefix2.push_back("quick");
    prefix2.push_back("brown");
    tester.addEntry(prefix2, "fox");
    Prefix prefix3;
    prefix3.push_back("brown");
    prefix3.push_back("fox");
    tester.addEntry(prefix3, "jumps");
    Prefix prefix4;
    prefix4.push_back("fox");
    prefix4.push_back("jumps");
    tester.addEntry(prefix4, "over");
    Prefix startPrefix;
    startPrefix.push_back("the");
    startPrefix.push_back("quick");
    std::string text = tester.generateText(3, startPrefix);
    int wordCount = tester.countWords(text);
    EXPECT_EQ(wordCount, 5);
    EXPECT_TRUE(text.find("the quick brown fox jumps") != std::string::npos);
}
TEST(MarkovChainTest, GenerationStopsWhenNoSuffix) {
    MarkovChainTester tester(2);
    Prefix prefix;
    prefix.push_back("hello");
    prefix.push_back("world");
    tester.addEntry(prefix, "!");
    Prefix startPrefix;
    startPrefix.push_back("hello");
    startPrefix.push_back("world");
    std::string text = tester.generateText(100, startPrefix);
    int wordCount = tester.countWords(text);
    EXPECT_LE(wordCount, 3);
}
TEST(MarkovChainTest, EmptyPrefixFormation) {
    MarkovChainTester tester(2);
    std::vector<std::string> emptyWords;
    Prefix prefix = tester.makePrefix(emptyWords);
    EXPECT_EQ(prefix.size(), 2);
    EXPECT_EQ(prefix[0], "");
    EXPECT_EQ(prefix[1], "");
}
TEST(MarkovChainTest, PrefixSizeThreeTest) {
    MarkovChainTester tester(3);
    std::vector<std::string> words = {"one", "two", "three"};
    Prefix prefix = tester.makePrefix(words);
    EXPECT_EQ(prefix.size(), 3);
    EXPECT_EQ(prefix[0], "one");
    EXPECT_EQ(prefix[1], "two");
    EXPECT_EQ(prefix[2], "three");
    tester.addEntry(prefix, "four");
    std::vector<std::string> suffixes = tester.getSuffixes(prefix);
    EXPECT_EQ(suffixes.size(), 1);
    EXPECT_EQ(suffixes[0], "four");
}
TEST(MarkovChainTest, ComplexGenerationWithMultipleBranches) {
    MarkovChainTester tester(2);
    Prefix prefix1;
    prefix1.push_back("I");
    prefix1.push_back("am");
    tester.addEntry(prefix1, "happy");
    tester.addEntry(prefix1, "sad");
    Prefix prefix2;
    prefix2.push_back("am");
    prefix2.push_back("happy");
    tester.addEntry(prefix2, "because");
    Prefix prefix3;
    prefix3.push_back("happy");
    prefix3.push_back("because");
    tester.addEntry(prefix3, "I");
    Prefix prefix4;
    prefix4.push_back("am");
    prefix4.push_back("sad");
    tester.addEntry(prefix4, "very");
    Prefix prefix5;
    prefix5.push_back("sad");
    prefix5.push_back("very");
    tester.addEntry(prefix5, "sad");
    Prefix startPrefix;
    startPrefix.push_back("I");
    startPrefix.push_back("am");
    std::string text = tester.generateText(10, startPrefix);
    int wordCount = tester.countWords(text);
    EXPECT_GT(wordCount, 0);
    EXPECT_EQ(text.find("I am"), 0);
    std::vector<std::string> validWords = {"I", "am", "happy", "sad",
    "because", "very"};
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        bool found = false;
        for (const auto& vw : validWords) {
            if (word == vw) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Unexpected word: " << word;
    }
}
