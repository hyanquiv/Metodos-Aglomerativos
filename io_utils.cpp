#include "io_utils.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

using namespace std;

vector<vector<double>> readDistanceMatrix(const string& filename) {
    ifstream in(filename);
    string line;
    vector<vector<double>> matrix;

    int row = 0;
    while (getline(in, line)) {
        istringstream iss(line);
        int index;
        iss >> index;
        vector<double> rowData(row);
        for (int i = 0; i < row; ++i) iss >> rowData[i];
        matrix.push_back(rowData);
        row++;
    }

    return matrix;
}

void writeMatrixStep(const vector<vector<double>>& matrix, int step, const string& dir) {
    filesystem::create_directories(dir);
    ofstream out(dir + "/matrices_pasos.txt", ios::app);
    out << "Paso " << step << ":\n";
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < i; ++j)
            out << setw(15) << setprecision(12) << matrix[i][j] << " ";
        out << "\n";
    }
    out << "\n";
}

void writeFusionStep(const vector<int>& A, const vector<int>& B, double d, const string& dir) {
    ofstream out(dir + "/fusiones.txt", ios::app);
    out << "Fusion: { ";
    for (int i : A) out << i + 1 << " ";
    out << "} + { ";
    for (int j : B) out << j + 1 << " ";
    out << "} -> dist = " << setprecision(12) << d << "\n";
}
