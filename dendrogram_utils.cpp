#include "dendrogram_utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;

struct Node {
    set<int> members;
    double distance;
    int level;
    int leftmost, rightmost;
    int centerPos;
};

void updateDendrogram(const string& dir) {
    ifstream in(dir + "/fusiones.txt");
    ofstream out(dir + "/dendrograma.txt");
    
    if (!in || !out) {
        cerr << "Error abriendo archivos en el directorio: " << dir << "\n";
        return;
    }
    
    vector<Node> nodes;
    int maxId = 0;
    string line;
    
    // Leer y procesar las fusiones
    while (getline(in, line)) {
        if (line.empty()) continue;
        
        size_t posa = line.find('{');
        size_t posa_end = line.find('}', posa);
        string A = line.substr(posa + 1, posa_end - posa - 1);
        
        size_t posb = line.find('{', posa_end);
        size_t posb_end = line.find('}', posb);
        string B = line.substr(posb + 1, posb_end - posb - 1);
        
        size_t posd = line.find("dist =");
        double dist = stod(line.substr(posd + 7));
        
        set<int> setA, setB;
        istringstream sa(A), sb(B);
        int id;
        
        while (sa >> id) {
            setA.insert(id);
            maxId = max(maxId, id);
        }
        while (sb >> id) {
            setB.insert(id);
            maxId = max(maxId, id);
        }
        
        set<int> merged = setA;
        merged.insert(setB.begin(), setB.end());
        
        Node node;
        node.members = merged;
        node.distance = dist;
        node.level = nodes.size();
        nodes.push_back(node);
    }
    
    int numLeaves = maxId;
    int spacing = 8; // Más espacio entre nodos
    int width = numLeaves * spacing + 20; // Margen adicional
    int height = nodes.size() * 4 + 10; // Más altura entre niveles
    
    vector<vector<char>> canvas(height, vector<char>(width, ' '));
    
    // Posiciones iniciales de las hojas
    map<set<int>, int> clusterPositions;
    for (int i = 1; i <= numLeaves; ++i) {
        set<int> singleNode = {i};
        clusterPositions[singleNode] = (i - 1) * spacing + spacing/2;
    }
    
    // Procesar cada fusión de abajo hacia arriba
    for (size_t level = 0; level < nodes.size(); ++level) {
        Node& node = nodes[level];
        int currentRow = height - 3 - (level * 4);
        
        // Encontrar subclusters que se están fusionando
        vector<set<int>> subclusters;
        for (const auto& pair : clusterPositions) {
            if (node.members != pair.first) {
                bool isSubset = true;
                for (int member : pair.first) {
                    if (node.members.find(member) == node.members.end()) {
                        isSubset = false;
                        break;
                    }
                }
                if (isSubset) {
                    subclusters.push_back(pair.first);
                }
            }
        }
        
        if (subclusters.size() < 2) continue;
        
        // Encontrar posiciones extremas
        int leftmost = width, rightmost = 0;
        vector<int> positions;
        
        for (const auto& subcluster : subclusters) {
            if (clusterPositions.find(subcluster) != clusterPositions.end()) {
                int pos = clusterPositions[subcluster];
                positions.push_back(pos);
                leftmost = min(leftmost, pos);
                rightmost = max(rightmost, pos);
            }
        }
        
        if (positions.empty()) continue;
        
        int center = (leftmost + rightmost) / 2;
        
        // Dibujar líneas verticales desde cada subcluster
        for (int pos : positions) {
            if (pos >= 0 && pos < width) {
                // Línea vertical hacia arriba
                for (int row = currentRow + 2; row < height - 1; ++row) {
                    if (canvas[row][pos] == ' ') {
                        canvas[row][pos] = '|';
                    }
                }
                // Conexión horizontal
                canvas[currentRow + 1][pos] = '+';
            }
        }
        
        // Dibujar línea horizontal de conexión
        if (leftmost < rightmost && leftmost >= 0 && rightmost < width) {
            for (int col = leftmost; col <= rightmost; ++col) {
                if (canvas[currentRow + 1][col] == ' ') {
                    canvas[currentRow + 1][col] = '-';
                }
            }
        }
        
        // Línea vertical del nuevo nodo
        if (center >= 0 && center < width) {
            canvas[currentRow][center] = '|';
            canvas[currentRow + 1][center] = '+';
        }
        
        // Escribir la distancia
        stringstream ss;
        ss << fixed << setprecision(3) << node.distance;
        string distLabel = ss.str();
        
        int labelStart = max(0, center - (int)distLabel.length()/2);
        labelStart = min(labelStart, width - (int)distLabel.length());
        
        if (labelStart >= 0 && currentRow > 0) {
            for (size_t i = 0; i < distLabel.length() && labelStart + i < width; ++i) {
                canvas[currentRow - 1][labelStart + i] = distLabel[i];
            }
        }
        
        // Actualizar posición del nuevo cluster
        clusterPositions[node.members] = center;
        
        // Remover subclusters fusionados
        for (const auto& subcluster : subclusters) {
            clusterPositions.erase(subcluster);
        }
    }
    
    // Dibujar IDs de las hojas en la base
    for (int i = 1; i <= numLeaves; ++i) {
        int xpos = (i - 1) * spacing + spacing/2;
        string id = to_string(i);
        
        if (xpos >= 0 && xpos < width) {
            // Centrar el ID
            int startPos = xpos - id.length()/2;
            startPos = max(0, startPos);
            startPos = min(startPos, width - (int)id.length());
            
            for (size_t j = 0; j < id.length() && startPos + j < width; ++j) {
                canvas[height - 1][startPos + j] = id[j];
            }
            
            // Línea vertical desde la hoja
            canvas[height - 2][xpos] = '|';
        }
    }
    
    // Agregar encabezado explicativo
    out << "DENDROGRAMA JERARQUICO\n";
    out << "\nLeyenda:\n";
    out << "| = conexion vertical\n";
    out << "- = conexion horizontal\n";
    out << "+ = punto de union\n";
    
    // Escribir el canvas al archivo
    for (const auto& row : canvas) {
        string line;
        for (char c : row) {
            line += c;
        }
        // Remover espacios en blanco al final de la línea
        while (!line.empty() && line.back() == ' ') {
            line.pop_back();
        }
        out << line << '\n';
    }

    in.close();
    out.close();
}