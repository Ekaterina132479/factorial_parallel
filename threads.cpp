#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

int shared_counter = 0;  // ñ÷¸ò÷èê
std::mutex mtx;          // ìüþòåêñ

// ôóíêöèÿ ñ ãîíêàìè äàííûõ
void increment_unsafe(int thread_id) {
    for (int i = 0; i < 1000; ++i) {
        ++shared_counter; 
    }
}

// ôóíêöèÿ ñ ñèíõðîíèçàöèåé
void increment_safe(int thread_id) {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); 
        ++shared_counter;
    }
}

// çàïóñê ïîòîêîâ
void run_threads(void (*func)(int), const std::string& description) {
    shared_counter = 0;  
    const int num_threads = 5;
    std::vector<std::thread> threads;

    std::cout << description << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(func, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << shared_counter << "\n\n";
}

int main() {
    run_threads(increment_unsafe, "1:");
    run_threads(increment_safe, "2:");

    return 0;
}
