#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// PageRank № 2 - Оптимизация CRS (метод сжатых разрежённых строк)

int main(int argc, char *argv[]) {
    // Проверка на наличие аргументов командной строки
    if (argc < 2) {
        fprintf(stderr, "[Error] No input file specified!\n");
        fprintf(stderr, "Usage: %s <filename.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // Открываем файл с данными
    FILE *fp;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "[Error] Cannot open the file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
  
    // Чтение данных и получение количества узлов (n) и рёбер (e)
    int n, e;
    char ch;
    char str[100];
    ch = getc(fp);
    while (ch == '#') {
        fgets(str, 100 - 1, fp);
        sscanf (str,"%*s %d %*s %d", &n, &e); // Количество узлов и рёбер
        ch = getc(fp);
    }
    ungetc(ch, fp);

    // Вывод информации о прочитанных данных
    printf("\nGraph data:\n\n  Nodes: %d, Edges: %d\n", n, e);
  
    /************************* CSR СТРУКТУРА *****************************/
    
    /* CRS - Формат сжатых разрежённых строк:
       - Вектор val: содержит 1.0, если в строке существует ребро
       - Вектор col_ind: содержит индекс столбца соответствующего значения в val
       - Вектор row_pt: указывает на начало каждой строки в col_ind
    */

    // Начинаем замерять время работы алгоритма
	clock_t begin, end;
	double time_spent;
	begin = clock();

    float *val = (float *)calloc(e, sizeof(float));
    int *col_ind = (int *)calloc(e, sizeof(int));
    int *row_ptr = (int *)calloc(n + 1, sizeof(int));
  
    int from, to;
    int cur_row = 0;
    int i = 0, j = 0;
    int elrow = 0; // Элемент строки
    int curel = 0; // Совокупное число элементов
  
    while (!feof(fp)) {    
        fscanf(fp, "%d%d", &from, &to);
        if (from > cur_row) { // Меняем строку
            curel = curel + elrow;
            for (int k = cur_row + 1; k <= from; k++) {
                row_ptr[k] = curel;
            }
            elrow = 0;
            cur_row = from;
        }
        val[i] = 1.0;
        col_ind[i] = to;
        elrow++;
        i++;
    }
    row_ptr[cur_row + 1] = curel + elrow - 1;

    // Закрываем файл с набором данных
    fclose(fp);

    // Корректируем стохастизацию
    int *out_link = (int *)calloc(n, sizeof(int));
    int rowel = 0;
    for (i = 0; i < n; i++) {
        if (row_ptr[i+1] != 0) {
            rowel = row_ptr[i+1] - row_ptr[i];
            out_link[i] = rowel;
        }
    }
  
    int curcol = 0;
    for (i = 0; i < n; i++) {
        rowel = row_ptr[i+1] - row_ptr[i];
        for (j = 0; j < rowel; j++) {
            val[curcol] = val[curcol] / out_link[i];
            curcol++;
        }
    }

    /***************** ИНИЦИАЛИЗАЦИЯ ДЛЯ P-ВЕКТОРА РЕЙТИНГА, ДЕМПИНГ ФАКТОР **********************/

    // Коэффициет демпфирования
    const float d = 0.85;

    // Инициализация вектора p
    float *p = (float *)malloc(n * sizeof(float));
    for (i = 0; i < n; i++) {
        p[i] = 1.0 / n;
    }
  
    /*************************** PageRank **************************/

    // Счётчик итераций и условие цикла PageRank
    int looping = 1;
    int k = 0;
    
    // Инициализация вектора p_new
    float *p_new = (float *)malloc(n * sizeof(float));
    while (looping) {
        // p_new инициализируется как вектор из n элементов со значением 0.0
        for (i = 0; i < n; i++) {
            p_new[i] = 0.0;
        }
    
        rowel = 0;
        curcol = 0;
    
        // Оптимизированный алгоритм PageRank
        for (i = 0; i < n; i++) {
            rowel = row_ptr[i + 1] - row_ptr[i];
            for (j = 0; j < rowel; j++) {
                p_new[col_ind[curcol]] = p_new[col_ind[curcol]] + val[curcol] * p[i];
                curcol++;
            }
        }

        // Корректировка для висящих вершин графа 
        for (i = 0; i < n; i++) {
            p_new[i] = d * p_new[i] + (1.0 - d) / n;
        }
       
        // ПРЕКРАЩЕНИЕ ИТЕРИРОВАНИЯ: проверка, нужно ли циклу остановиться
        float stop = 0.0;
        for (i = 0; i < n; i++) {
            stop = stop + fabs(p_new[i] - p[i]);
        }
        // Останавливаемся, если две последовательности вектора PageRank почти идентичны
        if (stop < 0.000001) {
            looping = 0;
        }
    
        // Обновляем вектор p
        for (i = 0; i < n; i++) {
            p[i] = p_new[i];
        }
        k++; // Увеличиваем число итераций цикла PageRank
    }
  
    /*************************** ВЫВОД *******************************/

    // Остановка таймера и подсчёт времени работы алгоритма PageRank в секундах
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    
    // Вывод результата
    printf("\nFinal PageRank values:\n\n[");
    for (i = 0; i < 10; i++){
        printf("%f ", p[i]);
        if (i != (n - 1)) {
            printf(", ");
        }
    }
    printf("]\n\nNumber of iteration to converge: %d\n\nTime spent: %lf seconds.\n", k, time_spent);
    
    // Освобождаем выделенную память
    free(val);
    free(col_ind);
    free(row_ptr);
    free(p);
    free(out_link);
    free(p_new);

    return EXIT_SUCCESS;
}