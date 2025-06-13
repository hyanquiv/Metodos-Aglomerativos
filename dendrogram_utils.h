#ifndef DENDROGRAM_UTILS_H
#define DENDROGRAM_UTILS_H

#include <vector>
#include <string>

void updateDendrogram(const std::vector<int>& clusterA, const std::vector<int>& clusterB, double distance, const std::string& dir);

#endif
