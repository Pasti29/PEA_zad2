//
// Created by karol on 13.12.2021.
//

#ifndef PEA_ZAD2_SIMULATEDANNEALING_H
#define PEA_ZAD2_SIMULATEDANNEALING_H

#include <windows.h>
#include <random>
#include <functional>
#include <iostream>

using namespace std;

class SimulatedAnnealing {
private:
	int **array;
	int N;
	int *oldPath;
	int *newPath;
	int *bestPath;
	double temperature;
	float alpha;
	int stop;
	int bestCost;
	random_device rd;


public:
	SimulatedAnnealing(int **array, int N, float alpha, int stop);

	~SimulatedAnnealing();

private:
	static long long int read_QPC();

	static void swap(int &a, int &b);

	int calcPathCost(const int *path);

	void randomNeighbor(int *path);

	void setStartPath();

	void copyPath(const int *src, int *dest);

	double setStartTemperature(int cost);

	void calcNewTemperature();

	void showResults(long long int moment);

public:
	void findPath();
};


#endif //PEA_ZAD2_SIMULATEDANNEALING_H
