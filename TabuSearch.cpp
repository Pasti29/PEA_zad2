//
// Created by karol on 16.12.2021.
//

#include "TabuSearch.h"

/*
 * Konstruktor inicjalizujący wszystkie potrzebne zmienne.
 */
TabuSearch::TabuSearch(int **array, int N, int stop, int neighborDefinition) {
	this->array = array;
	this->N = N;
	this->stop = stop;
	this->neighborDefinition = neighborDefinition;
	maxTabuListSize = N;
	bestPath.path = new int[N];
	oldPath.path = new int[N];
	newPath.path = new int[N];
	setStartPath();
	bestPath.cost = calcPathCost(bestPath.path);
	copyPath(bestPath.path, oldPath.path);
}

/*
 * Dekonstruktor niszczący używane tabele dynamiczne.
 */
TabuSearch::~TabuSearch() {
	delete[] bestPath.path;
	delete[] oldPath.path;
	delete[] newPath.path;
}

/*
 * Funkcja potrzebna do obliczania czasu.
 */
long long int TabuSearch::read_QPC() {
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return ((long long int) count.QuadPart);
}

/*
 * Funkcja zmieniająca miejscami dwie liczby.
 */
void TabuSearch::swap(int &a, int &b) {
	int tmp = a;
	a = b;
	b = tmp;
}

/*
 * Funkcja obliczająca koszt ścieżki podanej jako parametr.
 */
int TabuSearch::calcPathCost(const int *path) {
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
tuple<int, int> TabuSearch::randomNeighbor1(int *path) {
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
	return {random1, random2};
}

/*
 * Funkcja znajdująca sąsiada podanej w parametrze ścieżki.
 * Sąsiad jest znajdowany zmieniając 3 losowe węzły miejscami ze
 * sobą.
 */
tuple<int, int, int> TabuSearch::randomNeighbor2(int *path) {
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, N - 1);
	auto random = bind(distribution, gen);

	int random1, random2, random3;
	random1 = random();

	// Pętla zapobiega wylosowaniu się dwóch tych samych węzłów.
	do {
		random2 = random();
	} while (random1 == random2);

	// Pętla zapobiega wylosowaniu się dwóch tych samych węzłów.
	do {
		random3 = random();
	} while (random1 == random3 || random2 == random3);
	swap(path[random1], path[random3]);
	swap(path[random2], path[random3]);
	return {random1, random2, random3};
}

/*
 * Funkcja znajdująca sąsiada podanej w parametrze ścieżki.
 * Sąsiad jest znajdowany znajdując 2 losowe węzły i odwracane są
 * węzły na łuku wskazanym przez te dwa wylosowane węzły.
 */
tuple<int, int> TabuSearch::randomNeighbor3(int *path) {
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, N - 1);
	auto random = bind(distribution, gen);

	int random1, random2;
	random1 = random();
	do {
		random2 = random();
	} while (random1 == random2);

	for (int i = 0; i < abs(random1 - random2 + 1) / 2; ++i) {
		if (random1 < random2) {
			swap(path[random1 + i], path[random2 - i]);
		} else {
			swap(path[random2 + i], path[random1 - i]);
		}
	}
	return {random1, random2};
}

/*
 * Funkcja mieszająca ścieżkę podaną w parametrze. Losowane są dwa węzły i
 * zmieniane śa ich miejsca w ścieżce. Proces jest powtarzany N liczbę razy, gdzie N
 * to długość ścieżki
 */
void TabuSearch::randomPath(int *path) {
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, N - 1);
	auto random = bind(distribution, gen);

	int random1, random2;
	for (int i = 0; i < N; ++i) {
		random1 = random();

		// Pętla zapobiega wylosowaniu się dwóch tych samych węzłów.
		do {
			random2 = random();
		} while (random1 == random2);
		swap(path[random1], path[random2]);
	}
}

/*
 * Funkcja znajdująca początkowe rozwiązanie.
 * Zastosowano algorytm najbliższego sąsiada, czyli znajdowany jest losowy
 * węzeł i od niego tworzona jest krawędź do kolejnego najbliższego węzła i tak
 * dalej, aż wszystkie węzły zostaną użyte w grafie.
 */
void TabuSearch::setStartPath() {
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
	bestPath.path[0] = startingNode;
	visited[startingNode] = true;

	for (int i = 1; i < N; ++i) {
		bestLocalCost = INT_MAX;
		for (int j = 0; j < N; ++j) {
			if (array[bestPath.path[i - 1]][j] < bestLocalCost && bestPath.path[i - 1] != j && !visited[j]) {
				bestLocalCost = array[bestPath.path[i - 1]][j];
				bestLocal = j;
			}
		}
		bestPath.path[i] = bestLocal;
		visited[bestLocal] = true;
	}
}

/*
 * Funkcja kopiuje ścieżkę z src (źródło) do dest (cel).
 */
void TabuSearch::copyPath(const int *src, int *dest) {
	for (int i = 0; i < N; ++i) {
		dest[i] = src[i];
	}
}

/*
 * Wyświetlenie wyników działania algorytmu.
*/
void TabuSearch::showResults(long long int moment) {
	cout << "\tSciezka = ";
	for (int i = 0; i < N; ++i) {
		cout << bestPath.path[i] << " ";
	}
	cout << "\n\tNajlepszy koszt: " << bestPath.cost << "\n"
	     << "\tMoment w czasie [ms]: " << moment << "\n\n";
}

/*
 * Główna funkcja algorytmu.
 */
void TabuSearch::findPath() {
	cout << "\tAlgorytm startuje\n";
	// Zmienne potrzebne do zliczania czasu.
	long long int frequency, start, moment = 0;
	QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);

	// Tablica tabu, maksymalna wielkość jest określona w konstruktorze
	list<int> move;
	list<list<int>> tabuList;
	int count = 0;
	boolean isInTabu;
	int node1, node2, node3 = -1;

	//tabuList.push_back(bestPath);
	oldPath.cost = bestPath.cost;
	copyPath(oldPath.path, newPath.path);

	start = read_QPC();
	while ((read_QPC() - start) / frequency < stop) {
		copyPath(oldPath.path, newPath.path);

		// Znajdowanie sąsiada zależnie od wybranej definicji
		if (neighborDefinition == 1) {
			tie(node1, node2) = randomNeighbor1(newPath.path);
			move.push_back(node1);
			move.push_back(node2);
		} else if (neighborDefinition == 2) {
			tie(node1, node2, node3) = randomNeighbor2(newPath.path);
			move.push_back(node1);
			move.push_back(node2);
			move.push_back(node3);
		} else {
			tie(node1, node2) = randomNeighbor3(newPath.path);
			move.push_back(node1);
			move.push_back(node2);
		}

		newPath.cost = calcPathCost(newPath.path);
		isInTabu = false;

		// Pętla sprawdza, czy sąsiad jest już w tabeli tabu
		for (list path: tabuList) {
			if (neighborDefinition == 1 || neighborDefinition == 2) {
				if (find(path.begin(), path.end(), node1) != path.end() &&
					find(path.begin(), path.end(), node2) != path.end()) {

					isInTabu = true;
					break;
				}
			} else {
				if (find(path.begin(), path.end(), node1) != path.end() &&
				    find(path.begin(), path.end(), node2) != path.end() &&
					find(path.begin(), path.end(), node3) != path.end()) {

					isInTabu = true;
					break;
				}
			}

		}
		count++;
		if (!isInTabu) {
			tabuList.push_back(move);
			if (newPath.cost < oldPath.cost) {
				copyPath(newPath.path, oldPath.path);
				oldPath.cost = newPath.cost;
			}
			if (newPath.cost < bestPath.cost) {
				cout << newPath.cost << "\n";
				copyPath(newPath.path, bestPath.path);
				bestPath.cost = newPath.cost;
				moment = (read_QPC() - start) * 1'000 / frequency;
				count = 0;
			}

			// Jeżeli tabela Tabu jest pełna, to pierwszy ruch jest usuwany
			if (tabuList.size() > maxTabuListSize) {
				tabuList.erase(tabuList.begin());
			}
		}
		move.clear();

		// Dywersyfikacja
		if (count >= N * (N - 1) * (N - 2) * (N - 3)) {
			randomPath(oldPath.path);
			oldPath.cost = calcPathCost(oldPath.path);
			copyPath(oldPath.path, newPath.path);
		}
	}
	showResults(moment);
}
