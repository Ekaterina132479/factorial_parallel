#include <mpi.h>
#include <iostream>
#include <vector>
#include <complex>
#include "lodepng.h"
#include <chrono> 

// определяем размеры изображений и максимальное количество итераций
const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_ITER = 1000;

// функция для вычисления количества итераций, которые необходимы для того, чтобы комплексное число c покинуло область фрактала Мандельброта
// начинаем с z=0 и на каждом шаге вычисляем z = z * z + c, если z>2, то точка покидает фрактал и итерации прекращаются
int mandelbrot(std::complex<double> c) {
    std::complex<double> z = 0; // начальная точка
    int iter = 0; // количество итераций
    // цикл выполняется, пока модуль меньше 2 и количество итераций меньше 1000
    while (std::abs(z) < 2.0 && iter < MAX_ITER) {
        z = z * z + c;
        ++iter;
    }
    return iter;
}
// функция для расчёта цвета пикселя в зависимости от количества итераций, чем больше итераций, тем более ярким будет пиксель.
// если итераций больше, чем 1000, то пиксель будет чёрным
void color_pixel(int iter, unsigned char& r, unsigned char& g, unsigned char& b) {
    if (iter == MAX_ITER) {
        r = g = b = 0; // черный, если точка принадлежит множеству Мандельброта
    }
    else {
        double t = (double)iter / MAX_ITER;


        r = (unsigned char)(255 * sin(0.16 * iter) * 0.8);  // красный
        g = (unsigned char)(255 * sin(0.13 * iter + 1) * 0.8);  // зелёный
        b = (unsigned char)(255 * sin(0.12 * iter + 2) * 0.8);  // синий
    }
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // рассчитываем количество строк, которые будут обрабатываться каждым процессом
    int rows_per_proc = HEIGHT / size;
    int start_row = rank * rows_per_proc;
    int end_row = (rank == size - 1) ? HEIGHT : start_row + rows_per_proc;

    // вектор для хранения количества итераций для каждого пикселя
    std::vector<int> local_iters((end_row - start_row) * WIDTH);

    // вычисление количества итераций для каждого пикселя в пределах своего фрагмента
    for (int y = start_row; y < end_row; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double real = -2.0 + x * 3.0 / WIDTH; // реальная часть 
            double imag = -1.5 + y * 3.0 / HEIGHT; // мнимая часть
            std::complex<double> c(real, imag); // комплексное число
            int iter = mandelbrot(c); //количество итераций для числа
            local_iters[(y - start_row) * WIDTH + x] = iter; // сохраняем результат для данного пикселя
        }
    }

    std::vector<int> all_iters;
    if (rank == 0) {
        all_iters.resize(WIDTH * HEIGHT);
    }

    // собираем результаты от всех процессов
    MPI_Gather(local_iters.data(), local_iters.size(), MPI_INT,
        all_iters.data(), local_iters.size(), MPI_INT,
        0, MPI_COMM_WORLD);
    // главный процесс создаёт изображение и сохраняет его в файл
    if (rank == 0) {
        std::vector<unsigned char> image(WIDTH * HEIGHT * 4);
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int iter = all_iters[y * WIDTH + x];  // получаем количество итераций для пикселя
                unsigned char r, g, b;
                color_pixel(iter, r, g, b);  // вычисляем цвет пикселя
                int idx = 4 * (y * WIDTH + x);  // индекс для сохранения в вектор изображения
                image[idx + 0] = r;  // красный
                image[idx + 1] = g;  // зелёный
                image[idx + 2] = b;  // синий
                image[idx + 3] = 255;  // прозрачность
            }
        }
        // сохраняем изображение в файл
        unsigned error = lodepng::encode("mandelbrot.png", image, WIDTH, HEIGHT);
        if (error) {
            std::cerr << "PNG encoder error " << error << ": " << lodepng_error_text(error) << "\n";
        }
        else {
            std::cout << "mandelbrot.png saved successfully!\n";
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    if (rank == 0) {
        std::chrono::duration<double> duration = end - start;
        std::cout << "Execution time: " << duration.count() << " seconds." << std::endl;
    }
    // завершаем работу с mpi
    MPI_Finalize();
    return 0;
}
