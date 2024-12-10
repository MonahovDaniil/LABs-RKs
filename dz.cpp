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
				cerr << "������ ������������� ������� ��� ������ ������ �����. ������������ �������� �� ���������.\n" << endl;
			}
			inFile.close();
		}
		else {
			cerr << "���� �� ������. ������������ �������� �� ���������.\n" << endl;
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
			cout << "����: �������." << endl;
		}
		else {
			cout << "����: ��������." << endl;
		}
		cout << "�����������: " << thermostat.getTemperature() << "C." << endl;
		if (securitysystem.getState() == true) {
			cout << "������� ������������: ��������. " << endl;
		}
		else {
			cout << "������� ������������: ���������. " << endl;
		}
		if (curtains.getState() == true) {
			cout << "�����: �������. " << endl;
		}
		else {
			cout << "�����: �������. " << endl;
		}
		if (airconditioner.getState() == true) {
			cout << "�����������: �������." << endl;
		}
		else {
			cout << "�����������: ��������." << endl;
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
			cout << "\n��������� ������ ���� ������� ��������� � ����." << endl;
			outFile.close();
		}
		else {
			cerr << "\n�� ������� ������� ���� ��� ������." << endl;
		}
	}
	~SmartHome() {
		saveState();
	}

	void userInteraction() {
		int choice;
		do {
			cout << "\n�������� ��������:\n" << endl;
			cout << "1. �������� ����" << endl;
			cout << "2. ��������� ����" << endl;
			cout << "3. ���������� �����������" << endl;
			cout << "4. �������� ������� ������������" << endl;
			cout << "5. ��������� ������� ������������" << endl;
			cout << "6. ������� �����" << endl;
			cout << "7. �������� �����" << endl;
			cout << "8. �������� �����������" << endl;
			cout << "9. ��������� �����������" << endl;
			cout << "0. �����" << endl;
			cout << "\n������� �����: ";

			cin >> choice;

			if (choice == 1) {
				controlLight(true);
			}
			if (choice == 2) {
				controlLight(false);
			}
			if (choice == 3) {
				int newTemp;
				cout << "������� �������� �����������: ";
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
				cout << "�������� �����. ���������� �����." << endl;
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