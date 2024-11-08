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
		cout << title << " - �����: " << author << ", ��� �������: " << year << "." << endl;
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
		cerr << "���� �� ������." << endl;
	}
	return countbooks;
}

void titleSortBooks(Library books[], int countbooks, int sort) {
	cout << "\n";
	if (sort != 1 && sort != 2) {
		cout << "�������� �����." << endl;
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
		cout << "����� ������� ������������� �� ������." << endl;
	}
	if (sort==2) {
		cout << "����� ������� ������������� �� ���� �������." << endl;
	}
	if (sort == 1 or sort == 2) {
		cout << "\n";
		for (int i = 0; i < countbooks; i++) {
			cout << i + 1 << ". " << books[i].title << " - �����: " << books[i].author << ", ��� �������: " << books[i].year << "\n" << endl;
		}
		fstream f("output.txt", ios::out | ios::app);
		if (f.is_open()) {
			if (sort == 1) {
				f << "���������� ���������� ���� �� ������:\n";
			}
			if (sort == 2) {
				f << "���������� ���������� ���� �� ���� �������:\n";
			}
			for (int i = 0; i < countbooks; i++) {
				f << i + 1 << ". " << books[i].title << " - �����: " << books[i].author << ", ��� �������: " << books[i].year << "\n" << endl;
			}
			f.close();
		}
	}
}

void saverbooks(Library books[], int countbooks) {
	ifstream file("books.txt");
	if (!file.is_open()) {
		cerr << "���� �� ������." << endl;
	}
	fstream outfile("books.txt", ios::out | ios::app);
	if (outfile.is_open()) {
		for (int i = 0; i < countbooks; ++i) {
			outfile << books[i].title << "," << books[i].author << "," << to_string(books[i].year) << endl;
		}
		outfile.close();
	}
	else {
		cerr << "���������� ������� ����." << endl;
	}
	try {
		ifstream infile("books.txt");
	}
	catch(const ifstream::failure& e) {
		cerr << "���������� �������/��������� ����." << endl;
	}
}

void addbooks(Library books[], int countbooks, int Max_books) {
	if (countbooks >= Max_books) {
		cout << "���������� �������� ����� �����. ��������� ����� �� ����������.";
		return;
	}

	Library newbook;
	cout << "������� �������� ����� �����: ";
	cin.ignore();
	getline(cin, newbook.title);
	cout << "������� ������ ����� �����: ";
	getline(cin, newbook.author);
	cout << "������� ��� ������� ����� �����: ";
	cin >> newbook.year;

	books[countbooks] = newbook;
	ifstream file("books.txt");
	if (!file.is_open()) {
		cerr << "���� �� ������." << endl;
	}
	fstream outfile("books.txt", ios::out | ios::app);
	if (outfile.is_open()) {
		outfile << books[countbooks].title << "," << books[countbooks].author << "," << to_string(books[countbooks].year) << endl;
		outfile.close();
	}
	else {
		cerr << "���������� ������� ����." << endl;
	}
	try {
		ifstream infile("books.txt");
	}
	catch (const ifstream::failure& e) {
		cerr << "���������� �������/��������� ����." << endl;
	}
	fstream f("output.txt", ios::out | ios::app);
	if (f.is_open()) {
		f << "����� �����:\n" << books[countbooks].title << " - �����: " << books[countbooks].author << ", ��� �������: " << books[countbooks].year << "\n" << endl;
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
				f << "��������� �����:\n" << books[i].title << " - �����: " << books[i].author << ", ��� �������: " << books[i].year << "\n" << endl;
				f.close();
			}
		}
	}
	if (!findbook) {
		cout << "����� �� �������." << endl;
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
					f << "���������� ������ ����, �������� ����� ���������� ����:\n";
				}
			}
			findbook = true;
			if (f.is_open()) {
				f << books[i].title << " - �����: " << books[i].author << ", ��� �������: " << books[i].year << "\n" << endl;
			}
		}
	}
	f.close();
	if (!findbook) {
		cout << "�� ������� �����, �������� ������ ���������� ����." << endl;
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
					f << "���������� ������ ���� ���������� ������:\n";
				}
			}
			findbook = true;
			if (f.is_open()) {
				f << books[i].title << " - �����: " << books[i].author << ", ��� �������: " << books[i].year << "\n" << endl;
			}
		}
	}
	f.close();
	if (!findbook) {
		cout << "�� ������� ����� ���������� ������." << endl;
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
		cout << "������� �������� ����� " << countbooks + 1 << ": ";
		getline(cin, title);

		if (title.empty()) {
			break;
		}

		string author;
		cout << "������� ������ ����� " << countbooks + 1 << ": ";
		getline(cin, author);

		int year;
		cout << "������� ��� ������� ����� " << countbooks + 1 << ": ";
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
		cout << "1. ��������\n";
		cout << "2. �����\n";
		cout << "3. ����������\n";
		cout << "4. �����\n";
		cout << "0. ��������� ���������\n";
		cout << "\n";

		cout << "������� ��� �����: ";
		cin >> vibor;
		cout << "\n";

		switch (vibor) {
			case 1:
				addbooks(books, countbooks, Max_books);
				countbooks++;
				loadbooks(books, countbooks, Max_books);
				break;
			case 2:
				cout << "������� �������� ����� ��� ������: ";
				cin.ignore();
				getline(cin, title);
				searchbooks(books, countbooks, title);
				break;
			case 3:
				cout << "���������� ��:\n1. ������\n2. ����\n ������� ��� �����: ";
				cin >> sort;
				titleSortBooks(books, countbooks, sort);
				break;
			case 4:
				cout << "����� ���� ��:\n1. ������\n2. ����, ����� ����������\n������� ��� �����: ";
				cin >> showchoice;
				cout << "\n";
				if (showchoice == 1) {
					cout << "����������, ������� ������: ";
					cin.ignore();
					getline(cin, author);
					searchbyauthor(books, countbooks, author);
				}
				if (showchoice == 2) {
					cout << "����������, ������� ���: ";
					cin >> year;
					searchbyyear(books, countbooks, year);
				}
				if (showchoice!=1 && showchoice!=2) {
					cout << "�������� �����." << endl;
				}
				break;
			case 0:
				cout << "��������� ���������.\n";
				break;
			default:
				cout << "�������� �����. ���������� �����.\n";
		}
	} while (vibor != 0);

	return 0;
}