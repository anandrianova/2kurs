#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <string>
#include <mutex>
#include <vector>
#include <barrier>
#include <atomic>
#include <condition_variable>
#include <semaphore>
using namespace std;
using namespace std::chrono;

// Глобальные переменные для синхронизации вывода
mutex cout_mutex;
atomic<int> total_chars(0);

// Barrier
mutex barrier_mutex;
void myBarrier(int id, int summ, barrier<>& mybar) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    mybar.arrive_and_wait();  // Ждем все потоки
    
    {
        lock_guard<mutex> lock(barrier_mutex);
        total_chars += letters.size();
    }
    
    {
        lock_guard<mutex> lock(cout_mutex);
        if (id == 0) {
            cout << "Все потоки прошли барьер. ";
            cout << "Общее количество символов: " << total_chars << endl;
        }
    }
}

// Monitor
class Monitor {
private:
    mutex mtx;
    condition_variable cv;
    bool locked = false;
    
public:
    void enter() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !locked; });
        locked = true;
    }
    
    void leave() {
        {
            lock_guard<mutex> lock(mtx);
            locked = false;
        }
        cv.notify_one();
    }
};

void myMonitor(int id, int summ, Monitor& monitor) {
    monitor.enter();
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    total_chars += letters.size();
    
    monitor.leave();
}

// Mutex
mutex global_mutex;
void myMutexes(int id, int summ) {
    lock_guard<mutex> lock(global_mutex);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    total_chars += letters.size();
}

// Semaphore
counting_semaphore<1> my_semaphore(1);
void mySem(int id, int summ) {
    my_semaphore.acquire();
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    total_chars += letters.size();
    
    my_semaphore.release();
}

// SpinLock (базовый)
atomic_flag basic_spinlock = ATOMIC_FLAG_INIT;
void mySpinLock(int id, int summ) {
    while (basic_spinlock.test_and_set(memory_order_acquire)) {}
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    total_chars += letters.size();
    
    basic_spinlock.clear(memory_order_release);
}

// SpinWait с yield
class SpinWait {
private:
    atomic_flag flag;
    
public:
    SpinWait() : flag(ATOMIC_FLAG_INIT) {}
    
    void lock() {
        while (flag.test_and_set(memory_order_acquire)) {
            this_thread::yield();
        }
    }
    
    void unlock() {
        flag.clear(memory_order_release);
    }
};

SpinWait spinwait;
void mySpinWait(int id, int summ) {
    spinwait.lock();
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    
    string letters;
    for (int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    
    total_chars += letters.size();
    
    spinwait.unlock();
}

// Функция для запуска теста
template<typename Func, typename... Args>
void runTest(const string& test_name, Func func, Args&&... args) {
    const int num_threads = 100;
    const int chars_per_thread = 100;
    
    vector<thread> threads;
    total_chars = 0;
    
    cout << "\n=== " << test_name << " ===" << endl;
    auto start = high_resolution_clock::now();
    
    // Создаем и запускаем потоки
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(func, i, chars_per_thread, forward<Args>(args)...);
    }
    
    // Ждем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    cout << "Время выполнения: " << duration.count() << " мкс" << endl;
    cout << "Всего сгенерировано символов: " << total_chars << endl;
    cout << "Пропускная способность: " 
         << (total_chars * 1000000.0 / duration.count()) << " символов/сек" << endl;
}

int main() {
    cout << "Тестирование примитивов синхронизации" << endl;
    cout << "=====================================" << endl;
    cout << "Количество потоков: 100" << endl;
    cout << "Символов на поток: 100" << endl;
    cout << "Всего символов: 10000" << endl;
    
    // Тест 1: Barrier
    {
        barrier mybar(100);
        runTest("Barrier", myBarrier, ref(mybar));
    }
    
    // Тест 2: Monitor
    {
        Monitor monitor;
        runTest("Monitor", myMonitor, ref(monitor));
    }
    
    // Тест 3: Mutex
    runTest("Mutex", myMutexes);
    
    // Тест 4: Semaphore
    runTest("Semaphore", mySem);
    
    // Тест 5: SpinLock (базовый)
    runTest("SpinLock (basic)", mySpinLock);
    
    // Тест 6: SpinWait (с yield)
    runTest("SpinWait (with yield)", mySpinWait);
    
    cout << "\n=====================================" << endl;
    cout << "Тестирование завершено!" << endl;
    
    // Анализ результатов
    cout << "\nАнализ результатов:" << endl;
    cout << "1. Barrier - все потоки ждут друг друга перед выполнением" << endl;
    cout << "2. Monitor - использует condition_variable для синхронизации" << endl;
    cout << "3. Mutex - стандартный мьютекс (блокирующий)" << endl;
    cout << "4. Semaphore - семафор с счетчиком" << endl;
    cout << "5. SpinLock - активное ожидание без переключения контекста" << endl;
    cout << "6. SpinWait - активное ожидание с yield для других потоков" << endl;
    
    return 0;
}