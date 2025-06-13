#include "clustering.h"
#include "io_utils.h"
#include <iostream>

int main() {
    std::string inputFile = "data/distancia.txt";

    auto matrix = readDistanceMatrix(inputFile);

    hierarchicalClustering(matrix, "min", "data/distancia_minima_output");
    hierarchicalClustering(matrix, "max", "data/distancia_maxima_output");
    hierarchicalClustering(matrix, "avg", "data/distancia_promedio_output");

    std::cout << "Clustering completado." << std::endl;
    return 0;
}
