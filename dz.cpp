#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Thermostat {
private:
	int temperature;
public:
	Thermostat() : temperature(20) {}
	void setTemperature(int t) {
		temperature = t;
	}
	int getTemperature() const {
		return temperature;
	}
};

class Light {
private:
	bool isOn;
public:
	Light() : isOn(false) {}
	void turnOn() {
		isOn = true;
	}
	void turnOff() {
		isOn = false;
	}
	string getStatus() const {
		return isOn ? "On" : "Off";
	}
	bool getState() {
		return isOn;
	}
	void setState(bool state) {
		isOn = state;
	}
};

class SecuritySystem {
private:
	bool isArmed;
public:
	SecuritySystem() : isArmed(false) {}
	void arm() {
		isArmed = true;
	}
	void disArm() {
		isArmed = false;
	}
	string getStatus() const {
		return isArmed ? "Armed" : "Disarmed";
	}
	bool getState() {
		return isArmed;
	}
	void setState(bool state) {
		isArmed = state;
	}
};

class Curtains {
private:
	bool isUp;
public:
	Curtains() : isUp(false) {}
	void raise() {
		isUp = true;
	}
	void lower() {
		isUp = false;
	}
	string getStatus() const {
		return isUp ? "Raised" : "Lowered";
	}
	bool getState() {
		return isUp;
	}
	void setState(bool state) {
		isUp = state;
	}
};

class AirConditioner {
private:
	bool isOn;
public:
	AirConditioner() : isOn(false) {}
	void turnOn() {
		isOn = true;
	}
	void turnOff() {
		isOn = false;
	}
	string getStatus() const {
		return isOn ? "On" : "Off";
	}
	bool getState() {
		return isOn;
	}
	void setState(bool state) {
		isOn = state;
	}
};

class SmartHome {
private:
	Thermostat thermostat;
	Light light;
	SecuritySystem securitysystem;
	Curtains curtains;
	AirConditioner airconditioner;
	const string stateFile = "home_state.txt";
public:
	void loadState() {
		ifstream inFile(stateFile);
		if (inFile.is_open()) {
			int temp;
			bool lightState;
			bool securitysystemState;
			bool curtainsState;
			bool airconditionerState;
			if (inFile >> lightState >> temp >> securitysystemState >> curtainsState >> airconditionerState) {
				light.setState(lightState);
				thermostat.setTemperature(temp);
				securitysystem.setState(securitysystemState);
				curtains.setState(curtainsState);
				airconditioner.setState(airconditionerState);
			}
			else {
				cerr << "Первое использование системы или ошибка чтения файла. Используются значения по умолчанию.\n" << endl;
			}
			inFile.close();
		}
		else {
			cerr << "Файл не найден. Используются значения по умолчанию.\n" << endl;
		}
	}
	SmartHome() {
		loadState();
	}
	void setTemperature(int temp) {
		thermostat.setTemperature(temp);
	}
	void controlLight(bool state) {
		if (state = true) {
			light.turnOn();
		}
		else {
			light.turnOff();
		}
	}
	void controlSecurity(bool state) {
		if (state = true) {
			securitysystem.arm();
		}
		else {
			securitysystem.disArm();
		}
	}
	void controlCurtains(bool state) {
		if (state = true) {
			curtains.raise();
		}
		else {
			curtains.lower();
		}
	}
	void controlAirConditioner(bool state) {
		if (state = true) {
			airconditioner.turnOn();
		}
		else {
			airconditioner.turnOff();
		}
	}
	void displayStatus() {
		if (light.getState() == true) {
			cout << "Свет: включен." << endl;
		}
		else {
			cout << "Свет: выключен." << endl;
		}
		cout << "Температура: " << thermostat.getTemperature() << "C." << endl;
		if (securitysystem.getState() == true) {
			cout << "Система безопасности: включена. " << endl;
		}
		else {
			cout << "Система безопасности: отключена. " << endl;
		}
		if (curtains.getState() == true) {
			cout << "Шторы: подняты. " << endl;
		}
		else {
			cout << "Шторы: опущены. " << endl;
		}
		if (airconditioner.getState() == true) {
			cout << "Кондиционер: включен." << endl;
		}
		else {
			cout << "Кондиционер: выключен." << endl;
		}
	}
	
	void saveState() {
		ofstream outFile(stateFile);
		if (outFile.is_open()) {
			if (light.getState() == true) {
				outFile << "1" << endl;
			}
			else {
				outFile << "0" << endl;
			}

			outFile << thermostat.getTemperature() << endl;

			if (securitysystem.getState() == true) {
				outFile << "1" << endl;
			}
			else {
				outFile << "0" << endl;
			}

			if (curtains.getState() == true) {
				outFile << "1" << endl;
			}
			else {
				outFile << "0" << endl;
			}
			if (airconditioner.getState() == true) {
				outFile << "1" << endl;
			}
			else {
				outFile << "0" << endl;
			}
			cout << "\nСостояние умного дома успешно сохранено в файл." << endl;
			outFile.close();
		}
		else {
			cerr << "\nНе удается открыть файл для записи." << endl;
		}
	}
	~SmartHome() {
		saveState();
	}

	void userInteraction() {
		int choice;
		do {
			cout << "\nВыберите действие:\n" << endl;
			cout << "1. Включить свет" << endl;
			cout << "2. Выключить свет" << endl;
			cout << "3. Установить температуру" << endl;
			cout << "4. Включить систему безопасности" << endl;
			cout << "5. Выключить систему безопасности" << endl;
			cout << "6. Поднять шторы" << endl;
			cout << "7. Опустить шторы" << endl;
			cout << "8. Включить кондиционер" << endl;
			cout << "9. Выключить кондиционер" << endl;
			cout << "0. Выход" << endl;
			cout << "\nВведите выбор: ";

			cin >> choice;

			if (choice == 1) {
				controlLight(true);
			}
			if (choice == 2) {
				controlLight(false);
			}
			if (choice == 3) {
				int newTemp;
				cout << "Введите значение температуры: ";
				cin >> newTemp;
				setTemperature(newTemp);
			}
			if (choice == 4) {
				controlSecurity(true);
			}
			if (choice == 5) {
				controlSecurity(false);
			}
			if (choice == 6) {
				controlCurtains(true);
			}
			if (choice == 7) {
				controlCurtains(false);
			}
			if (choice == 8) {
				controlAirConditioner(true);
			}
			if (choice == 9) {
				controlAirConditioner(false);
			}
			else {
				cout << "Неверный выбор. Попробуйте снова." << endl;
			}
		} while (choice != 0);
	}
};

int main() {
	setlocale(LC_ALL, "rus");
	SmartHome home;
	
	home.displayStatus();
	home.userInteraction();
	home.displayStatus();

	return 0;
}