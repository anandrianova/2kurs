#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <limits> // Для std::numeric_limits

using namespace std;

// ==================== Конфигурация ====================

struct SimulationConfig {
    int fieldWidth = 15; // Ширина поля по умолчанию
    int fieldHeight = 10; // Высота поля по умолчанию
    int maxSteps = 25; // Максимальное количество шагов симуляции
    int initialPreyCount = 5; // Начальное количество жертв
    int initialPredatorCount = 2; // Начальное количество хищников
    int maxAgePrey = 10; // Максимальный возраст для жертвы (лет)
    int maxAgePredator = 15; // Максимальный возраст для хищника (лет)
    int predatorSatietyToReproduce = 2; // Сколько еды нужно хищнику для размножения
    int stepDelayMs = 400; // Задержка между фазами шага для наглядности
    int phaseDelayMs = 200; // Задержка между секциями вывода
    bool manualMode = false; // Режим ручной настройки
};

// ==================== Вспомогательные функции и классы ====================

/*Класс для генерации случайных чисел.*/
class RandomGenerator {
private:
    static mt19937 gen; // Генератор случайных чисел
    
public:
    /*Инициализация генератора с помощью текущего времени.*/
    static void init() {
        gen.seed(chrono::system_clock::now().time_since_epoch().count());
    }
    
    /*Получение случайного целого числа в диапазоне [min, max].*/
    static int getInt(int min, int max) {
        return uniform_int_distribution<>(min, max)(gen);
    }
    
    /*Получение случайной вероятности в диапазоне [0.0, 1.0].*/
    static double getProbability() {
        return uniform_real_distribution<>(0.0, 1.0)(gen);
    }
};

// Инициализация статического члена класса
mt19937 RandomGenerator::gen;

/* Вспомогательная функция для безопасного чтения целого числа от пользователя.*/
int readIntInput(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        // Проверка на корректность типа и чтение
        if (cin >> value) {
            // Проверка на диапазон
            if (value >= min && value <= max) {
                return value;
            } else {
                cout << "Ошибка: Значение должно быть в диапазоне от " << min << " до " << max << ".\n";
            }
        } else {
            // Очистка флагов ошибок (для некорректного типа)
            cin.clear();
            // Игнорирование оставшихся символов в буфере
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: Введите корректное числовое значение.\n";
        }
    }
}

// ==================== Базовые классы ====================

/*Перечисление для представления направления движения. */
enum class Direction { UP, RIGHT, DOWN, LEFT };

/*Преобразует направление в строку (для вывода). */
string directionToString(Direction d) {
    const string directions[] = {"^", "->", "/", "<-"};
    return directions[static_cast<int>(d)];
}

/*Преобразует целое число в направление (циклически).*/
Direction intToDirection(int i) {
    i = (i % 4 + 4) % 4;
    return static_cast<Direction>(i);
}

/*Абстрактный базовый класс для всех существ (Animal).Демонстрирует принципы Наследования и Полиморфизма. */
class Animal {
protected:
    int posX, posY; // Координаты
    Direction facing; // Направление
    int years; // Возраст
    int changeStep; // Как часто меняет направление (шаги)
    int stepsSinceTurn; // Шаги с момента последнего изменения направления
    bool alive; // Флаг жизни
    int maxLifespan; // Максимальная продолжительность жизни

public:
    // Конструктор
    Animal(int startX, int startY, Direction startDir, int changeStepVal, int maxLife)
        : posX(startX), posY(startY), facing(startDir), changeStep(changeStepVal),
          stepsSinceTurn(0), years(0), alive(true), maxLifespan(maxLife) {}
    
    // Виртуальный деструктор для корректной очистки памяти через базовый класс
    virtual ~Animal() = default;
    
    // ЧИСТО ВИРТУАЛЬНЫЕ МЕТОДЫ (Полиморфизм)
    virtual void performMove() = 0; // Реализуется в потомках (шаг 1 или 2)
    virtual char getDisplayChar() const = 0; // Символ для отображения
    virtual bool isPredator() const = 0; // Тип существа
    virtual bool canMultiply() const = 0; // Условие размножения
    virtual unique_ptr<Animal> createOffspring() = 0; // Создание потомка
    virtual void consume() = 0; // Действие после поглощения пищи
    virtual string getSpeciesName() const = 0; // Название вида
    
    // ВИРТУАЛЬНЫЕ МЕТОДЫ ДЛЯ КОНФЛИКТА (Полиморфизм/Стратегия)
    virtual bool vulnerableInCombat() const { return false; } // Вероятность быть раненым
    virtual bool attemptToAttack() { return false; } // Вероятность атаковать
    
    /*Увеличение возраста существа на 1 год. Проверка на старость.*/
    void ageOneYear() {
        years++;
        if (years >= maxLifespan) perish();
    }
    
    // Геттеры и сеттеры
    bool isLiving() const { return alive; }
    void perish() { alive = false; }
    int getX() const { return posX; }
    int getY() const { return posY; }
    int getYears() const { return years; }
    Direction getFacing() const { return facing; }
    void reposition(int newX, int newY) { posX = newX; posY = newY; }
    
    /*Проверяет, пора ли сменить направление движения, и меняет его.*/
    void considerDirectionChange() {
        stepsSinceTurn++;
        if (stepsSinceTurn >= changeStep) {
            // Смена направления по часовой стрелке
            facing = intToDirection(static_cast<int>(facing) + 1);
            stepsSinceTurn = 0;
        }
    }
    
    /* Возвращает строку статуса существа для отчета.*/
    string getStatus() const {
        // Используем строковый поток для более чистого форматирования
        stringstream ss;
        ss << getSpeciesName() << " в (" << posX << "," << posY << 
              ") " << directionToString(facing) << " возраст:" << years;
        return ss.str();
    }
};

// ==================== Жертва ====================
class Prey : public Animal {
private:
    // Используется для отслеживания, сколько раз уже размножалась жертва
    mutable int reproductionEvents; 
    
public:
    Prey(int x, int y, Direction dir, int freq, int maxAge)
        : Animal(x, y, dir, freq, maxAge), reproductionEvents(0) {}
    
    /*Движение жертвы: перемещение на 1 клетку.Реализация performMove() для жертвы.*/
    void performMove() override {
        switch (facing) {
            case Direction::UP: posY--; break;
            case Direction::RIGHT: posX++; break;
            case Direction::DOWN: posY++; break;
            case Direction::LEFT: posX--; break;
        }
    }
    
    char getDisplayChar() const override { return 'V'; }
    bool isPredator() const override { return false; }
    string getSpeciesName() const override { return "Жертва"; }
    
    /*Условие размножения жертвы.Размножается только в возрасте 4 и 8 лет (если ранее не размножалась).*/
    bool canMultiply() const override {
        if (reproductionEvents >= 2) return false;
        // Используются фиксированные возраста для воспроизведения
        return (years == 4 && reproductionEvents == 0) || 
               (years == 8 && reproductionEvents == 1);
    }
    
    /*Создание нового объекта Prey.*/
    unique_ptr<Animal> createOffspring() override {
        reproductionEvents++;
        // Потомку передаются текущие координаты, направление и частота поворота родителя
        return make_unique<Prey>(posX, posY, facing, changeStep, maxLifespan);
    }
    
    void consume() override {} // Жертва не потребляет
};

// ==================== Хищники ====================
//базовый класс хищники
class Predator : public Animal {
protected:
    int foodEaten; // Съедено еды за текущий цикл
    int foodNeededForReproduction; // Необходимая еда для размножения
    char symbol; // Символ вида
    string species; // Название вида
    
public:
    Predator(int x, int y, Direction dir, int freq, int maxLife, 
             int foodNeeded, char sym, string name)
        : Animal(x, y, dir, freq, maxLife), foodEaten(0), 
          foodNeededForReproduction(foodNeeded), symbol(sym), species(name) {}
    
    /*Движение хищника: перемещение на 2 клетки (быстрее жертвы).Реализация performMove() для хищника.*/
    void performMove() override {
        switch (facing) {
            case Direction::UP: posY -= 2; break;
            case Direction::RIGHT: posX += 2; break;
            case Direction::DOWN: posY += 2; break;
            case Direction::LEFT: posX -= 2; break;
        }
    }
    
    char getDisplayChar() const override { return symbol; }
    bool isPredator() const override { return true; }
    int getFoodCount() const { return foodEaten; }
    string getSpeciesName() const override { return species; }
    
    /*Условие размножения хищника.Размножается, если съел достаточно пищи.*/
    bool canMultiply() const override { return foodEaten >= foodNeededForReproduction; }
    
    //увеличение сытности
    void consume() override {
        foodEaten++;
    }
    
    //добавление сытности
    string getStatus() const {
        return Animal::getStatus() + " сытость:" + to_string(foodEaten);
    }
};

//класс волк
class Wolf : public Predator {
public:
    Wolf(int x, int y, Direction dir, int freq, int maxAge, int foodNeeded)
        : Predator(x, y, dir, freq, maxAge, foodNeeded, 'W', "Волк") {}
    
    unique_ptr<Animal> createOffspring() override {
        // Создание потомка, обнуление счетчика сытости для родителя
        foodEaten = 0; 
        return make_unique<Wolf>(posX, posY, facing, changeStep, maxLifespan, foodNeededForReproduction);
    }
    
    // Вероятность быть раненым/убитым в конфликте с другим хищником (45%)
    bool vulnerableInCombat() const override {
        return RandomGenerator::getProbability() < 0.45;
    }
    
    // Вероятность атаковать/победить в конфликте с другим хищником (55%)
    bool attemptToAttack() override {
        return RandomGenerator::getProbability() < 0.55;
    }
};

//класс тигр
class Tiger : public Predator {
public:
    Tiger(int x, int y, Direction dir, int freq, int maxAge, int foodNeeded)
        : Predator(x, y, dir, freq, maxAge, foodNeeded, 'T', "Тигр") {}
    
    unique_ptr<Animal> createOffspring() override {
        foodEaten = 0;
        return make_unique<Tiger>(posX, posY, facing, changeStep, maxLifespan, foodNeededForReproduction);
    }
    
    // Вероятность быть раненым/убитым в конфликте с другим хищником (25% - меньше, т.е. сильнее)
    bool vulnerableInCombat() const override {
        return RandomGenerator::getProbability() < 0.25;
    }
    
    // Вероятность атаковать/победить в конфликте с другим хищником (75% - больше)
    bool attemptToAttack() override {
        return RandomGenerator::getProbability() < 0.75;
    }
};

//класс лиса
class Fox : public Predator {
public:
    Fox(int x, int y, Direction dir, int freq, int maxAge, int foodNeeded)
        : Predator(x, y, dir, freq, maxAge, foodNeeded, 'F', "Лиса") {}
    
    unique_ptr<Animal> createOffspring() override {
        foodEaten = 0;
        return make_unique<Fox>(posX, posY, facing, changeStep, maxLifespan, foodNeededForReproduction);
    }
    
    // Вероятность быть раненым/убитым в конфликте с другим хищником (35%)
    bool vulnerableInCombat() const override {
        return RandomGenerator::getProbability() < 0.35;
    }
    
    // Вероятность атаковать/победить в конфликте с другим хищником (40% - меньше, т.е. слабее)
    bool attemptToAttack() override {
        return RandomGenerator::getProbability() < 0.4;
    }
};

// ==================== Фабрика существ ====================
class CreatureFactory {
public:
    //случайная жертва
    static unique_ptr<Animal> generatePrey(const SimulationConfig& config) {
        int x = RandomGenerator::getInt(0, config.fieldWidth - 1);
        int y = RandomGenerator::getInt(0, config.fieldHeight - 1);
        Direction dir = intToDirection(RandomGenerator::getInt(0, 3));
        int freq = RandomGenerator::getInt(2, 4);
        return make_unique<Prey>(x, y, dir, freq, config.maxAgePrey);
    }
    
    // Создает случайного хищника (Волк, Тигр или Лиса). 
    static unique_ptr<Animal> generatePredator(const SimulationConfig& config) {
        int x = RandomGenerator::getInt(0, config.fieldWidth - 1);
        int y = RandomGenerator::getInt(0, config.fieldHeight - 1);
        Direction dir = intToDirection(RandomGenerator::getInt(0, 3));
        int freq = RandomGenerator::getInt(2, 4);
        
        int type = RandomGenerator::getInt(0, 2);
        // Разные виды имеют разные характеристики (MaxAge, FoodNeeded, Combat abilities)
        switch (type) {
            case 0: return make_unique<Wolf>(x, y, dir, freq, config.maxAgePredator, config.predatorSatietyToReproduce);
            case 1: return make_unique<Tiger>(x, y, dir, freq, config.maxAgePredator + 3, config.predatorSatietyToReproduce);
            case 2: return make_unique<Fox>(x, y, dir, freq, config.maxAgePredator - 2, config.predatorSatietyToReproduce + 1);
            default: return make_unique<Wolf>(x, y, dir, freq, config.maxAgePredator, config.predatorSatietyToReproduce);
        }
    }
};

// ==================== Поле и симуляция ====================
//Класс GameWorld: среда и логика симуляции.Агрегирует существ, управляет шагами симуляции.
class GameWorld {
private:
    SimulationConfig settings; // Настройки
    // Вектор уникальных указателей для хранения существ. 
    // Обеспечивает полиморфизм и автоматическое управление памятью.
    vector<unique_ptr<Animal>> creatures; 
    int currentStep; // Текущий шаг симуляции
    
    //корректировка координат
    void adjustCoordinates(int& x, int& y) const {
        x = (x % settings.fieldWidth + settings.fieldWidth) % settings.fieldWidth;
        y = (y % settings.fieldHeight + settings.fieldHeight) % settings.fieldHeight;
    }
    
    //загаловок
    void showSection(const string& title) {
        cout << "\n┌────────────────────────────────────────┐\n";
        cout << "│ " << left << setw(36) << title << " │\n";
        cout << "└────────────────────────────────────────┘\n";
        this_thread::sleep_for(chrono::milliseconds(settings.phaseDelayMs));
    }
    
    //логирование
    void logEvent(const string& event) {
        cout << "  ◦ " << event << endl;
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    
public:
    GameWorld(const SimulationConfig& config) : settings(config), currentStep(0) {
        RandomGenerator::init();
        
        if (settings.manualMode) {
            setupManual();
        } else {
            setupRandom();
        }
    }
    
    //ручная настройка
    void setupManual() {
        cout << "\n=== РУЧНАЯ НАСТРОЙКА СИМУЛЯЦИИ ===\n";
        
        // Используем вспомогательную функцию для валидации ввода
        settings.fieldWidth = readIntInput("Ширина поля (5-30): ", 5, 30);
        settings.fieldHeight = readIntInput("Высота поля (5-20): ", 5, 20);
        settings.maxSteps = readIntInput("Количество шагов (5-50): ", 5, 50);
        settings.initialPreyCount = readIntInput("Начальное количество жертв (1-20): ", 1, 20);
        settings.initialPredatorCount = readIntInput("Начальное количество хищников (1-10): ", 1, 10);
        
        // Создание существ
        for (int i = 0; i < settings.initialPreyCount; ++i) {
            creatures.push_back(CreatureFactory::generatePrey(settings));
        }
        
        for (int i = 0; i < settings.initialPredatorCount; ++i) {
            creatures.push_back(CreatureFactory::generatePredator(settings));
        }
        
        cout << "Создано " << creatures.size() << " существ.\n";
    }
    
    //автоматическая настройка
    void setupRandom() {
        cout << "\n=== АВТОМАТИЧЕСКАЯ ГЕНЕРАЦИЯ (параметры по умолчанию) ===\n";
        
        for (int i = 0; i < settings.initialPreyCount; ++i) {
            auto prey = CreatureFactory::generatePrey(settings);
            logEvent("Добавлена жертва: " + prey->getStatus());
            creatures.push_back(move(prey));
        }
        
        for (int i = 0; i < settings.initialPredatorCount; ++i) {
            auto predator = CreatureFactory::generatePredator(settings);
            logEvent("Добавлен хищник: " + predator->getStatus());
            creatures.push_back(move(predator));
        }
        
        cout << "Сгенерировано " << creatures.size() << " существ случайным образом.\n";
    }
    
    //выполнение 1 шага 
    void executeStep() {
        currentStep++;
        
        // 1. ФАЗА ПЕРЕМЕЩЕНИЯ
        showSection("ШАГ " + to_string(currentStep) + ": ПЕРЕДВИЖЕНИЕ");
        for (auto& creature : creatures) {
            if (creature->isLiving()) {
                // Вызов полиморфного метода performMove() (разный для Prey/Predator)
                creature->performMove(); 
                int x = creature->getX();
                int y = creature->getY();
                // Применение эффекта замкнутого поля
                adjustCoordinates(x, y); 
                creature->reposition(x, y);
                // Проверка необходимости смены направления
                creature->considerDirectionChange(); 
                logEvent(creature->getSpeciesName() + " переместился в (" + to_string(x) + "," + to_string(y) + ")");
            }
        }
        displayWorld("После перемещения");
        this_thread::sleep_for(chrono::milliseconds(settings.stepDelayMs));
        
        // 2. ФАЗА ПИТАНИЯ (Хищники едят жертв в той же клетке)
        showSection("ШАГ " + to_string(currentStep) + ": ПИТАНИЕ");
        int consumedCount = 0;
        
        for (size_t i = 0; i < creatures.size(); ++i) {
            if (!creatures[i]->isLiving() || !creatures[i]->isPredator()) continue;
            
            for (size_t j = 0; j < creatures.size(); ++j) {
                // Если i - хищник, j - жертва, и они в одной клетке
                if (i != j && creatures[j]->isLiving() && !creatures[j]->isPredator() &&
                    creatures[i]->getX() == creatures[j]->getX() && 
                    creatures[i]->getY() == creatures[j]->getY()) {
                    
                    logEvent(creatures[i]->getSpeciesName() + " (" + to_string(i) + ") съел жертву (" + to_string(j) + ")");
                    creatures[j]->perish(); // Жертва погибает
                    creatures[i]->consume(); // Хищник становится сытнее
                    consumedCount++;
                    // Прерываем цикл по жертвам, т.к. хищник съел свою порцию
                    break; 
                }
            }
        }
        logEvent(consumedCount == 0 ? "Питания не было" : "Всего съедено: " + to_string(consumedCount));
        
        displayWorld("После питания");
        this_thread::sleep_for(chrono::milliseconds(settings.stepDelayMs));
        
        // 3. ФАЗА КОНФЛИКТОВ (Разные виды хищников в одной клетке)
        showSection("ШАГ " + to_string(currentStep) + ": КОНФЛИКТЫ");
        int conflictCount = 0;
        
        for (size_t i = 0; i < creatures.size(); ++i) {
            if (!creatures[i]->isLiving() || !creatures[i]->isPredator()) continue;
            
            for (size_t j = i + 1; j < creatures.size(); ++j) {
                if (!creatures[j]->isLiving() || !creatures[j]->isPredator()) continue;
                
                // Если два разных вида хищников находятся в одной клетке
                if (creatures[i]->getX() == creatures[j]->getX() && 
                    creatures[i]->getY() == creatures[j]->getY() &&
                    creatures[i]->getDisplayChar() != creatures[j]->getDisplayChar()) { 
                    
                    conflictCount++;
                    logEvent("Конфликт (" + creatures[i]->getSpeciesName() + " vs " + creatures[j]->getSpeciesName() + ")");
                    
                    // Проверка атаки и уязвимости (Полиморфизм в действии)
                    bool iAttacks = creatures[i]->attemptToAttack();
                    bool iVulnerable = creatures[i]->vulnerableInCombat();
                    bool jAttacks = creatures[j]->attemptToAttack();
                    bool jVulnerable = creatures[j]->vulnerableInCombat();
                    
                    // i атакует и j уязвим
                    if (iAttacks && jVulnerable) {
                        creatures[j]->perish();
                        logEvent(creatures[i]->getSpeciesName() + " победил и убил " + creatures[j]->getSpeciesName());
                    }
                    // j атакует и i уязвим
                    else if (jAttacks && iVulnerable) {
                        creatures[i]->perish();
                        logEvent(creatures[j]->getSpeciesName() + " победил и убил " + creatures[i]->getSpeciesName());
                    }
                }
            }
        }
        logEvent(conflictCount == 0 ? "Конфликтов не было" : "Конфликты урегулированы.");
        
        displayWorld("После конфликтов");
        this_thread::sleep_for(chrono::milliseconds(settings.stepDelayMs));
        
        // 4. ФАЗА СТАРЕНИЯ
        showSection("ШАГ " + to_string(currentStep) + ": СТАРЕНИЕ");
        int diedFromAge = 0;
        
        for (auto& creature : creatures) {
            if (creature->isLiving()) {
                creature->ageOneYear(); // Вызов базового метода
                if (!creature->isLiving()) {
                    diedFromAge++;
                    logEvent(creature->getSpeciesName() + " умерло от старости (возраст: " + to_string(creature->getYears()) + ")");
                }
            }
        }
        logEvent("Всего умерло от старости: " + to_string(diedFromAge));
        
        displayWorld("После старения");
        this_thread::sleep_for(chrono::milliseconds(settings.stepDelayMs));
        
        // 5. ФАЗА РАЗМНОЖЕНИЯ
        showSection("ШАГ " + to_string(currentStep) + ": РАЗМНОЖЕНИЕ");
        int newBorns = 0;
        vector<unique_ptr<Animal>> offspring;
        
        for (auto& creature : creatures) {
            if (creature->isLiving() && creature->canMultiply()) {
                // Вызов полиморфного метода createOffspring()
                offspring.push_back(creature->createOffspring());
                newBorns++;
                logEvent(creature->getSpeciesName() + " размножился");
            }
        }
        
        // Добавление новых существ в основной вектор
        for (auto& child : offspring) {
            creatures.push_back(move(child));
        }
        
        logEvent(newBorns == 0 ? "Размножения не было" : "Рождено новых существ: " + to_string(newBorns));
        
        displayWorld("После размножения");
        this_thread::sleep_for(chrono::milliseconds(settings.stepDelayMs));
        
        // 6. ФАЗА ОЧИСТКИ
        showSection("ШАГ " + to_string(currentStep) + ": ОЧИСТКА");
        int before = creatures.size();
        removeDead();
        int removed = before - creatures.size();
        
        logEvent("Удалено " + to_string(removed) + " мёртвых существ.");
        
        displayWorld("Итог шага " + to_string(currentStep));
        
        cout << "\n══════════════════════════════════════════\n";
        cout << "СТАТИСТИКА ШАГА " << currentStep << ":\n";
        cout << " • Съедено жертв: " << consumedCount << "\n";
        cout << " • Конфликтов: " << conflictCount << "\n";
        cout << " • Умерло от старости: " << diedFromAge << "\n";
        cout << " • Рождено новых: " << newBorns << "\n";
        cout << " • Всего существ: " << creatures.size() << "\n";
        cout << "══════════════════════════════════════════\n";
    }
    
    //удаление мертвых сцществ
    void removeDead() {
        
        creatures.erase(
            remove_if(creatures.begin(), creatures.end(),
                [](const unique_ptr<Animal>& a) { return !a->isLiving(); }),
            creatures.end()
        );
    }
    
    
    void displayWorld(const string& title = "Текущее состояние") const {
        // Очистка экрана для лучшей визуализации
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        cout << "\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║ " << left << setw(38) << title << "    ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        // Сетка для отображения существ
        vector<vector<int>> preyGrid(settings.fieldHeight, vector<int>(settings.fieldWidth, 0));
        vector<vector<int>> predatorGrid(settings.fieldHeight, vector<int>(settings.fieldWidth, 0));
        
        int preyTotal = 0, predatorTotal = 0;
        for (const auto& creature : creatures) {
            if (creature->isLiving()) {
                int x = creature->getX();
                int y = creature->getY();
                
                if (creature->isPredator()) {
                    predatorGrid[y][x]++;
                    predatorTotal++;
                } else {
                    preyGrid[y][x]++;
                    preyTotal++;
                }
            }
        }
        
        // Отрисовка поля
        cout << "  ";
        for (int i = 0; i < settings.fieldWidth; ++i) cout << i % 10;
        cout << "\n  +";
        for (int i = 0; i < settings.fieldWidth; ++i) cout << "-";
        cout << "+\n";
        
        for (int y = 0; y < settings.fieldHeight; ++y) {
            cout << setw(2) << y << " |";
            for (int x = 0; x < settings.fieldWidth; ++x) {
                if (predatorGrid[y][x] > 0 && preyGrid[y][x] > 0) {
                    cout << "X"; // Конфликт/Еда в одной клетке
                } else if (predatorGrid[y][x] > 0) {
                    if (predatorGrid[y][x] > 1) {
                        cout << predatorGrid[y][x]; // Несколько хищников
                    } else {
                        // Ищем, какой именно хищник (W, T, F) тут стоит
                        char sym = '.';
                        for (const auto& creature : creatures) {
                            if (creature->isLiving() && creature->isPredator() && creature->getX() == x && creature->getY() == y) {
                                sym = creature->getDisplayChar();
                                break;
                            }
                        }
                        cout << sym; 
                    }
                } else if (preyGrid[y][x] > 0) {
                    if (preyGrid[y][x] > 1) {
                        cout << preyGrid[y][x]; // Несколько жертв
                    } else {
                        cout << "V"; // Одна жертва
                    }
                } else {
                    cout << "."; // Пустая клетка
                }
            }
            cout << "| " << setw(2) << y << "\n";
        }
        
        cout << "  +";
        for (int i = 0; i < settings.fieldWidth; ++i) cout << "-";
        cout << "+\n ";
        for (int i = 0; i < settings.fieldWidth; ++i) cout << i % 10;
        
        cout << "\n\n┌─────── ЛЕГЕНДА ───────────────┐\n";
        cout << "│ V - жертва                        │\n";
        cout << "│ W - волк, T - тигр, F - лиса      │\n";
        cout << "│ X - хищник и жертва в одной клетке│\n";
        cout << "│ цифра - несколько существ         │\n";
        cout << "└───────────────────────────────────┘\n";
        
        cout << "\n СТАТИСТИКА (" << currentStep << "/" << settings.maxSteps << "):\n";
        cout << " Жертвы: " << preyTotal << "\n";
        cout << " Хищники: " << predatorTotal << "\n";
        cout << " Всего: " << (preyTotal + predatorTotal) << "\n";
        
        if (!creatures.empty()) {
            cout << "\n Первые 3 существа:\n";
            int shown = 0;
            for (const auto& creature : creatures) {
                if (creature->isLiving() && shown < 3) {
                    // Вызов полиморфного getStatus()
                    cout << " " << creature->getStatus() << "\n"; 
                    shown++;
                }
            }
            if (creatures.size() > 3) {
                cout << " ... и еще " << (creatures.size() - shown) << " существ\n";
            }
        }
    }
    
    //проверка на продолжение
    bool hasNextStep() const {
        return currentStep < settings.maxSteps && !creatures.empty();
    }
    
    //вывод итогов после симуляции
    void showFinalReport() const {
        int preyCount = 0, wolfCount = 0, tigerCount = 0, foxCount = 0;
        
        for (const auto& creature : creatures) {
            if (creature->isLiving()) {
                if (!creature->isPredator()) {
                    preyCount++;
                } else {
                    char sym = creature->getDisplayChar();
                    if (sym == 'W') wolfCount++;
                    else if (sym == 'T') tigerCount++;
                    else if (sym == 'F') foxCount++;
                }
            }
        }
        
        cout << "\n\n";
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        cout << "!               ФИНАЛЬНЫЙ ОТЧЕТ        !\n";
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
        
        cout << "Выполнено шагов: " << currentStep << "\n";
        cout << "Осталось существ: " << creatures.size() << "\n\n";
        
        cout << "Распределение:\n";
        cout << " Жертвы (V): " << preyCount << "\n";
        int predatorTotal = wolfCount + tigerCount + foxCount;
        cout << "  Хищники: " << predatorTotal << "\n";
        if (predatorTotal > 0) {
            cout << " • Волки (W): " << wolfCount << "\n";
            cout << " • Тигры (T): " << tigerCount << "\n";
            cout << " • Лисы (F): " << foxCount << "\n";
        }
        
        if (preyCount == 0 && predatorTotal > 0) {
            cout << "\n Исход: Все жертвы истреблены, хищники на грани вымирания.\n";
        } else if (predatorTotal == 0 && preyCount > 0) {
            cout << "\n Исход: Хищники вымерли, популяция жертв растет.\n";
        } else if (creatures.empty()) {
            cout << "\n Исход: Все существа вымерли (Великое вымирание!).\n";
        } else {
            cout << "\n Исход: Экосистема сохранила баланс или близка к нему.\n";
        }
    }
};

// ==================== Главное меню ====================

class SimulationController {
private:
    SimulationConfig config;
    
    void showWelcome() {
        cout << "\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║    СИМУЛЯТОР ЭКОСИСТЕМЫ                ║\n";
        cout << "║    Модель «Хищник — Жертва»            ║\n";
        cout << "╚════════════════════════════════════════╝\n";
    }
    
    //режим симуляции
    void chooseMode() {
        cout << "Выберите режим работы:\n";
        cout << " 1. Автоматическая симуляция (случайные параметры)\n";
        cout << " 2. Ручная настройка параметров (ввод с проверкой)\n";
        cout << " 3. Быстрый старт (значения по умолчанию)\n";
        
        int choice = readIntInput("\nВаш выбор (1-3): ", 1, 3);
        
        // Очистка буфера после ввода (если была использована readIntInput)
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                config.manualMode = false;
                // Автоматический режим будет использовать значения по умолчанию
                cout << "\nВыбран режим: АВТОМАТИЧЕСКАЯ СИМУЛЯЦИЯ\n";
                break;
            case 2:
                config.manualMode = true;
                cout << "\nВыбран режим: РУЧНАЯ НАСТРОЙКА\n";
                break;
            case 3:
            default:
                config.manualMode = false;
                cout << "\nВыбран режим: БЫСТРЫЙ СТАРТ (Default)\n";
                break;
        }
    }
    
public:
//основной цикл запуска
    void start() {
        showWelcome();
        chooseMode();
        
        GameWorld world(config);
        
        cout << "\nНажмите Enter для начала симуляции...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ждем Enter
        
        while (world.hasNextStep()) {
            world.executeStep();
            
            if (world.hasNextStep()) {
                cout << "\nНажмите Enter для следующего шага...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ждем Enter
            }
        }
        
        world.showFinalReport();
        
        cout << "\n──────────────────────────────────────────\n";
        cout << "Симуляция завершена. Нажмите Enter для выхода...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
};

// ==================== Точка входа ====================
int main() {

    setlocale(LC_ALL, "ru_RU.UTF-8"); 
    
    SimulationController controller;
    controller.start();
    return 0;
}