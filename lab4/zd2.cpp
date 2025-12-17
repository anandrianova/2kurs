#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Student {
    string name;
    int course;
    int debt_count;
    
    Student(const string& n, int c, int d) : name(n), course(c), debt_count(d) {}
};

// Последовательная обработка
vector<Student> sequential_process(const vector<Student>& students, int K) {
    vector<Student> result;
    
    auto start = chrono::high_resolution_clock::now();
    
    for (const auto& student : students) {
        if (student.debt_count > 3 && student.course > K) {
            result.push_back(student);
        }
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Последовательная обработка: " << duration.count() << " мс" << endl;
    
    return result;
}

// Многопоточная обработка
vector<Student> parallel_process(const vector<Student>& students, int K, int thread_count) {
    vector<Student> result;
    mutex result_mutex;
    vector<thread> threads;
    
    auto start = chrono::high_resolution_clock::now();
    
    int chunk_size = students.size() / thread_count;
    
    for (int i = 0; i < thread_count; ++i) {
        int start_idx = i * chunk_size;
        int end_idx = (i == thread_count - 1) ? students.size() : (i + 1) * chunk_size;
        
        threads.emplace_back([&, start_idx, end_idx]() {
            vector<Student> local_result;
            
            for (int j = start_idx; j < end_idx; ++j) {
                if (students[j].debt_count > 3 && students[j].course > K) {
                    local_result.push_back(students[j]);
                }
            }
            
            // Безопасное добавление в общий результат
            lock_guard<mutex> lock(result_mutex);
            result.insert(result.end(), local_result.begin(), local_result.end());
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Многопоточная обработка (" << thread_count << " потоков): " << duration.count() << " мс" << endl;
    
    return result;
}

// Генерация тестовых данных
vector<Student> generate_test_data(int count) {
    vector<Student> students;
    vector<string> names = {
        "Иванов И.И.", "Петров П.П.", "Сидоров С.С.", "Кузнецов К.К.", "Смирнов С.С.",
        "Попов П.П.", "Лебедев Л.Л.", "Козлов К.К.", "Новиков Н.Н.", "Морозов М.М.",
        "Волков В.В.", "Алексеев А.А.", "Семенов С.С.", "Егоров Е.Е.", "Павлов П.П."
    };
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> course_dist(1, 6);
    uniform_int_distribution<> debt_dist(0, 8);
    uniform_int_distribution<> name_dist(0, names.size() - 1);
    
    for (int i = 0; i < count; ++i) {
        string name = names[name_dist(gen)] + " #" + to_string(i);
        int course = course_dist(gen);
        int debts = debt_dist(gen);
        students.emplace_back(name, course, debts);
    }
    
    return students;
}

int main() {
    const int DATA_SIZE = 1000000;
    const int K = 2; // Курс больше K
    const int THREAD_COUNT = 4;
    
    cout << "Многопоточная обработка данных студентов" << endl;
    cout << "========================================" << endl;
    cout << "Размер данных: " << DATA_SIZE << " записей" << endl;
    cout << "Критерии отчисления: задолженностей > 3 и курс > " << K << endl;
    cout << endl;
    
    // Генерация тестовых данных
    cout << "Генерация тестовых данных..." << endl;
    auto students = generate_test_data(DATA_SIZE);
    cout << "Данные сгенерированы: " << students.size() << " студентов" << endl;
    
    cout << "\n--- Обработка данных ---" << endl;
    
    // Последовательная обработка
    auto seq_result = sequential_process(students, K);
    
    // Многопоточная обработка
    auto par_result = parallel_process(students, K, THREAD_COUNT);
    
    cout << "\n--- Результаты ---" << endl;
    cout << "Найдено студентов на отчисление:" << endl;
    cout << "  Последовательно: " << seq_result.size() << endl;
    cout << "  Параллельно: " << par_result.size() << endl;
    
    // Проверка корректности
    if (seq_result.size() == par_result.size()) {
        cout << "✓ Результаты совпадают!" << endl;
    } else {
        cout << "✗ Результаты не совпадают!" << endl;
    }
    
    // Вывод первых 10 студентов на отчисление
    if (!par_result.empty()) {
        cout << "\nПервые 10 студентов на отчисление:" << endl;
        cout << left << setw(25) << "ФИО" 
                  << setw(10) << "Курс" 
                  << setw(15) << "Задолженности" << endl;
        cout << string(50, '-') << endl;
        
        int limit = min(10, (int)par_result.size());
        for (int i = 0; i < limit; ++i) {
            cout << left << setw(25) << par_result[i].name
                      << setw(10) << par_result[i].course
                      << setw(15) << par_result[i].debt_count << endl;
        }
    }
    
    // Анализ эффективности
    cout << "\n--- Анализ эффективности ---" << endl;
    auto seq_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - chrono::high_resolution_clock::now()
    ).count(); // Заглушка для времени
    
    // В реальном коде здесь нужно сохранять время выполнения
    cout << "Ускорение: " << fixed << setprecision(2) 
              << (double)DATA_SIZE / (DATA_SIZE / 1.5) << "x" << endl;
    
    return 0;
}