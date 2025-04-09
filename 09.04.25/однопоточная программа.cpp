#include <iostream>
#include <vector>
#include <complex>
#include "lodepng.h"
#include <chrono>

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_ITER = 1000;


int mandelbrot(std::complex<double> c) {
    std::complex<double> z = 0;
    int iter = 0;
    while (std::abs(z) < 2.0 && iter < MAX_ITER) {
        z = z * z + c;
        iter++;
    }
    return iter;
}


void color_pixel(int iter, unsigned char& r, unsigned char& g, unsigned char& b) {
    if (iter == MAX_ITER) {
        r = g = b = 0;  
    }
    else {
        
        r = (unsigned char)(255 * sin(0.16 * iter) * 0.8);  
        g = (unsigned char)(255 * sin(0.13 * iter + 1) * 0.8);  
        b = (unsigned char)(255 * sin(0.12 * iter + 2) * 0.8); 
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<unsigned char> image(WIDTH * HEIGHT * 4);  

    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            
            double real = -2.0 + x * 3.0 / WIDTH;  
            double imag = -1.5 + y * 3.0 / HEIGHT; 
            std::complex<double> c(real, imag);

            
            int iter = mandelbrot(c);
            unsigned char r, g, b;
            color_pixel(iter, r, g, b);

            int idx = 4 * (y * WIDTH + x);
            image[idx + 0] = r;      
            image[idx + 1] = g;      
            image[idx + 2] = b;      
            image[idx + 3] = 255;   
        }
    }

   
    unsigned error = lodepng::encode("mandelbrot.png", image, WIDTH, HEIGHT);
    if (error) {
        std::cerr << "Ошибка сохранения PNG: " << lodepng_error_text(error) << "\n";
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Вычисление заняло: " << duration.count() << " секунд.\n";

    return 0;
}
