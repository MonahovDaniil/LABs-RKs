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

void sortBooks(Library* books, int countbooks) {
	for (int i = 0; i < countbooks-1; ++i) {
		for (int j = 0; j < countbooks - i - 1; ++j) {
			if (strcmp(books[j].title, books[j + 1].title) > 0) {
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
	cout << "Введите количество книг в библиотеке: ";
	cin >> countbooks;
	cin.ignore();

	Library* books = new Library[countbooks];

	for (int i = 0; i < countbooks; ++i) {
		books[i].title = new char[100];
		cout << "Введите название книги " << i + 1 << ": ";
		cin.getline(books[i].title, 100);

		books[i].author = new char[100];
		cout << "Введите автора книги " << i + 1 << ": ";
		cin.getline(books[i].author, 100);

		cout << "Введите год издания книги " << i + 1 << ": ";
		cin >> books[i].year;

		cout << "Введите количество копий книги " << i + 1 << ": ";
		cin >> books[i].copies;

		cin.ignore();
		cout << endl;
	}

	sortBooks(books, countbooks);

	for (int i = 0; i < countbooks; ++i) {
		cout << i + 1 << ". " << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << ", Количество копий: " << books[i].copies << endl;
	}

	for (int i = 0; i < countbooks; ++i) {
		delete[] books[i].title;
		delete[] books[i].author;
	}
	delete[] books;

	return 0;
}