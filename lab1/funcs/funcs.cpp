#include <iostream>
#include <funcs.h>

bool isVowel(char c) {
    c = std::tolower(c);
    return c == 'e' || c == 'u' || c == 'i' || c == 'o' || c == 'a';
}

std::string removeVowels(const std::string& userString) {
    std::string resultString;
    for (char c : userString) {
        if (!isVowel(c)) {
            resultString += c;
        }
    }
    return resultString;
}