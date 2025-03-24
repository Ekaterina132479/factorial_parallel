#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>
#include <chrono>  

using namespace cv;
using namespace std;
using namespace chrono;

void drawSierpinski(Mat& image, int x, int y, int size, int depth) {
    if (depth == 0) {
        rectangle(image, Point(x, y), Point(x + size, y + size), Scalar(0, 0, 0), FILLED);
        return;
    }
    int newSize = size / 3;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == 1 && j == 1) continue; // Пропускаем центральный квадрат
            drawSierpinski(image, x + i * newSize, y + j * newSize, newSize, depth - 1);
        }
    }
}

int main() {
    int size, depth, threads;
    cout << "Vvedite razmer izobrageniua (N x N): ";
    cin >> size;
    cout << "Vveditw glubiny recurcii: ";
    cin >> depth;
    cout << "Vvedite kolichestvo potocov: ";
    cin >> threads;

    omp_set_num_threads(threads);
    Mat image = Mat::ones(size, size, CV_8UC3) * 255;

    // Замер времени начала
    auto start = high_resolution_clock::now();

    drawSierpinski(image, 0, 0, size, depth);

    // Замер времени окончания
    auto end = high_resolution_clock::now();

    // Расчет времени выполнения
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "lead time : " << duration.count() << " milliseconds" << endl;

    imwrite("sierpinski_carpet.png", image);
    imshow("Sierpinski Carpet", image);
    waitKey(0);
    return 0;
}
