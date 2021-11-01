# DPLL
Все исходники в include/ src/ директориях.

Установка
```
cmake -S . -B build --target yadpll # для основного исполняемого файла
cmake -S . -B build --target unit # для юнит тестов (нужен gtest)
cmake -S . -B build --target stress # для stress тестов (нужен gtest)
cmake --build build
```

Использование
```
./build/yadpll tests/sat/$FILE # Считать из файла и решить
./build/unit # Запустить юнит тесты
./build/stress # Запустить тесты, читающие кнф из файлов tests/sat/* tests/unsat/*
```
