#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <vector>
#include <string>

std::vector<std::vector<double>> readDistanceMatrix(const std::string& filename);
void writeMatrixStep(const std::vector<std::vector<double>>&, int step, const std::string& dir);
void writeFusionStep(const std::vector<int>&, const std::vector<int>&, double, const std::string& dir);

#endif
