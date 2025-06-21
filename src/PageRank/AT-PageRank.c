#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// PageRank № 1 - Грубый подход с использованием транспонированной матрицы

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

   	/********************** ИНИЦИАЛИЗАЦИЯ МАТРИЦ СМЕЖНОСТИ **************************/

	// Начинаем замерять время работы алгоритма
	clock_t begin, end;
	double time_spent;
	begin = clock();

	int i = 0, j = 0;
    int from, to;

    float **a = (float **)malloc(n * sizeof(float *));
	for (i = 0; i < n; i++) {
	  	a[i] = (float *)calloc(n, sizeof(float));
	}

	// Читаем файл и заполняем матрицу смежности
	while (!feof(fp)) {
	    fscanf(fp, "%d%d", &from, &to);
	    a[from][to] = 1.0;
	}

	// Закрываем файл с набором данных
	fclose(fp);
    
    float **at = (float **)malloc(n * sizeof(float *));
	for (i = 0; i < n; i++) {
	    at[i] = (float *)calloc(n, sizeof(float));
	}

	float *p = (float *)malloc(n * sizeof(float));
	for(i = 0; i < n; i++) {
		p[i] = 1.0 / n;
	}

	/*********************** СТОХАСТИЗАЦИЯ И ТРАНСПОНИРОВАНИЕ МАТРИЦЫ  ***********************/

	int *out_link = (int *)calloc(n, sizeof(int));  
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (a[i][j] != 0.0) {
				out_link[i]++;
			}
		}
	}

	// Стохастизируем матрицу
	for (i = 0; i < n; i++) {
		if (out_link[i] == 0) {
			// Корректируем свисающие узлы
			for (j = 0; j < n; j++) {
				a[i][j] = 1.0 / (float)n;
			}
		} else {
			for (j = 0; j < n; j++) {
				if (a[i][j] != 0) {
					a[i][j] = a[i][j] / (float)out_link[i];
				}
			}
		}
	}

	// Транспонирование матрицы
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			at[j][i] = a[i][j];
		}
	}

	/*************************** PageRank **************************/

	// Коэффициент демпфирования
	const float d = 0.85;

	// Условие для цикла и счётчик итераций
	int looping = 1;
	int k = 0;

	float *p_new = (float *)malloc(n * sizeof(float));

	while (looping) {
		for (i = 0; i < n; i++) {
			p_new[i] = 0.0;
		}

		// Обновляем вектор без учёта коэффициента демпфирования
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				p_new[i] = p_new[i] + (at[i][j] * p[j]);
			}
		} 

		// Обновляем вектор с учётом коэффициента демпфирования
		for (i = 0; i < n; i++) {
		 	p_new[i] = d * p_new[i] + (1.0 - d) / n;
		}

		// Останавливаем цикл, если векторы становятся почти идентичны
	    float stop = 0.0;
	    for (i = 0; i < n; i++ ) {
	        stop = stop + fabs(p_new[i] - p[i]);
	    }
	    if (stop < 0.000001) {
	        looping = 0;
	    }

	    // Обновляем вектор с рейтингом
	    for (i = 0; i < n; i++) {
	    	p[i] = p_new[i];
		}
	    k++; // Увеличиваем счётчик итераций
	}

	/*************************** ВЫВОД *******************************/

	// Рассчитываем итоговое время работы алгоритма
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		
	// Выводим результат: 
	printf ("\nFinal Pagerank values:\n\n[");
	for (i = 0; i < 10; i++) {
		printf("%f ", p[i]);
		if (i != (n - 1)) {
            printf(", ");
        }
	}
	printf("]\n\nNumber of iteration to converge: %d\n\nTime spent: %lf seconds.\n", k, time_spent);
	
	// Освобождаем выделенную память
	for (i = 0; i < n; i++) {
		free(a[i]);
	}
	free(a);

	for (i = 0; i < n; i++) {
		free(at[i]);
	}
	free(at);

	free(p);
	free(out_link);
	free(p_new);

    return EXIT_SUCCESS;
}