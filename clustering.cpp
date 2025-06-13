#include "clustering.h"
#include "io_utils.h"
#include "dendrogram_utils.h"
#include <limits>
#include <fstream>
#include <set>
#include <iomanip>
using namespace std;

/**
 * Calcula la distancia entre dos clusters seg�n el m�todo de enlace especificado
 * @param clusterA: Vector con los �ndices de los elementos del primer cluster
 * @param clusterB: Vector con los �ndices de los elementos del segundo cluster
 * @param originalMatrix: Matriz de distancias original (triangular inferior)
 * @param method: M�todo de enlace ("min", "max", o "avg")
 * @return: Distancia calculada entre los clusters
 */
double computeDistance(
    const vector<int>& clusterA, const vector<int>& clusterB,
    const vector<vector<double>>& originalMatrix,
    const string& method
) {
    double result;
    
    // Inicializaci�n seg�n el m�todo de enlace
    if (method == "min") 
        result = numeric_limits<double>::max();      // Para encontrar el m�nimo
    else if (method == "max") 
        result = numeric_limits<double>::lowest();   // Para encontrar el m�ximo
    else 
        result = 0.0;                               // Para calcular el promedio
    
    int count = 0;  // Contador para el m�todo de promedio
    
    // Doble bucle: examina todos los pares de elementos entre los dos clusters
    for (int a : clusterA) {
        for (int b : clusterB) {
            // Acceso a matriz triangular inferior: si a < b, usa [b][a], sino [a][b]
            double d = (a < b) ? originalMatrix[b][a] : originalMatrix[a][b];
            
            // Aplica el criterio de enlace correspondiente
            if (method == "min") 
                result = min(result, d);        // Single linkage: distancia m�nima
            else if (method == "max") 
                result = max(result, d);        // Complete linkage: distancia m�xima
            else 
                result += d, count++;           // Average linkage: suma para promedio
        }
    }
    
    // Si es m�todo de promedio, divide la suma entre el n�mero de pares
    if (method == "avg") 
        result /= count;
    
    return result;
}

/**
 * Algoritmo principal de clustering jer�rquico aglomerativo
 * @param distMatrix: Matriz de distancias inicial (triangular inferior)
 * @param method: M�todo de enlace a utilizar
 * @param outputDir: Directorio donde guardar los resultados intermedios
 */
void hierarchicalClustering(
    const vector<vector<double>>& distMatrix,
    const string& method,
    const string& outputDir
) {
    int n = distMatrix.size();  // N�mero de elementos a agrupar
    
    // PASO 1: Inicializaci�n - cada elemento forma su propio cluster
    vector<vector<int>> clusters(n);
    for (int i = 0; i < n; ++i) 
        clusters[i].push_back(i);  // Cluster i contiene solo el elemento i
    
    // Copia de la matriz para ir actualiz�ndola en cada paso
    vector<vector<double>> currentMatrix = distMatrix;
    
    int step = 0;  // Contador de pasos para archivos de salida
    
    // Guarda el estado inicial de la matriz
    writeMatrixStep(currentMatrix, step, outputDir);
    
    // ALGORITMO PRINCIPAL: contin�a hasta que quede un solo cluster
    while (clusters.size() > 1) {
        
        // PASO 2: Buscar el par de clusters con menor distancia
        double bestDist = numeric_limits<double>::max();  // Mejor distancia encontrada
        int bestI = -1, bestJ = -1;                      // �ndices de los mejores clusters
        
        // Examina todos los pares de clusters
        for (int i = 0; i < clusters.size(); ++i) {
            for (int j = i + 1; j < clusters.size(); ++j) {
                // Calcula distancia entre cluster i y cluster j
                double d = computeDistance(clusters[i], clusters[j], distMatrix, method);
                
                // Si es la menor distancia hasta ahora, la guarda
                if (d < bestDist) {
                    bestDist = d;
                    bestI = i;
                    bestJ = j;
                }
            }
        }
        
        // PASO 3: Registrar la fusi�n que se va a realizar
        writeFusionStep(clusters[bestI], clusters[bestJ], bestDist, outputDir);
        updateDendrogram(clusters[bestI], clusters[bestJ], bestDist, outputDir);
        
        // PASO 4: Fusionar los dos clusters m�s cercanos
        vector<int> merged = clusters[bestI];  // Copia el primer cluster
        // A�ade todos los elementos del segundo cluster
        merged.insert(merged.end(), clusters[bestJ].begin(), clusters[bestJ].end());
        
        // Elimina los clusters originales (del mayor �ndice al menor para evitar problemas)
        if (bestI > bestJ) 
            swap(bestI, bestJ);  // Asegura que bestI < bestJ
        
        clusters.erase(clusters.begin() + bestJ);  // Elimina primero el de mayor �ndice
        clusters.erase(clusters.begin() + bestI);  // Luego el de menor �ndice
        clusters.push_back(merged);                // A�ade el cluster fusionado
        
        // PASO 5: Recalcular matriz de distancias para el nuevo conjunto de clusters
        vector<vector<double>> newMatrix(clusters.size(), vector<double>(clusters.size(), 0));
        
        // Calcula distancias solo para la parte triangular inferior
        for (int i = 0; i < clusters.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                newMatrix[i][j] = computeDistance(clusters[i], clusters[j], distMatrix, method);
            }
        }
        
        // Actualiza la matriz actual y guarda el paso
        currentMatrix = newMatrix;
        writeMatrixStep(currentMatrix, ++step, outputDir);
    }
    
    // Al final del bucle, queda un solo cluster que contiene todos los elementos
}