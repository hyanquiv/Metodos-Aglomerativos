
# 📁 Proyecto: Clustering Jerárquico (C++)

Este proyecto implementa tres estrategias de clustering jerárquico aglomerativo usando una matriz de distancias triangular inferior como entrada:

- **Distancia mínima**  
- **Distancia máxima** 
- **Distancia promedio**

## 📥 Entrada

Un archivo `.txt` con la matriz triangular inferior de distancias entre elementos, por ejemplo:

```
1
2    41.621436
3    18.047724    36.185017
4    30.021817    24.691644    23.839064
...
```

Ubicar este archivo como:  
`data/distancia.txt`

## 📤 Salida

Se generan 3 carpetas de salida:

- `data/distancia_minima_output/`  
- `data/distancia_maxima_output/`  
- `data/distancia_promedio_output/`

Cada una contiene:

- `matrices_pasos.txt`: matrices de distancia en cada paso de fusión  
- `fusiones.txt`: clusters que se unieron y la distancia  
- `dendrograma.txt`: resumen jerárquico de fusiones


## 📂 Estructura de archivos

```
Bio_Lab06.cpp (main)
clustering.h / clustering.cpp
io_utils.h / io_utils.cpp
dendrogram_utils.h / dendrogram_utils.cpp
data/dist_matrix.txt
```
