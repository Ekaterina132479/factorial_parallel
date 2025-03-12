#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>  // Для usleep (анимация в терминале)
#include <omp.h>

using namespace std;

const int SIZE = 100;  // Размер игрового поля (уменьшен для читаемости)
const double LIVE_PROBABILITY = 0.2; // Вероятность появления живой клетки
const char ALIVE = '0', DEAD = '.'; // Символы для визуализации

// Функция для очистки экрана
void clear_screen() {
    cout << "\033[H\033[J"; // ANSI-код для очистки экрана
}

// Функция для инициализации поля
void initialize(vector<vector<char>>& grid) {
    srand(time(nullptr));
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            grid[i][j] = (rand() / (double)RAND_MAX < LIVE_PROBABILITY) ? ALIVE : DEAD;
}

// Подсчёт живых соседей
int count_live_neighbors(const vector<vector<char>>& grid, int x, int y) {
    int live_neighbors = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // Пропускаем саму клетку
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && grid[nx][ny] == ALIVE) {
                ++live_neighbors;
            }
        }
    }
    return live_neighbors;
}

// Обновление состояния поля (параллельная обработка)
void update_grid(const vector<vector<char>>& current, vector<vector<char>>& next) {
#pragma omp parallel for collapse(2) 
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            int live_neighbors = count_live_neighbors(current, i, j);
            if (current[i][j] == ALIVE) {
                next[i][j] = (live_neighbors == 2 || live_neighbors == 3) ? ALIVE : DEAD;
            }
            else {
                next[i][j] = (live_neighbors == 3) ? ALIVE : DEAD;
            }
        }
    }
}

// Визуализация поля с цветами
void print_grid(const vector<vector<char>>& grid, int iteration) {
    clear_screen(); // Очищаем экран перед каждой отрисовкой
    cout << "Итерация: " << iteration << "\n";
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (grid[i][j] == ALIVE) {
                cout << "\033[32mO\033[0m ";  // Зелёный цвет живых клеток
            }
            else {
                cout << "\033[90m.\033[0m ";  // Серый цвет мёртвых клеток
            }
        }
        cout << endl;
    }
    usleep(100000); // Задержка 100 мс (анимация)
}

int main() {
    vector<vector<char>> grid(SIZE, vector<char>(SIZE));
    vector<vector<char>> next_grid(SIZE, vector<char>(SIZE));

    initialize(grid);
    int iteration = 0;

    while (true) {  // Бесконечный цикл, можно прервать Ctrl + C
        print_grid(grid, iteration);
        update_grid(grid, next_grid);
        grid.swap(next_grid); // Меняем местами текущий и следующий кадр
        iteration++;
    }

    return 0;
}

