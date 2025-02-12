#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

int shared_counter = 0;  // счётчик
std::mutex mtx;          // мьютекс

// функция с гонками данных
void increment_unsafe(int thread_id) {
    for (int i = 0; i < 1000; ++i) {
        ++shared_counter; 
    }
}

// функция с синхронизацией
void increment_safe(int thread_id) {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); 
        ++shared_counter;
    }
}

// запуск потоков
void run_threads(void (*func)(int), const std::string& description) {
    shared_counter = 0;  
    const int num_threads = 5;
    std::vector<std::thread> threads;

    std::cout << description << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();  // старт времени

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(func, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();  // конец времени
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << "Final counter value: " << shared_counter << "\n";
    std::cout << "Execution time: " << duration.count() << " seconds\n\n";
}

int main() {
    run_threads(increment_unsafe, "1:");     // выдаёт рандомный счётчик
    run_threads(increment_safe, "2:");       // всегда выдаёт 5000

    return 0;
}
