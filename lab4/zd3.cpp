#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <string>
#include <iomanip>

using namespace std;

const int NUM_PHILOSOPHERS = 5;

class DiningPhilosophers {
private:
    vector<mutex> forks;
    vector<thread> philosophers;
    vector<int> eat_count;
    mutex cout_mutex;
    bool running;
    
public:
    DiningPhilosophers() : forks(NUM_PHILOSOPHERS), eat_count(NUM_PHILOSOPHERS, 0), running(true) {}
    
    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> think_dist(100, 500);
        uniform_int_distribution<> eat_dist(100, 300);
        
        while (running) {
            // Философ размышляет
            think(id, think_dist(gen));
            
            // Пытается взять вилки
            if (take_forks(id)) {
                // Философ ест
                eat(id, eat_dist(gen));
                
                // Кладет вилки
                put_forks(id);
            }
        }
        
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Философ " << id << " закончил. Поел " << eat_count[id] << " раз." << endl;
        }
    }
    
    void think(int id, int duration) {
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Философ " << id << " размышляет (" << duration << "мс)" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(duration));
    }
    
    bool take_forks(int id) {
        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;
        
        // Чтобы избежать deadlock, философ с четным ID берет сначала левую вилку,
        // а с нечетным - сначала правую
        if (id % 2 == 0) {
            forks[left].lock();
            {
                lock_guard<mutex> lock(cout_mutex);
                cout << "Философ " << id << " взял левую вилку (" << left << ")" << endl;
            }
            
            // Пробуем взять правую вилку с таймаутом
            if (forks[right].try_lock()) {
                lock_guard<mutex> lock(cout_mutex);
                cout << "Философ " << id << " взял правую вилку (" << right << ")" << endl;
                return true;
            } else {
                // Если не удалось взять правую вилку, отпускаем левую
                forks[left].unlock();
                {
                    lock_guard<mutex> lock(cout_mutex);
                    cout << "Философ " << id << " не смог взять правую вилку, отпустил левую" << endl;
                }
                return false;
            }
        } else {
            forks[right].lock();
            {
                lock_guard<mutex> lock(cout_mutex);
                cout << "Философ " << id << " взял правую вилку (" << right << ")" << endl;
            }
            
            if (forks[left].try_lock()) {
                lock_guard<mutex> lock(cout_mutex);
                cout << "Философ " << id << " взял левую вилку (" << left << ")" << endl;
                return true;
            } else {
                forks[right].unlock();
                {
                    lock_guard<mutex> lock(cout_mutex);
                    cout << "Философ " << id << " не смог взять левую вилку, отпустил правую" << endl;
                }
                return false;
            }
        }
    }
    
    void eat(int id, int duration) {
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Философ " << id << " ЕСТ (" << duration << "мс)" << endl;
        }
        eat_count[id]++;
        this_thread::sleep_for(chrono::milliseconds(duration));
    }
    
    void put_forks(int id) {
        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;
        
        if (id % 2 == 0) {
            forks[right].unlock();
            forks[left].unlock();
        } else {
            forks[left].unlock();
            forks[right].unlock();
        }
        
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Философ " << id << " положил вилки" << endl;
        }
    }
    
    void start() {
        cout << "Начинается ужин философов!" << endl;
        cout << "==========================" << endl;
        
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i);
        }
        
        // Даем философам поужинать 10 секунд
        this_thread::sleep_for(chrono::seconds(10));
        
        running = false;
        
        for (auto& philosopher : philosophers) {
            philosopher.join();
        }
        
        cout << "\n==========================" << endl;
        cout << "Ужин окончен! Результаты:" << endl;
        cout << "==========================" << endl;
        
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            cout << "Философ " << i << ": поел " << eat_count[i] << " раз" << endl;
        }
        
        int total_meals = 0;
        for (int count : eat_count) {
            total_meals += count;
        }
        cout << "\nВсего приемов пищи: " << total_meals << endl;
        cout << "Среднее на философа: " << (double)total_meals / NUM_PHILOSOPHERS << endl;
    }
};

int main() {
    DiningPhilosophers dinner;
    dinner.start();
    
    return 0;
}