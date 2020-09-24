#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

namespace utils {
	void createFile(const string& filename, int64_t rows, int64_t columns) {
		ofstream fileout(filename, ios_base::out | ios_base::trunc);
		fileout << rows << " " << columns << endl;
		for (int64_t i = 0; i < rows; i++) {
			for (int64_t j = 0; j < columns; j++) {
				fileout << (double)(rand()) / rand() << " ";
			}
			fileout << endl;
		}
		fileout.close();
	}

	vector<vector<double>> fromFileToMatrix(const string& filename) {
		vector<vector<double>> matrix;
		int64_t rows, columns;
		ifstream file(filename);
		if (!file) {
			cerr << "Ошибка открытия файла: " << filename << ".\n";
			return matrix;
		}
		file >> rows >> columns;
		if (rows < 1 || columns < 1) {
			cerr << "Количество строк и стобцов должно быть больше 0.\n";
			return matrix;
		}
		matrix.resize(rows);
		for (auto& row : matrix) {
			row.resize(columns);
		}
		for (auto& row : matrix) {
			for (auto& num : row) {
				file >> num;
			}
		}
		file.close();
		return matrix;
	}

	void createResultFile(vector<vector<double>> result) {
		int64_t rows = result.size();
		int64_t columns = result[0].size();
		ofstream fileout("res.txt", ios_base::trunc);
		fileout << rows << " " << columns << endl;
		for (int64_t i = 0; i < rows; i++) {
			for (int64_t j = 0; j < columns; j++) {
				fileout << result[i][j] << " ";
			}
			fileout << endl;
		}
		fileout.close();
	}

	void createLog(string& str) {
		ofstream filelog("log.txt", ios::out | ios::app);
		filelog << str;
		filelog.close();
	}

	void printMatrix(vector<vector<double>> matrix) {
		for (int i = 0; i < matrix.size(); i++) {
			for (int j = 0; j < matrix.size(); j++) {
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	vector<vector<double>> oneThread(vector<vector<double>>& a, vector<vector<double>>& b) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
		for (int64_t row = 0; row < rows1; row++) {
			for (int64_t column = 0; column < columns2; column++) {
				double sum = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					sum += a[row][inter] * b[inter][column];
				}
				result[row][column] = sum;
			}
		}
		return result;
	}

	vector<vector<double>> parallelStatic(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(static, chunkSize)
			for (int inner = 0; inner < a[0].size(); inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		}
		else if (rows1 < 4) {
#pragma omp parallel for schedule(static,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1 * columns2; i++) {
				int64_t column = i % columns2;
				int64_t row = i / columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(static,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}

	vector<vector<double>> parallelDynamic(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(dynamic, chunkSize)
			for (int inner = 0; inner < inter21; inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		}
		else if (rows1 < 4) {
#pragma omp parallel for schedule(dynamic,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1 * columns2; i++) {
				int64_t column = i % columns2;
				int64_t row = i / columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(dynamic,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}

	vector<vector<double>> parallelGuided(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		vector<std::vector<double>> result(rows1, vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(guided, chunkSize)
			for (int inner = 0; inner < inter21; inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		}
		else if (rows1 < 4) {
#pragma omp parallel for schedule(guided,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1 * columns2; i++) {
				int64_t column = i % columns2;
				int64_t row = i / columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(guided,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}
}