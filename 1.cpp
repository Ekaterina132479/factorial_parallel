#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;

mutex mtx;

// Функция для вычисления части факториала
void partial_factorial(int start, int end, cpp_int &result, int thread_id) {
    cpp_int temp = 1;
    for (int i = start; i <= end; ++i) {
        temp *= i;
    }
    {
        lock_guard<mutex> lock(mtx);
        cout << "Поток " << thread_id << " вычислил: " << temp << endl;
        result *= temp;
    }
}

// Основная функция многопоточного факториала
cpp_int parallel_factorial(int n, int num_threads) {
    num_threads = min(n, num_threads);
    vector<thread> threads;
    vector<cpp_int> partial_results(num_threads, 1);

    int chunk_size = n / num_threads;
    int remainder = n % num_threads;
    int start = 1;

    for (int i = 0; i < num_threads; ++i) {
        int end = start + chunk_size - 1 + (i < remainder ? 1 : 0);
        threads.emplace_back(partial_factorial, start, end, ref(partial_results[i]), i + 1);
        start = end + 1;
    }

    for (auto &t : threads) {
        t.join();
    }

    cpp_int final_result = 1;
    for (auto &res : partial_results) {
        final_result *= res;
    }

    return final_result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Использование: " << argv[0] << " <число_потоков> <число_факториала>" << endl;
        return 1;
    }

    int num_threads = stoi(argv[1]);
    int factorial_number = stoi(argv[2]);

    cout << "Запуск вычисления " << factorial_number << "! с " << num_threads << " потоками..." << endl;
    cpp_int result = parallel_factorial(factorial_number, num_threads);
    cout << "\nИтоговый результат: " << factorial_number << "! = " << result << endl;

    return 0;
}
