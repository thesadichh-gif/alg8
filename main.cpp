#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <queue>
#include <chrono>

using namespace std;

// Узел для собственной очереди (из ЛР №3)
struct UzelOcheredi {
    int versh;
    struct UzelOcheredi* sled;
};

// Собственная очередь на базе односвязного списка (Задание 2*)
struct SvoyaOchered {
    struct UzelOcheredi* golova;
    struct UzelOcheredi* hvost;
};

void och_init(struct SvoyaOchered* q) {
    q->golova = NULL;
    q->hvost = NULL;
}

int och_pusta(struct SvoyaOchered* q) {
    return q->golova == NULL;
}

// Добавление в конец очереди за O(1)
void och_push(struct SvoyaOchered* q, int v) {
    struct UzelOcheredi* nov = (struct UzelOcheredi*)malloc(sizeof(struct UzelOcheredi));
    nov->versh = v;
    nov->sled = NULL;
    if (q->hvost == NULL) {
        q->golova = nov;
        q->hvost = nov;
    } else {
        q->hvost->sled = nov;
        q->hvost = nov;
    }
}

// Извлечение из начала очереди за O(1)
int och_pop(struct SvoyaOchered* q) {
    if (q->golova == NULL) return -1;
    struct UzelOcheredi* tmp = q->golova;
    int v = tmp->versh;
    q->golova = q->golova->sled;
    if (q->golova == NULL) {
        q->hvost = NULL;
    }
    free(tmp);
    return v;
}

void och_ochist(struct SvoyaOchered* q) {
    while (!och_pusta(q)) {
        och_pop(q);
    }
}

// Узел для списков смежности графа
struct UzelSpiska {
    int versh;
    struct UzelSpiska* sled;
};

// Генерация матрицы смежности неориентированного графа без петель
int** sozd_matr(int n, int ver) {
    int** G = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        G[i] = (int*)calloc(n, sizeof(int));
    }
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (rand() % 100 < ver) {
                G[i][j] = 1;
                G[j][i] = 1;
            }
        }
    }
    return G;
}

void ochist_matr(int** G, int n) {
    if (!G) return;
    for (int i = 0; i < n; i++) free(G[i]);
    free(G);
}

void vivod_matr(int** G, int n) {
    printf("Матрица смежности (%d x %d):\n     ", n, n);
    for (int j = 0; j < n; j++) printf("v%-3d", j);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("v%-3d ", i);
        for (int j = 0; j < n; j++) {
            printf("%-4d", G[i][j]);
        }
        printf("\n");
    }
}

// Преобразование матрицы смежности в списки смежности
struct UzelSpiska** sozd_spiski(int** G, int n) {
    struct UzelSpiska** spiski = (struct UzelSpiska**)malloc(n * sizeof(struct UzelSpiska*));
    for (int i = 0; i < n; i++) {
        spiski[i] = NULL;
        struct UzelSpiska* hvost = NULL;
        for (int j = 0; j < n; j++) {
            if (G[i][j] == 1) {
                struct UzelSpiska* nov = (struct UzelSpiska*)malloc(sizeof(struct UzelSpiska));
                nov->versh = j;
                nov->sled = NULL;
                if (spiski[i] == NULL) {
                    spiski[i] = nov;
                    hvost = nov;
                } else {
                    hvost->sled = nov;
                    hvost = nov;
                }
            }
        }
    }
    return spiski;
}

void ochist_spiski(struct UzelSpiska** spiski, int n) {
    if (!spiski) return;
    for (int i = 0; i < n; i++) {
        struct UzelSpiska* tek = spiski[i];
        while (tek) {
            struct UzelSpiska* tmp = tek;
            tek = tek->sled;
            free(tmp);
        }
    }
    free(spiski);
}

void vivod_spiskov(struct UzelSpiska** spiski, int n) {
    printf("Списки смежности:\n");
    for (int i = 0; i < n; i++) {
        printf("v%d: ", i);
        struct UzelSpiska* tek = spiski[i];
        if (!tek) printf("пусто");
        while (tek) {
            printf("%d ", tek->versh);
            tek = tek->sled;
        }
        printf("\n");
    }
}

// 1. Обход в ширину по матрице смежности с std::queue (Задание 1)
void bfs_matr_std(int** G, int n, int start, int* pos, int pokaz) {
    queue<int> Q;
    Q.push(start);
    pos[start] = 1;

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();
        if (pokaz) printf("%d ", v);

        for (int i = 0; i < n; i++) {
            if (G[v][i] == 1 && !pos[i]) {
                pos[i] = 1;
                Q.push(i);
            }
        }
    }
}

// 2. Обход в ширину по спискам смежности с std::queue (Задание 1, п. 3*)
void bfs_spis_std(struct UzelSpiska** spiski, int start, int* pos, int pokaz) {
    queue<int> Q;
    Q.push(start);
    pos[start] = 1;

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();
        if (pokaz) printf("%d ", v);

        struct UzelSpiska* tek = spiski[v];
        while (tek) {
            int sosed = tek->versh;
            if (!pos[sosed]) {
                pos[sosed] = 1;
                Q.push(sosed);
            }
            tek = tek->sled;
        }
    }
}

// 3. Обход в ширину по матрице со своей очередью (Задание 2*)
void bfs_matr_svoya(int** G, int n, int start, int* pos, int pokaz) {
    struct SvoyaOchered Q;
    och_init(&Q);

    och_push(&Q, start);
    pos[start] = 1;

    while (!och_pusta(&Q)) {
        int v = och_pop(&Q);
        if (pokaz) printf("%d ", v);

        for (int i = 0; i < n; i++) {
            if (G[v][i] == 1 && !pos[i]) {
                pos[i] = 1;
                och_push(&Q, i);
            }
        }
    }
    och_ochist(&Q);
}

// 4. Обход в ширину по спискам смежности со своей очередью (Задание 2*)
void bfs_spis_svoya(struct UzelSpiska** spiski, int start, int* pos, int pokaz) {
    struct SvoyaOchered Q;
    och_init(&Q);

    och_push(&Q, start);
    pos[start] = 1;

    while (!och_pusta(&Q)) {
        int v = och_pop(&Q);
        if (pokaz) printf("%d ", v);

        struct UzelSpiska* tek = spiski[v];
        while (tek) {
            int sosed = tek->versh;
            if (!pos[sosed]) {
                pos[sosed] = 1;
                och_push(&Q, sosed);
            }
            tek = tek->sled;
        }
    }
    och_ochist(&Q);
}

// Замер времени в миллисекундах
double vremya_ms() {
    auto t = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t.time_since_epoch()).count();
}

// Процедура ПОШ (обход всех компонент связности) и замер времени
double zapusk_posh(int tip, int n, int** G, struct UzelSpiska** spiski, int start, int pokaz) {
    int* pos = (int*)calloc(n, sizeof(int));

    if (pokaz) printf("Порядок обхода: ");

    double t1 = vremya_ms();

    // Сначала запускаем обход из выбранной стартовой вершины
    if (!pos[start]) {
        if (tip == 1) bfs_matr_std(G, n, start, pos, pokaz);
        else if (tip == 2) bfs_spis_std(spiski, start, pos, pokaz);
        else if (tip == 3) bfs_matr_svoya(G, n, start, pos, pokaz);
        else if (tip == 4) bfs_spis_svoya(spiski, start, pos, pokaz);
    }

    // Если граф несвязный, допроходим оставшиеся компоненты связности
    for (int i = 0; i < n; i++) {
        if (!pos[i]) {
            if (tip == 1) bfs_matr_std(G, n, i, pos, pokaz);
            else if (tip == 2) bfs_spis_std(spiski, i, pos, pokaz);
            else if (tip == 3) bfs_matr_svoya(G, n, i, pos, pokaz);
            else if (tip == 4) bfs_spis_svoya(spiski, i, pos, pokaz);
        }
    }

    double t2 = vremya_ms();

    if (pokaz) {
        printf("\nВремя обхода: %.6f мс\n", t2 - t1);
    }

    free(pos);
    return t2 - t1;
}

// Сравнение времени работы на графах разных порядков (Задание 2*)
void sravnenie_vremeni() {
    int ver = 30;
    printf("Введите вероятность ребра для теста (1-100%%): ");
    scanf("%d", &ver);

    printf("\nСравнение времени обхода (в мс) при плотности %d%%:\n", ver);
    printf("%-8s | %-12s | %-12s | %-12s | %-12s\n",
        "Вершин", "Матр(std)", "Спис(std)", "Матр(своя)", "Спис(своя)");
    printf("----------------------------------------------------------------------\n");

    int razmeri[5] = { 100, 500, 1000, 2000, 3000 };

    for (int s = 0; s < 5; s++) {
        int n = razmeri[s];
        int** G = sozd_matr(n, ver);
        struct UzelSpiska** spiski = sozd_spiski(G, n);

        double tm_std = zapusk_posh(1, n, G, spiski, 0, 0);
        double ts_std = zapusk_posh(2, n, G, spiski, 0, 0);
        double tm_sv  = zapusk_posh(3, n, G, spiski, 0, 0);
        double ts_sv  = zapusk_posh(4, n, G, spiski, 0, 0);

        printf("%-8d | %-9.6f мс | %-9.6f мс | %-9.6f мс | %-9.6f мс\n",
            n, tm_std, ts_std, tm_sv, ts_sv);

        ochist_matr(G, n);
        ochist_spiski(spiski, n);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    int n = 6, ver = 45;
    int** G = sozd_matr(n, ver);
    struct UzelSpiska** spiski = sozd_spiski(G, n);

    int vibor;
    while (1) {
        printf("\n1 - Обход матрицы (std::queue) [Задание 1]\n");
        printf("2 - Обход списков (std::queue) [Задание 1, п. 3*]\n");
        printf("3 - Обход матрицы (своя очередь) [Задание 2*]\n");
        printf("4 - Обход списков (своя очередь) [Задание 2*]\n");
        printf("5 - Сравнение времени на разных размерах графа\n");
        printf("6 - Сгенерировать новый граф\n");
        printf("7 - Показать матрицу и списки смежности\n");
        printf("0 - Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &vibor);

        if (vibor == 0) break;

        if (vibor >= 1 && vibor <= 4) {
            int start = 0;
            printf("Введите стартовую вершину (0..%d): ", n - 1);
            scanf("%d", &start);
            if (start < 0 || start >= n) start = 0;

            printf("\n");
            zapusk_posh(vibor, n, G, spiski, start, 1);
        }
        else if (vibor == 5) {
            sravnenie_vremeni();
        }
        else if (vibor == 6) {
            int nov_n, nov_ver;
            printf("Введите количество вершин и вероятность ребра (%%): ");
            scanf("%d %d", &nov_n, &nov_ver);

            // Сначала освобождаем память со старым размером n!
            ochist_matr(G, n);
            ochist_spiski(spiski, n);

            n = nov_n;
            ver = nov_ver;
            G = sozd_matr(n, ver);
            spiski = sozd_spiski(G, n);

            printf("\nНовый граф сгенерирован:\n");
            if (n <= 20) {
                vivod_matr(G, n);
                vivod_spiskov(spiski, n);
            }
        }
        else if (vibor == 7) {
            printf("\n");
            vivod_matr(G, n);
            printf("\n");
            vivod_spiskov(spiski, n);
        }
    }

    ochist_matr(G, n);
    ochist_spiski(spiski, n);
    return 0;
}