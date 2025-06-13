#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <string>

void hierarchicalClustering(
    const std::vector<std::vector<double>>& distMatrix,
    const std::string& method,
    const std::string& outputDir
);

#endif
