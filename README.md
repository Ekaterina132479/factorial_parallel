# Многопоточное вычисление факториала

Этот проект вычисляет факториал числа с использованием многопоточности в C++ и библиотеки Boost.Multiprecision.

## Требования
- Компилятор C++ (GCC, Clang или MSVC)
- Библиотека Boost

## Компиляция
```bash
g++ -o factorial main2.cpp -pthread -I /usr/include/boost
```

## Запуск
```bash
./factorial <число_потоков> <число_факториала>
```
Пример:
```bash
./factorial 4 20
```

## Оптимизация
Чтобы использовать оптимальное число потоков:
```cpp
int num_threads = std::thread::hardware_concurrency();
```

## Лицензия
MIT.

