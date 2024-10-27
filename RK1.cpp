#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

struct Library {
	char* title;
	char* author;
	int year;
	int copies;
};

int stringCompare(const char* str1, const char* str2) {
	while (*str1 != '\0' && *str2 != '\0') {
		if (*str1 != *str2) {
			return (*str1 > *str2) ? 1 : -1;
		}
		++str1;
		++str2;
	}
	if (*str1 == '\0' && *str2 == '\0') {
		return 0;
	}
	return (*str1 == '\0') ? -1 : 1;
}

void sortBooks(Library* books, int countbooks) {
	for (int i = 0; i < countbooks-1; ++i) {
		for (int j = 0; j < countbooks - i - 1; ++j) {
			if (stringCompare(books[j].title, books[j + 1].title) > 0) {
				Library temp = books[j];
				books[j] = books[j + 1];
				books[j + 1] = temp;
			}
		}
	}
}

int main() {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);

	int countbooks;
	cout << "Ââåäèòå êîëè÷åñòâî êíèã â áèáëèîòåêå: ";
	cin >> countbooks;
	cin.ignore();

	Library* books = new Library[countbooks];

	for (int i = 0; i < countbooks; ++i) {
		books[i].title = new char[100];
		cout << "Ââåäèòå íàçâàíèå êíèãè " << i + 1 << ": ";
		cin.getline(books[i].title, 100);

		books[i].author = new char[100];
		cout << "Ââåäèòå àâòîðà êíèãè " << i + 1 << ": ";
		cin.getline(books[i].author, 100);

		cout << "Ââåäèòå ãîä èçäàíèÿ êíèãè " << i + 1 << ": ";
		cin >> books[i].year;

		cout << "Ââåäèòå êîëè÷åñòâî êîïèé êíèãè " << i + 1 << ": ";
		cin >> books[i].copies;

		cin.ignore();
		cout << endl;
	}

	sortBooks(books, countbooks);

	for (int i = 0; i < countbooks; ++i) {
		cout << i + 1 << ". " << books[i].title << " - Àâòîð: " << books[i].author << ", Ãîä èçäàíèÿ: " << books[i].year << ", Êîëè÷åñòâî êîïèé: " << books[i].copies << endl;
	}

	for (int i = 0; i < countbooks; ++i) {
		delete[] books[i].title;
		delete[] books[i].author;
	}
	delete[] books;

	return 0;
}
