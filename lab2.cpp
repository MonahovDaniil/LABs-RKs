#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <climits>
using namespace std;
int lengthOfWord(const char* stroka) {
    int length = 0;
    while (*stroka != '\0') {
        ++length;
        ++stroka;
    }
    return length;
}
int numberWords(const char* str, const char* delimiters) {
    char buffer[300];
    strcpy(buffer, str);
    char* token = strtok(buffer, delimiters);
    int count = 0;
    while (token != nullptr) {
        if (*token == 'a') {
            ++count;
        }
        token = strtok(nullptr, delimiters);
    }
    return count;
}
int shortWord(const char* str, const char* delimiters) {
    char buffer[300];
    strcpy(buffer, str);
    int minlength = INT_MAX;
    char* token = strtok(buffer, delimiters);
    while (token != nullptr) {
        int length = lengthOfWord(token);
        if (length < minlength) {
            minlength = length;
        }
        token = strtok(nullptr, delimiters);
    }
    return minlength;
}
int aCount(const char* str, const char* delimiters) {
    char buffer[300];
    strcpy(buffer, str);
    int counta = 0;
    int countwords = 0;
    char* token = strtok(buffer, delimiters);
    while (token != nullptr) {
        countwords++;
        if (countwords == 10) {
            while (*token != '\0') {
                if (*token == 'a') {
                    ++counta;
                }
                token++;
            }
        }
        token = strtok(nullptr, delimiters);
    }
    return counta;
}
void replaceLetters(const char* str) {
    char destination[300];
    char* destPtr = destination;
    while (*str != '\0') {
        *destPtr = tolower(*str);
        str++;
        destPtr++;
    }
    *destPtr = '\0';
    cout << "Новая строка: " << destination << endl;
}
int matchWord(const char* str, const char* delimiters) {
    char buffer[300];
    strcpy(buffer, str);
    char* token = strtok(buffer, delimiters);
    int countw = 0;
    while (token != nullptr) {
        char start = *token;
        while (*token != '\0') {
            token++;
        }
        token--;
        char finish = *token;
        if (start == finish) {
            ++countw;
        }
        token = strtok(nullptr, delimiters);
    }
    return countw;
}
char* longestCommonSubstring(const char* str1, const char* str2) {
    int len1 = lengthOfWord(str1);
    int len2 = lengthOfWord(str2);
    int maxLength = 0;
    int endIndex = 0;
    // Проходим по каждому символу первого слова
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            // Если символы совпадают, проверяем на наличие подстроки
            if (str1[i] == str2[j]) {
                int length = 0;
                const char* p1 = str1 + i;
                const char* p2 = str2 + j;

                // Сравниваем подстроки
                while (*p1 && *p2 && *p1 == *p2) {
                    length++;
                    p1++;
                    p2++;
                }

                // Если длина подстроки больше предыдущей, обновляем
                if (length > maxLength) {
                    maxLength = length;
                    endIndex = i;
                }
            }
        }
    }

    // Если не найдено общей подстроки, возвращаем nullptr
    if (maxLength == 0) {
        return nullptr;
    }
    char* commonSubstring = new char[maxLength + 1];
    std::strncpy(commonSubstring, str1 + endIndex, maxLength);
    commonSubstring[maxLength] = '\0'; // Завершаем строку

    return commonSubstring;
}
char* subString(char* words[10]) {
    char* result = nullptr;
    int minLength = INT_MAX;
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (longestCommonSubstring(words[i], words[j])) {
                int length = strlen(longestCommonSubstring(words[i], words[j]));
                char* sstring = longestCommonSubstring(words[i], words[j]);
                if (length < minLength) {
                    minLength = length;
                    delete[] result;
                    result = sstring;
                }
            }
        }
    }
    return result;
}
    int main() {
        setlocale(LC_ALL, "rus");
        const char* delimiters = " ,;";
        const int strsize = 300;
        char str[strsize];
        cin.getline(str, strsize);
        char* words[10];
        char buffer[300];
        strcpy(buffer, str);
        char* token = strtok(buffer, delimiters);
        for (int i = 0; i < 10; i++) {
            words[i] = token;
            token = strtok(nullptr, delimiters);
        }

        cout << "Количество слов, начинающихся с буквы a: " << numberWords(str, delimiters) << endl;
        cout << "Длина самого короткого слова: " << shortWord(str, delimiters) << endl;
        cout << "Количество букв a в последнем слове: " << aCount(str, delimiters) << endl;
        replaceLetters(str);
        cout << "Количество слов, у которых первый и последний символы совпадают: " << matchWord(str, delimiters) << endl;
        char* commonSubString = subString(words);
        if (commonSubString) {
            cout << "Самая короткая общая подстрока: " << subString(words) << endl;
        }
        else {
            cout << "Общая подстрока не найдена" << endl;
        }
        return 0;
    }