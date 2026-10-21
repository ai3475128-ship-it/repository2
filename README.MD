# matrix-tests

Тесты библиотеки линейной алгебры `linalg` на GoogleTest.
Библиотека подключена как git-подмодуль (папка `linalg`).

## Клонирование

    git clone --recurse-submodules https://git.miem.hse.ru/ivanov_lab_1/matrix-tests.git

Если репозиторий уже склонирован без подмодулей:

    git submodule update --init

## Сборка и запуск

    cmake -B build
    cmake --build build
    ./build/matrix_tests

GoogleTest скачивается автоматически при конфигурации (нужен интернет).

## Что покрыто тестами

- конструкторы, присваивания, доступ к элементам, итераторы
- reserve, reshape, clear, shrink_to_fit, swap
- вывод в поток, арифметические операторы, сравнение с точностью
- norm, trace, det, rank, метод Гаусса
- concatenate, transpose, invert, power, solve
- свойства: A^0 = E, (A^(T))^(-1) = (A^(-1))^(T), (A1A2)^(-1) = A2^(-1)A1^(-1)