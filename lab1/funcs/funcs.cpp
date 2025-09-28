#include <iostream>

bool isVowel(char c) {
    c = std::tolower(c);
    return c == 'e' || c == 'u' || c == 'i' || c == 'o' || c == 'a' ||
           c == 'у' || c == 'е' || c == 'ы' || c == 'а' || c == 'о' ||
           c == 'э' || c == 'я' || c == 'и' || c == 'ю';
}

std::string removeVowels(const std::string& userString) {
    std::string resultString;
    for (char c : userString) {
        if (!isVowel(c)) {
            resultString += c;
        }
        return resultString;
    }
}