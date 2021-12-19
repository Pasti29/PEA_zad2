#include <iostream>
#include "SimulatedAnnealing.h"
#include "Menu.h"
#include "ArrayClass.h"
#include "TabuSearch.h"

int **ARRAY = nullptr;
int N;

using namespace std;

int main() {
	int option;
	int stop = 30;
	float alpha = 0.99;
	int neighborDefinition = 1;

	while (true) {
		option = Menu::showMainMenu();
		switch (option) {
			case 1:
				if (ARRAY != nullptr) {
					ARRAY = ArrayClass::destroyArray(ARRAY, N);
				}
				ARRAY = ArrayClass::createArrayFromFile(ARRAY, N);
				cout << "\t";
				system("pause");
				break;
			case 2:
				cout << "\tStop [s] (obecnie " << stop << " [s]): ";
				cin >> stop;
				cout << "\t";
				system("pause");
				break;
			case 3:
				cout << "\tDefinicja sasiedztwa (obecnie nr. " << neighborDefinition << "):\n"
				     << "\t1. 2-zamiana\n"
				     << "\t2. 3-zamiana\n"
				     << "\t3. Wymiana lukow\n\t";
				cin >> neighborDefinition;
				if (neighborDefinition < 1 || neighborDefinition > 3) neighborDefinition = 1;
				cout << "\t";
				system("pause");
				break;
			case 4:
				if (ARRAY == nullptr) {
					cout << "\tNiezainicjalozowano tabeli." << endl;
				} else {
					//for (int i = 0; i < 10; ++i) {
					TabuSearch ts(ARRAY, N, stop, neighborDefinition);
					ts.findPath();
					//}
				}
				cout << "\t";
				system("pause");
				break;
			case 5:
				cout << "\tAlpha (obecnie " << alpha << "): ";
				cin >> alpha;
				cout << "\t";
				system("pause");
				break;
			case 6:
				if (ARRAY == nullptr) {
					cout << "\tNiezainicjalozowano tabeli." << endl;
				} else {
					//for (int i = 0; i < 10; ++i) {
					SimulatedAnnealing sa(ARRAY, N, alpha, stop);
					sa.findPath();
					//}
				}
				cout << "\t";
				system("pause");
				break;
			case 9:
				if (ARRAY != nullptr) {
					ARRAY = ArrayClass::destroyArray(ARRAY, N);
				}
				cout << "\t";
				system("pause");
				return 0;
			default:
				break;
		}
	}
}
