#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
using namespace std;

struct Library {
	string title;
	string author;
	int year;

	void show() const {
		cout << title << " - Автор: " << author << ", Год издания: " << year << "." << endl;
	}
};

int loadbooks(Library books[], int countbooks, int Max_books) {
	ifstream infile("books.txt");
	if (infile.is_open()) {
		string line;
		while (getline(infile, line) && countbooks < Max_books) {
			size_t pos1 = line.find(',');
			size_t pos2 = line.find(',', pos1 + 1);
			books[countbooks].title = line.substr(0, pos1);
			books[countbooks].author = line.substr(pos1 + 1, pos2 - pos1 - 1);
			books[countbooks].year = stoi(line.substr(pos2 + 1));
			countbooks++;
		}
		infile.close();
	}
	else {
		cerr << "Файл не найден." << endl;
	}
	return countbooks;
}

void titleSortBooks(Library books[], int countbooks, int sort) {
	cout << "\n";
	if (sort != 1 && sort != 2) {
		cout << "Неверный выбор." << endl;
	}
	for (int i = 0; i < countbooks - 1; ++i) {
		for (int j = 0; j < countbooks - i - 1; ++j) {
			bool needtoswap = false;
			if (sort==1) {
				needtoswap = books[j].author > books[j + 1].author;
			}
			if (sort==2) {
				needtoswap = books[j].year > books[j + 1].year;
			}
			if (needtoswap) {
				swap(books[j], books[j + 1]);
			}
		}
	}
	if (sort==1) {
		cout << "Книги успешно отсортированы по автору." << endl;
	}
	if (sort==2) {
		cout << "Книги успешно отсортированы по году выпуска." << endl;
	}
	if (sort == 1 or sort == 2) {
		cout << "\n";
		for (int i = 0; i < countbooks; i++) {
			cout << i + 1 << ". " << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << "\n" << endl;
		}
		fstream f("output.txt", ios::out | ios::app);
		if (f.is_open()) {
			if (sort == 1) {
				f << "Результаты сортировки книг по автору:\n";
			}
			if (sort == 2) {
				f << "Результаты сортировки книг по году выпуска:\n";
			}
			for (int i = 0; i < countbooks; i++) {
				f << i + 1 << ". " << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << "\n" << endl;
			}
			f.close();
		}
	}
}

void saverbooks(Library books[], int countbooks) {
	ifstream file("books.txt");
	if (!file.is_open()) {
		cerr << "Файл не найден." << endl;
	}
	fstream outfile("books.txt", ios::out | ios::app);
	if (outfile.is_open()) {
		for (int i = 0; i < countbooks; ++i) {
			outfile << books[i].title << "," << books[i].author << "," << to_string(books[i].year) << endl;
		}
		outfile.close();
	}
	else {
		cerr << "Невозможно открыть файл." << endl;
	}
	try {
		ifstream infile("books.txt");
	}
	catch(const ifstream::failure& e) {
		cerr << "Невозможно открыть/прочитать файл." << endl;
	}
}

void addbooks(Library books[], int countbooks, int Max_books) {
	if (countbooks >= Max_books) {
		cout << "Невозможно добавить новую книгу. Достигнут лимит по количеству.";
		return;
	}

	Library newbook;
	cout << "Введите название новой книги: ";
	cin.ignore();
	getline(cin, newbook.title);
	cout << "Введите автора новой книги: ";
	getline(cin, newbook.author);
	cout << "Введите год издания новой книги: ";
	cin >> newbook.year;

	books[countbooks] = newbook;
	ifstream file("books.txt");
	if (!file.is_open()) {
		cerr << "Файл не найден." << endl;
	}
	fstream outfile("books.txt", ios::out | ios::app);
	if (outfile.is_open()) {
		outfile << books[countbooks].title << "," << books[countbooks].author << "," << to_string(books[countbooks].year) << endl;
		outfile.close();
	}
	else {
		cerr << "Невозможно открыть файл." << endl;
	}
	try {
		ifstream infile("books.txt");
	}
	catch (const ifstream::failure& e) {
		cerr << "Невозможно открыть/прочитать файл." << endl;
	}
	fstream f("output.txt", ios::out | ios::app);
	if (f.is_open()) {
		f << "Новая книга:\n" << books[countbooks].title << " - Автор: " << books[countbooks].author << ", Год издания: " << books[countbooks].year << "\n" << endl;
		f.close();
	}
}

void searchbooks(Library books[], int countbooks, string title) {
	bool findbook = false;
	cout << "\n";
	for (int i = 0; i < countbooks; i++) {
		if (books[i].title == title) {
			books[i].show();
			findbook = true;
			fstream f("output.txt", ios::out | ios::app);
			if (f.is_open()) {
				f << "Найденная книга:\n" << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << "\n" << endl;
				f.close();
			}
		}
	}
	if (!findbook) {
		cout << "Книга не найдена." << endl;
	}
}

void searchbyyear(Library books[], int countbooks, int year) {
	bool findbook = false;
	fstream f("output.txt", ios::out | ios::app);
	for (int i = 0; i < countbooks; i++) {
		if (books[i].year <= year) {
			books[i].show();
			
			if (f.is_open()) {
				if (!findbook) {
					f << "Результаты поиска книг, изданных ранее указанного года:\n";
				}
			}
			findbook = true;
			if (f.is_open()) {
				f << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << "\n" << endl;
			}
		}
	}
	f.close();
	if (!findbook) {
		cout << "Не найдены книги, изданные раньше указанного года." << endl;
	}
}

void searchbyauthor(Library books[], int countbooks, string author) {
	bool findbook = false;
	fstream f("output.txt", ios::out | ios::app);
	for (int i = 0; i < countbooks; i++) {
		if (books[i].author == author) {
			books[i].show();
			if (f.is_open()) {
				if (!findbook) {
					f << "Результаты поиска книг указанного автора:\n";
				}
			}
			findbook = true;
			if (f.is_open()) {
				f << books[i].title << " - Автор: " << books[i].author << ", Год издания: " << books[i].year << "\n" << endl;
			}
		}
	}
	f.close();
	if (!findbook) {
		cout << "Не найдены книги указанного автора." << endl;
	}
}

int main() {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);

	int countbooks = 0;
	const int Max_books = 100;

	Library* books = new Library[Max_books];
	countbooks = loadbooks(books, countbooks, Max_books);

	while (countbooks < Max_books) {
		string title;
		cout << "Введите название книги " << countbooks + 1 << ": ";
		getline(cin, title);

		if (title.empty()) {
			break;
		}

		string author;
		cout << "Введите автора книги " << countbooks + 1 << ": ";
		getline(cin, author);

		int year;
		cout << "Введите год издания книги " << countbooks + 1 << ": ";
		cin >> year;
		cin.ignore();

		books[countbooks++] = { title, author, year };
	}
	saverbooks(books, countbooks);

	int vibor;
	string title;
	string author;
	int year;
	int sort;
	int showchoice;
	do {
		cout << "\n";
		cout << "1. Добавить\n";
		cout << "2. Поиск\n";
		cout << "3. Сортировка\n";
		cout << "4. Вывод\n";
		cout << "0. Завершить программу\n";
		cout << "\n";

		cout << "Введите ваш выбор: ";
		cin >> vibor;
		cout << "\n";

		switch (vibor) {
			case 1:
				addbooks(books, countbooks, Max_books);
				countbooks++;
				loadbooks(books, countbooks, Max_books);
				break;
			case 2:
				cout << "Введите название книги для поиска: ";
				cin.ignore();
				getline(cin, title);
				searchbooks(books, countbooks, title);
				break;
			case 3:
				cout << "Сортировка по:\n1. Автору\n2. Году\n Введите ваш выбор: ";
				cin >> sort;
				titleSortBooks(books, countbooks, sort);
				break;
			case 4:
				cout << "Вывод книг по:\n1. Автору\n2. Году, ранее указанного\nВведите ваш выбор: ";
				cin >> showchoice;
				cout << "\n";
				if (showchoice == 1) {
					cout << "Пожалуйста, укажите автора: ";
					cin.ignore();
					getline(cin, author);
					searchbyauthor(books, countbooks, author);
				}
				if (showchoice == 2) {
					cout << "Пожалуйста, укажите год: ";
					cin >> year;
					searchbyyear(books, countbooks, year);
				}
				if (showchoice!=1 && showchoice!=2) {
					cout << "Неверный выбор." << endl;
				}
				break;
			case 0:
				cout << "Программа завершена.\n";
				break;
			default:
				cout << "Неверный выбор. Попробуйте снова.\n";
		}
	} while (vibor != 0);

	return 0;
}