//
// Created by karol on 13.12.2021.
//

#include "SimulatedAnnealing.h"

/*
 * Konstruktor inicjalizujący wszystkie potrzebne zmienne.
 */
SimulatedAnnealing::SimulatedAnnealing(int **array, int N, float alpha, int stop) {
	this->array = array;
	this->N = N;
	this->alpha = alpha;
	this->stop = stop;
	oldPath = new int[N];
	newPath = new int[N];
	bestPath = new int[N];
	setStartPath();
	copyPath(bestPath, oldPath);
	bestCost = calcPathCost(bestPath);
	temperature = setStartTemperature(bestCost);
}
/*
 * Dekonstruktor niszczący używane tabele dynamiczne.
 */
SimulatedAnnealing::~SimulatedAnnealing() {
	delete[] oldPath;
	delete[] newPath;
	delete[] bestPath;
}
/*
 * Funkcja potrzebna do obliczania czasu.
 */
long long int SimulatedAnnealing::read_QPC() {
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return ((long long int) count.QuadPart);
}
/*
 * Funkcja zmieniająca miejscami dwie liczby.
 */
void SimulatedAnnealing::swap(int &a, int &b) {
	int tmp = a;
	a = b;
	b = tmp;
}
/*
 * Funkcja obliczająca koszt ścieżki podanej jako parametr.
 */
int SimulatedAnnealing::calcPathCost(const int *path) {
	int cost = 0;
	for (int i = 0; i < N - 1; ++i) {
		cost += array[path[i]][path[i + 1]];
	}
	cost += array[path[N - 1]][path[0]];
	return cost;
}
/*
 * Funkcja znajdująca sąsiada podanej w parametrze ścieżki.
 * Sąsiad jest znajdowany zmieniając 2 losowe węzły miejscami ze
 * sobą.
 */
void SimulatedAnnealing::randomNeighbor(int *path) {
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, N - 1);
	auto random = bind(distribution, gen);

	int random1, random2;
	random1 = random();

	// Pętla zapobiega wylosowaniu się dwóch tych samych węzłów.
	do {
		random2 = random();
	} while (random1 == random2);
	swap(path[random1], path[random2]);
}
/*
 * Funkcja znajdująca początkowe rozwiązanie.
 * Zastosowano algorytm najbliższego sąsiada, czyli znajdowany jest losowy
 * węzeł i od niego tworzona jest krawędź do kolejnego najbliższego węzła i tak
 * dalej, aż wszystkie węzły zostaną użyte w grafie.
 */
void SimulatedAnnealing::setStartPath() {
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, N - 1);
	auto random = bind(distribution, gen);

	// Tablica odwiedzonych węzłów.
	boolean *visited = new boolean[N];
	for (int i = 0; i < N; ++i) {
		visited[i] = false;
	}
	// bestLocal jest zmienną wskazującą na dotychczas najbliższy węzeł, a
	// bestLocalCost na jego koszt.
	int bestLocal, bestLocalCost;
	int startingNode = random();    // Pierwszy węzeł jest losowy.
	bestPath[0] = startingNode;
	visited[startingNode] = true;

	for (int i = 1; i < N; ++i) {
		bestLocalCost = INT_MAX;
		for (int j = 0; j < N; ++j) {
			if (array[bestPath[i - 1]][j] < bestLocalCost && bestPath[i - 1] != j && !visited[j]) {
				bestLocalCost = array[bestPath[i - 1]][j];
				bestLocal = j;
			}
		}
		bestPath[i] = bestLocal;
		visited[bestLocal] = true;
	}
}
/*
 * Funkcja kopiuje ścieżkę z src (źródło) do dest (cel).
 */
void SimulatedAnnealing::copyPath(const int *src, int *dest) {
	for (int i = 0; i < N; ++i) {
		dest[i] = src[i];
	}
}
/*
 * Funkcja ustawiająca początkową wartość temperatury.
 */
double SimulatedAnnealing::setStartTemperature(int cost) {
	return alpha * cost;
}
/*
 * Funkcja obliczającą nową wartość temperatury.
 */
void SimulatedAnnealing::calcNewTemperature() {
	temperature *= alpha;
}
/*
 * Wyświetlenie wyników działania algorytmu.
*/
void SimulatedAnnealing::showResults(long long int moment) {
	cout << "\tSciezka = ";
	for (int i = 0; i < N; ++i) {
		cout << bestPath[i] << " ";
	}
	cout << "\n\tNajlepszy koszt: " << calcPathCost(bestPath) << "\n"
	     << "\tMoment w czasie [ms]: " << moment << "\n"
	     << "\tTemperatura: " << temperature << "\n\n";
}
/*
 * Główna funkcja algorytmu.
 */
void SimulatedAnnealing::findPath() {
	// Zmienne potrzebne do zliczania czasu.
	long long int frequency, start, moment = 0;
	QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);

	// Zmienne kosztów nowej ścieżki oraz najlepszej lokalnej ścieżki.
	int newCost, oldCost;
	oldCost = bestCost;

	// Inicjacja generatora liczb losowych z zakresu <0; 1>.
	mt19937 gen(rd());
	uniform_real_distribution<> distribution(0, 1);
	auto random = bind(distribution, gen);

	cout << "\tAlgorytm startuje\n";
	start = read_QPC();

	// Pętla główna algorytmu
	while ((read_QPC() - start) / frequency < stop) {
		for (int i = 0; i < N * 50; ++i) {
			copyPath(oldPath, newPath);
			randomNeighbor(newPath);
			newCost = calcPathCost(newPath);

			// Jeżeli nowa ścieżka jest lepsza od globalnie najlepszej, to zmieniana
			// jest najlepsza globalna ścieżka.
			if (newCost < bestCost) {
				//cout << newCost << "\n";
				copyPath(newPath, bestPath);
				bestCost = newCost;
				moment = (read_QPC() - start) * 1'000 / frequency;
			}

			int delta = newCost - oldCost;
			if (delta < 0) {
				copyPath(newPath, oldPath);
				oldCost = newCost;
			} else {
				double x = random();
				if (x < exp(-delta / temperature)) {
					copyPath(newPath, oldPath);
					oldCost = newCost;
				}
			}
		}
		calcNewTemperature();
	}
	if (oldCost < bestCost) {
		copyPath(oldPath, bestPath);
		bestCost = oldCost;
		moment = (read_QPC() - start) * 1'000 / frequency;
	}
	showResults(moment);
}



