cmake_minimum_required(VERSION 3.10)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Список общих исходных файлов
set(COMMON_SOURCES
    Array.cpp
    SingleList.cpp
    DoubleList.cpp
    Stack.cpp
    Queue.cpp
    FullBinaryTree.cpp
    HashTable.cpp
    DB.cpp
)

# Основной исполняемый файл
add_executable(main
    main.cpp
    ${COMMON_SOURCES}
)

# Настройки компиляции
target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -O2
)

# ========== GOOGLE TEST ==========
find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Google Test найден")
    
    add_executable(google_tests
        google_test.cpp
        ${COMMON_SOURCES}
    )
    target_link_libraries(google_tests GTest::gtest GTest::gtest_main pthread)
    target_compile_options(google_tests PRIVATE -O2)
    
    add_test(NAME GoogleTests COMMAND google_tests)
else()
    message(WARNING "Google Test не найден. Установите: sudo apt-get install libgtest-dev")
endif()

# ========== CATCH2 ==========
# Ищем Catch2 как single-header
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/catch2/catch.hpp")
    message(STATUS "Catch2 найден как single-header")
    
    add_executable(catch2_tests
        catch2_tests.cpp
        ${COMMON_SOURCES}
    )
    target_include_directories(catch2_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_options(catch2_tests PRIVATE -O2)
    
    add_test(NAME Catch2Tests COMMAND catch2_tests)
else()
    # Пытаемся найти через систему
    find_package(Catch2 QUIET)
    if(Catch2_FOUND)
        message(STATUS "Catch2 найден в системе")
        
        add_executable(catch2_tests
            catch2_tests.cpp
            ${COMMON_SOURCES}
        )
        target_link_libraries(catch2_tests Catch2::Catch2WithMain)
        target_compile_options(catch2_tests PRIVATE -O2)
        
        add_test(NAME Catch2Tests COMMAND catch2_tests)
    else()
        message(WARNING "Catch2 не найден. Используйте single-header версию или установите")
    endif()
endif()

# ========== BOOST TEST ==========
find_package(Boost COMPONENTS unit_test_framework filesystem QUIET)  # Добавьте filesystem
if(Boost_FOUND)
    message(STATUS "Boost.Test найден")
    
    add_executable(boost_tests
        boost_test.cpp
        ${COMMON_SOURCES}
    )
    target_link_libraries(boost_tests 
        Boost::unit_test_framework
        Boost::filesystem  # Добавьте эту строку
    )
    target_compile_options(boost_tests PRIVATE -O2)
    
    add_test(NAME BoostTests COMMAND boost_tests)
else()
    message(WARNING "Boost.Test не найден. Установите: sudo apt-get install libboost-test-dev")
endif()

# ========== БЕНЧМАРК ==========
find_package(benchmark QUIET)
if(benchmark_FOUND)
    message(STATUS "Google Benchmark найден")
    
    add_executable(benchmark_exec
        benchmark.cpp
        ${COMMON_SOURCES}
    )
    target_link_libraries(benchmark_exec benchmark::benchmark)
    target_compile_options(benchmark_exec PRIVATE -O3)
else()
    message(WARNING "Google Benchmark не найден. Установите: sudo apt-get install libbenchmark-dev")
endif()

# ========== ЛИНТЕРЫ ==========
# Настройки линтера (clang-tidy)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
        message(STATUS "clang-tidy включен: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy не найден. Установите: sudo apt-get install clang-tidy")
    endif()
endif()

# Настройки cppcheck
option(ENABLE_CPPCHECK "Enable cppcheck" ON)
if(ENABLE_CPPCHECK)
    find_program(CPPCHECK_EXE NAMES cppcheck)
    if(CPPCHECK_EXE)
        add_custom_target(cppcheck_all
            COMMAND ${CPPCHECK_EXE}
            --enable=all
            --suppress=missingInclude
            --suppress=unmatchedSuppression
            --inconclusive
            --std=c++17
            ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Запуск cppcheck"
        )
        message(STATUS "cppcheck найден: ${CPPCHECK_EXE}")
    else()
        message(WARNING "cppcheck не найден. Установите: sudo apt-get install cppcheck")
    endif()
endif()

# ========== ПОКРЫТИЕ КОДА ==========
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    message(STATUS "Покрытие кода включено")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# ========== ЦЕЛИ ПО УМОЛЧАНИЮ ==========
# Установка зависимостей для WSL
add_custom_target(install_deps
    COMMAND echo "=== Установка зависимостей для WSL ==="
    COMMAND sudo apt-get update
    COMMAND sudo apt-get install -y g++ cmake clang-tidy cppcheck libgtest-dev libbenchmark-dev libboost-test-dev lcov
    COMMAND echo "=== Сборка Google Test ==="
    COMMAND cd /usr/src/gtest && sudo cmake . && sudo make && sudo cp *.a /usr/lib
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Обновление и установка зависимостей"
)

# Запуск всех тестов
add_custom_target(run_all_tests
    COMMAND echo "=== Запуск всех тестов ==="
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Выполнение всех тестов"
)

# Запуск бенчмарков
add_custom_target(run_benchmarks
    COMMAND echo "=== Запуск бенчмарков ==="
    COMMAND ./benchmark_exec
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск бенчмарков"
)

# Полная проверка
add_custom_target(full_check
    COMMAND echo "=== Полная проверка проекта ==="
    COMMAND echo "1. Сборка проекта..."
    COMMAND make -j$(nproc)
    COMMAND echo "2. Запуск тестов..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND echo "3. Запуск бенчмарков..."
    COMMAND ./benchmark_exec
    COMMAND echo "4. Проверка линтером..."
    COMMAND ${CPPCHECK_EXE} --enable=all --suppress=missingInclude --suppress=unmatchedSuppression --inconclusive --std=c++17 ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Полная проверка проекта"
)

# Генерация отчета о покрытии
add_custom_target(coverage_report
    COMMAND echo "=== Генерация отчета о покрытии ==="
    COMMAND lcov --directory . --capture --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_report
    COMMAND echo "Отчет создан: coverage_report/index.html"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Генерация отчета о покрытии кода"
)

# Показать помощь (переименовано с help на show_help)
add_custom_target(show_help
    COMMAND echo "=== Доступные цели ==="
    COMMAND echo "make install_deps      - Установить зависимости"
    COMMAND echo "make                   - Собрать проект"
    COMMAND echo "make run_all_tests     - Запустить все тесты"
    COMMAND echo "make run_benchmarks    - Запустить бенчмарки"
    COMMAND echo "make cppcheck_all      - Запустить cppcheck"
    COMMAND echo "make full_check        - Полная проверка"
    COMMAND echo "make coverage_report   - Генерация отчета о покрытии"
    COMMAND echo "make show_help         - Показать эту справку"
    COMMENT "Показать справку"
)

# Установите show_help как цель по умолчанию
add_custom_target(default ALL DEPENDS show_help)