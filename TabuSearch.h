//
// Created by karol on 16.12.2021.
//

#ifndef PEA_ZAD2_TABUSEARCH_H
#define PEA_ZAD2_TABUSEARCH_H

#include <windows.h>
#include <random>
#include <functional>
#include <iostream>

using namespace std;

class TabuSearch {
private:
	struct pathInfo {
		int *path;
		int cost;
	};

	int **array;
	int N;
	int neighborDefinition;
	pathInfo oldPath;
	pathInfo newPath;
	pathInfo bestPath;
	int stop;
	int maxTabuListSize;
	random_device rd;


public:
	TabuSearch(int **array, int N, int stop, int neighborDefinition);

	~TabuSearch();

private:
	static long long int read_QPC();

	static void swap(int &a, int &b);

	int calcPathCost(const int *path);

	tuple <int, int> randomNeighbor1(int *path);

	tuple<int, int, int>  randomNeighbor2(int *path);

	tuple <int, int> randomNeighbor3(int *path);

	void randomPath(int *path);

	void setStartPath();

	void copyPath(const int *src, int *dest);

	void showResults(long long int moment);

public:
	void findPath();

};


#endif //PEA_ZAD2_TABUSEARCH_H
