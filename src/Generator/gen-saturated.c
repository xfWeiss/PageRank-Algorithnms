#include <stdio.h>
#include <stdlib.h>

// Генератор насыщенного веб-орграфа для тестирования алгоритма PageRank 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[Error] No input number vertices web-graph\n");
        fprintf(stderr, "Usage: %s <num vertices\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Инициализируем количество генерируемых вершин
    int num_nodes = atoi(argv[1]);
    if (num_nodes <= 0) {
        fprintf(stderr, "[Error] Number of vertices must be greater than zero\n");
        return EXIT_FAILURE;
    }

    // Открываем файл для записи
    FILE *fp = fopen("saturated-web-graph.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "[Error] Cannot create or open the file\n");
        return EXIT_FAILURE;
    }

    // Инициализируем счётчик рёбер графа и итераторы циклов
    int num_edges = 0;
    int i = 0, j = 0;

    // Генерируем граф
    fprintf(fp, "0 0\n");
    num_edges++;
    while (i < num_nodes) {
        // Каждая вершина ссылается на все предыдущие
        while (j < i) {
            fprintf(fp, "%d %d\n", i, j);
            num_edges++;
            j++;
        }
        i++;
        j = 0;
    }

    // Закрываем файл
    fclose(fp);
    printf ("[Success] Saturated web graph with %d vertices and %d edges successfully generated in file \"web-graph.txt\"\n", num_nodes, num_edges);
    
    return EXIT_SUCCESS;
}