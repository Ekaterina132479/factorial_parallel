#include <iostream>
#include <cmath>
#include <omp.h>

#define PI 3.14159265358979323846

//функция, которую интегрируем
double f(double x) {
    return sin(x);
}

//метод прямоугольников
double sequential_integrate(double a, double b, int N) {
    double h = (b - a) / N; //шаг разбиения
    double sum = 0.0; //сумма площадей прямоугольников
    for (int i = 0; i < N; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return sum * h;
}

//параллельный метод прямоугольников
double parallel_integrate(double a, double b, int N, int num_threads) {
    double h = (b - a) / N;
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum) num_threads(num_threads)
    for (int i = 0; i < N; i++) {
        double x = a + i * h;
        sum += f(x);
    }

    return sum * h;
}

int main() {
    double a = 0.0, b = PI; //интервал интегрирования
    int N = 1000000; //количество разбиений
    int num_threads = 4; 

    //аналитическое решение интеграла от 0 до пи
    double exact_value = 2.0;

    //время однопоточного метода
    double start_time = omp_get_wtime();
    double seq_result = sequential_integrate(a, b, N);
    double seq_time = omp_get_wtime() - start_time;

    //время многопоточного метода
    start_time = omp_get_wtime();
    double par_result = parallel_integrate(a, b, N, num_threads);
    double par_time = omp_get_wtime() - start_time;

    std::cout << "Analytical value: " << exact_value << "\n";
    std::cout << "Sequential method: " << seq_result << ", time: " << seq_time << " sec\n";
    std::cout << "Parallel method: " << par_result << ", time: " << par_time << " sec\n";

    return 0;
}
