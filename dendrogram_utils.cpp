#include "dendrogram_utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

void updateDendrogram(const vector<int>& clusterA, const vector<int>& clusterB, double distance, const string& dir) {
    ofstream out(dir + "/dendrograma.txt", ios::app);
    out << "Union ("; 
    for (int i : clusterA) out << i + 1 << " ";
    out << ") + (";
    for (int j : clusterB) out << j + 1 << " ";
    out << ") -> dist: " << setprecision(12) << distance << "\n";
}
