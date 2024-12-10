#include <iostream>
#include <list>
#include <string>
using namespace std;

class ElectronicDevice {
protected:
	string brand, model;
public:
	ElectronicDevice(const string& brand, const string& model) : brand(brand), model(model) {}

	virtual ~ElectronicDevice() {}

	virtual void display() const {
		cout << "Бренд устройства: " << brand << ", Модель устройства: " << model << endl;
	}

	virtual void modify() = 0;
};

class Smartphone : public ElectronicDevice {
private:
	string os;
public:
	Smartphone(const string& brand, const string& model, const string& os) : ElectronicDevice(brand, model), os(os) {}

	void display() const override {
		cout << "Бренд смартфона - " << brand << ", Модель - " << model << ", Операционная система - " << os << endl;
	}

	void modify() override {
		os = "One UI 7.0";
		cout << "Характеристики смартфона обновлены! Новая операционная система - " << os << endl;
	}
};

class Laptop : public ElectronicDevice {
private:
	string screensize;
public:
	Laptop(const string& brand, const string& model, const string& screensize) : ElectronicDevice(brand, model), screensize(screensize) {}

	void display() const override {
		cout << "Бренд ноутбука - " << brand << ", Модель - " << model << ", Размер экрана: " << screensize << endl;
	}

	void modify() override {
		model = "Huawei Matebook D16";
		screensize = "16 дюймов";
		cout << "Характеристики ноутбука обновлены! Новая модель - " << model << ", Новый размер экрана - " << screensize << endl;
	}
};

void modifyDevice(ElectronicDevice* device) {
	device->modify();
}

int main() {
	setlocale(LC_ALL, "rus");

	list<ElectronicDevice*> devices;

	devices.push_back(new Smartphone("Samsung", "Samsung Galaxy A54 5G", "One UI 6.0"));
	devices.push_back(new Laptop("Huawei", "Huawei Matebook D15", "15.6 дюймов"));
	devices.push_back(new Smartphone("Apple", "Iphone 15 Pro Max", "IOS"));
	devices.push_back(new Laptop("Apple", "Macbook Air 13", "MacOS"));

	for (const auto& device : devices) {
		device->display();
	}

	cout << "\n";

	list<ElectronicDevice*>::iterator ptr1 = devices.begin();
	modifyDevice(*ptr1);

	list<ElectronicDevice*>::iterator ptr2 = next(devices.begin());
	modifyDevice(*ptr2);

	for (const auto& device : devices) {
		delete device;
	}

	return 0;
}