// ConsoleApplication1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <omp.h>
#include <vector>
#include "utils.hpp"

using namespace std;
using namespace utils;

int main(int64_t argc, char* argv[]) {


	string file1(argv[1]);
	string file2(argv[2]);
	if (argc == 6) {
		createFile(file1, atoi(argv[3]), atoi(argv[4]));
		createFile(file2, atoi(argv[4]), atoi(argv[5]));
	}
	else {
		int maxThreadNum = 4;
		omp_set_num_threads(maxThreadNum);
		vector<vector<double>> a = fromFileToMatrix(file1);
		vector<vector<double>> b = fromFileToMatrix(file2);
		int mode = argv[3] != nullptr ? atoi(argv[3]) : 1;
		int chunkSize = argv[4] != nullptr ? atoi(argv[4]) : 1;
		vector<vector<double>> result;
		int64_t rows = a.size();
		int64_t inter = b.size();
		int64_t columns = b[0].size();
		char buf[1000];
		int res = -1;
		ULONGLONG startTime = GetTickCount64(), timeMultiply;
		switch (mode) {
		case(1): {
			result = oneThread(a, b);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "oneThread: %I64dx%I64d on %I64dx%I64d, time %I64d ms\n", rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(2): {
			result = parallelStatic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "parallelStatic, chunkSize = %d: %I64dx%I64d on %I64dx%I64d; time %I64d ms\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(3): {
			result = parallelDynamic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "parallelDynamic, chunkSize = %d: %I64dx%I64d on %I64dx%I64d; time %I64d ms\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(4): {
			result = parallelGuided(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "parallelGuided, chunkSize = %d: %I64dx%I64d on %I64dx%I64d; time %I64d ms\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		default: break;
		}
		string str = "Ошибка!";
		if (res >= 0 && res < sizeof(buf))
			str = buf;
		cout << str << endl;
		createLog(str);
		createResultFile(result);
	}
	return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
