#include <iostream>
#include <string>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <postgresql/libpq-fe.h>
#include <vector>
#include <algorithm>
#include <cwchar>
#include <locale>

using namespace std;

// Функция для подсчета видимой ширины строки с учетом кириллицы
size_t visibleWidth(const string& str) {
    size_t width = 0;
    for (char c : str) {
        // Проверяем, является ли символ частью кириллицы
        if ((c & 0xC0) == 0xC0) {
            // UTF-8 мультибайтовый символ (кириллица) - считаем как 1 символ
            // Пропускаем дополнительные байты UTF-8 последовательности
        } else if ((unsigned char)c >= 0x20 && (unsigned char)c <= 0x7E) {
            // Обычные ASCII символы
            width++;
        } else if (c == '\t') {
            width += 4; // Табуляция как 4 пробела
        }
    }
    return width;
}

class CinemaDatabase {
private:
    PGconn* connection;
    
    void printResult(PGresult* res) {
        if (!res) return;
        
        int rows = PQntuples(res);
        int cols = PQnfields(res);
        
        if (rows == 0 || cols == 0) return;
        
        // Вычисляем максимальную видимую ширину для каждого столбца
        vector<size_t> colWidths(cols, 0);
        
        // Находим максимальную длину для заголовков
        for (int j = 0; j < cols; j++) {
            string colName = PQfname(res, j);
            colWidths[j] = max(colWidths[j], visibleWidth(colName));
        }
        
        // Находим максимальную длину для данных
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                string value = PQgetvalue(res, i, j) ? PQgetvalue(res, i, j) : "NULL";
                colWidths[j] = max(colWidths[j], visibleWidth(value));
            }
        }
        
        // Добавляем отступы для читаемости
        for (int j = 0; j < cols; j++) {
            colWidths[j] += 2; // 2 пробела с каждой стороны
        }
        
        // Вычисляем общую ширину таблицы
        size_t totalWidth = 0;
        for (int j = 0; j < cols; j++) {
            totalWidth += colWidths[j];
        }
        totalWidth += cols - 1; // Разделители между колонками
        
        // Выводим заголовок таблицы
        cout << "\n" << string(min(totalWidth, (size_t)120), '-') << "\n";
        
        // Выводим названия столбцов
        for (int j = 0; j < cols; j++) {
            string colName = PQfname(res, j);
            size_t visibleLen = visibleWidth(colName);
            size_t padding = colWidths[j] - visibleLen - 1;
            
            cout << " " << left << setw(padding + visibleLen) << colName;
            if (j < cols - 1) cout << "|";
        }
        cout << "\n" << string(min(totalWidth, (size_t)120), '-') << "\n";
        
        // Выводим данные
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                string value = PQgetvalue(res, i, j) ? PQgetvalue(res, i, j) : "NULL";
                size_t visibleLen = visibleWidth(value);
                
                // Обрезаем слишком длинные значения
                if (visibleLen > colWidths[j] - 3) {
                    // Находим позицию для обрезки с учетом UTF-8
                    size_t pos = 0;
                    size_t charCount = 0;
                    while (pos < value.length() && charCount < colWidths[j] - 6) {
                        if ((value[pos] & 0xC0) != 0x80) charCount++;
                        pos++;
                    }
                    value = value.substr(0, pos) + "...";
                    visibleLen = visibleWidth(value);
                }
                
                size_t padding = colWidths[j] - visibleLen - 1;
                cout << " " << left << setw(padding + visibleLen) << value;
                if (j < cols - 1) cout << "|";
            }
            cout << "\n";
        }
        
        // Выводим итоговую строку
        if (rows > 0) {
            cout << string(min(totalWidth, (size_t)120), '-') << "\n";
        }
    }
    
    bool executeQuery(const string& query) {
        if (!connection || PQstatus(connection) != CONNECTION_OK) {
            cerr << "Нет подключения к базе данных" << endl;
            return false;
        }
        
        PGresult* res = PQexec(connection, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка запроса: " << PQerrorMessage(connection) << endl;
            PQclear(res);
            return false;
        }
        
        printResult(res);
        PQclear(res);
        return true;
    }

public:
    CinemaDatabase(const string& conninfo) {
        connection = PQconnectdb(conninfo.c_str());
        if (PQstatus(connection) != CONNECTION_OK) {
            cerr << "Не удалось подключиться к БД: " << PQerrorMessage(connection) << endl;
            PQfinish(connection);
            connection = nullptr;
        }
    }
    
    ~CinemaDatabase() {
        if (connection) {
            PQfinish(connection);
            cout << "\nСоединение закрыто." << endl;
        }
    }
    
    bool isConnected() const {
        return connection != nullptr && PQstatus(connection) == CONNECTION_OK;
    }
    
    // ============ 10 ЗАПРОСОВ ДЛЯ ЛАБОРАТОРНОЙ РАБОТЫ ============
    
    // 1. Все фильмы, отсортированные по рейтингу
    void getAllFilmsSortedByRating() {
        cout << "\n1. ВСЕ ФИЛЬМЫ (СОРТИРОВКА ПО РЕЙТИНГУ)" << endl;
        string query = 
            "SELECT title, director, genre, rating "
            "FROM films ORDER BY rating DESC";
        executeQuery(query);
    }
    
    // 2. Фильмы определенного жанра (Мультфильмы и Аниме)
    void getFilmsByGenre() {
        cout << "\n2. МУЛЬТФИЛЬМЫ И АНИМЕ" << endl;
        string query = 
            "SELECT title, director, duration_minutes, rating "
            "FROM films "
            "WHERE genre LIKE '%Мультфильм%' OR genre LIKE '%Аниме%' "
            "ORDER BY rating DESC";
        executeQuery(query);
    }
    
    // 3. Расписание сеансов с JOIN
    void getScheduleWithDetails() {
        cout << "\n3. РАСПИСАНИЕ СЕАНСОВ 16-18 ДЕКАБРЯ" << endl;
        string query = 
            "SELECT f.title, h.hall_name, s.session_time, s.price, s.available_seats "
            "FROM sessions s "
            "JOIN films f ON s.film_id = f.film_id "
            "JOIN halls h ON s.hall_id = h.hall_id "
            "WHERE DATE(s.session_time) BETWEEN '2025-12-16' AND '2025-12-18' "
            "ORDER BY s.session_time "
            "LIMIT 15";
        executeQuery(query);
    }
    
    // 4. Статистика по жанрам (COUNT + GROUP BY)
    void getGenreStatistics() {
        cout << "\n4. СТАТИСТИКА ПО ЖАНРАМ" << endl;
        string query = 
            "SELECT genre, COUNT(*) as количество_фильмов, "
            "ROUND(AVG(rating), 2) as средний_рейтинг "
            "FROM films GROUP BY genre ORDER BY количество_фильмов DESC";
        executeQuery(query);
    }
    
    // 5. Общая статистика залов (SUM + AVG)
    void getHallsStatistics() {
        cout << "\n5. ОБЩАЯ ВМЕСТИМОСТЬ КИНОЗАЛОВ" << endl;
        string query = 
            "SELECT SUM(capacity) as общая_вместимость, "
            "AVG(capacity) as средняя_вместимость, "
            "COUNT(*) as количество_залов FROM halls";
        executeQuery(query);
    }
    
    // 6. Клиенты и их покупки (LEFT JOIN)
    void getClientsWithTickets() {
        cout << "\n6. КЛИЕНТЫ И ИХ ПОКУПКИ (LEFT JOIN)" << endl;
        string query = 
            "SELECT c.first_name, c.last_name, c.email, "
            "COUNT(t.ticket_id) as куплено_билетов, "
            "CASE WHEN COUNT(t.ticket_id) > 0 THEN 'Да' ELSE 'Нет' END as был_в_кино "
            "FROM clients c "
            "LEFT JOIN tickets t ON c.client_id = t.client_id "
            "GROUP BY c.client_id "
            "ORDER BY куплено_билетов DESC";
        executeQuery(query);
    }
    
    // 7. Премиум залы
    void getPremiumHalls() {
        cout << "\n7. ПРЕМИУМ ЗАЛЫ" << endl;
        string query = 
            "SELECT hall_name, capacity, "
            "CASE WHEN has_3d THEN 'Есть' ELSE 'Нет' END as \"3D\", "
            "CASE WHEN has_dolby THEN 'Есть' ELSE 'Нет' END as \"Dolby Atmos\" "
            "FROM halls WHERE capacity > 100 "
            "AND (has_3d = true OR has_dolby = true) "
            "ORDER BY capacity DESC";
        executeQuery(query);
    }
    
    // 8. Фильмы с рейтингом выше среднего (подзапрос)
    void getFilmsAboveAverageRating() {
        cout << "\n8. ФИЛЬМЫ ВЫШЕ СРЕДНЕГО РЕЙТИНГА (ПОДЗАПРОС)" << endl;
        string query = 
            "SELECT title, director, rating, "
            "(SELECT ROUND(AVG(rating), 2) FROM films) as средний_по_базе "
            "FROM films WHERE rating > (SELECT AVG(rating) FROM films) "
            "ORDER BY rating DESC";
        executeQuery(query);
    }
    
    // 9. Общая статистика кинотеатра (UNION)
    void getCinemaStatistics() {
        cout << "\n9. СТАТИСТИКА КИНОТЕАТРА (UNION)" << endl;
        string query = 
            "SELECT 'Фильмы' as категория, COUNT(*) as количество FROM films "
            "UNION ALL "
            "SELECT 'Клиенты', COUNT(*) FROM clients "
            "UNION ALL "
            "SELECT 'Залы', COUNT(*) FROM halls "
            "UNION ALL "
            "SELECT 'Сеансы (16-18 дек)', COUNT(*) FROM sessions "
            "WHERE DATE(session_time) BETWEEN '2025-12-16' AND '2025-12-18' "
            "UNION ALL "
            "SELECT 'Проданные билеты', COUNT(*) FROM tickets "
            "ORDER BY категория";
        executeQuery(query);
    }
    
    // 10. Детальная информация о билетах (сложный JOIN)
    void getDetailedTicketInfo() {
        cout << "\n10. ДЕТАЛЬНАЯ ИНФОРМАЦИЯ О ПРОДАННЫХ БИЛЕТАХ" << endl;
        string query = 
            "SELECT t.ticket_id, f.title, "
            "c.first_name || ' ' || c.last_name as клиент, "
            "h.hall_name, t.seat_number, s.session_time, "
            "s.price "
            "FROM tickets t "
            "JOIN sessions s ON t.session_id = s.session_id "
            "JOIN films f ON s.film_id = f.film_id "
            "JOIN clients c ON t.client_id = c.client_id "
            "JOIN halls h ON s.hall_id = h.hall_id "
            "ORDER BY t.ticket_id "
            "LIMIT 15";
        executeQuery(query);
    }
    
    // Дополнительные запросы для демонстрации
    
    // 11. Поиск фильма по названию
    void findFilmByTitle(const string& title) {
        cout << "\nПОИСК ФИЛЬМА: " << title << endl;
        string query = 
            "SELECT * FROM films WHERE title LIKE '%" + title + "%'";
        executeQuery(query);
    }
    
    // 12. Сеансы на конкретный день
    void getSessionsForDate(const string& date) {
        cout << "\nСЕАНСЫ НА " << date << endl;
        string query = 
            "SELECT f.title, h.hall_name, s.session_time, s.price "
            "FROM sessions s "
            "JOIN films f ON s.film_id = f.film_id "
            "JOIN halls h ON s.hall_id = h.hall_id "
            "WHERE DATE(s.session_time) = '" + date + "' "
            "ORDER BY s.session_time";
        executeQuery(query);
    }
    
    // 16. Самый популярный фильм по количеству сеансов
    void getMostPopularFilm() {
        cout << "\nСАМЫЙ ПОПУЛЯРНЫЙ ФИЛЬМ ПО КОЛИЧЕСТВУ СЕАНСОВ" << endl;
        string query = 
            "SELECT f.title, COUNT(s.session_id) as количество_сеансов "
            "FROM films f "
            "LEFT JOIN sessions s ON f.film_id = s.film_id "
            "GROUP BY f.film_id "
            "ORDER BY количество_сеансов DESC LIMIT 3";
        executeQuery(query);
    }
    
    // 17. Выручка от каждого фильма
    void getRevenueByFilm() {
        cout << "\nВЫРУЧКА ОТ КАЖДОГО ФИЛЬМА" << endl;
        string query = 
            "SELECT f.title, COUNT(t.ticket_id) as проданных_билетов, "
            "SUM(s.price) as общая_выручка "
            "FROM films f "
            "JOIN sessions s ON f.film_id = s.film_id "
            "LEFT JOIN tickets t ON s.session_id = t.session_id "
            "GROUP BY f.film_id "
            "ORDER BY общая_выручка DESC";
        executeQuery(query);
    }
    
    // 18. Свободные места на ближайшие сеансы
    void getAvailableSeats() {
        cout << "\nСВОБОДНЫЕ МЕСТА НА БЛИЖАЙШИЕ СЕАНСЫ" << endl;
        string query = 
            "SELECT f.title, s.session_time, h.hall_name, s.available_seats "
            "FROM sessions s "
            "JOIN films f ON s.film_id = f.film_id "
            "JOIN halls h ON s.hall_id = h.hall_id "
            "WHERE s.session_time > NOW() AND s.available_seats > 0 "
            "ORDER BY s.session_time "
            "LIMIT 10";
        executeQuery(query);
    }
    
    // 19. Клиенты с самой высокой активностью
    void getMostActiveClients() {
        cout << "\nСАМЫЕ АКТИВНЫЕ КЛИЕНТЫ" << endl;
        string query = 
            "SELECT c.first_name, c.last_name, c.email, "
            "COUNT(t.ticket_id) as куплено_билетов, "
            "SUM(s.price) as всего_потрачено "
            "FROM clients c "
            "JOIN tickets t ON c.client_id = t.client_id "
            "JOIN sessions s ON t.session_id = s.session_id "
            "GROUP BY c.client_id "
            "ORDER BY куплено_билетов DESC "
            "LIMIT 5";
        executeQuery(query);
    }
    
    // 20. Статистика по дням недели
    void getStatisticsByDay() {
        cout << "\nСТАТИСТИКА ПО ДНЯМ НЕДЕЛИ" << endl;
        string query = 
            "SELECT "
            "TO_CHAR(session_time, 'Day') as день_недели, "
            "COUNT(*) as количество_сеансов, "
            "SUM(available_seats) as всего_мест, "
            "AVG(price) as средняя_цена "
            "FROM sessions "
            "GROUP BY день_недели, EXTRACT(dow FROM session_time) "
            "ORDER BY EXTRACT(dow FROM session_time)";
        executeQuery(query);
    }
};

// Функция для безопасного ввода пароля
string getPasswordSecure() {
    char* password = getpass("Введите пароль PostgreSQL: ");
    if (password == nullptr) {
        return "";
    }
    return string(password);
}

int main() {
    // Устанавливаем локаль для правильной работы с UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    // Подключение к вашей базе данных
    string conninfo = "host=172.31.160.1 dbname=cinema_db user=postgres port=5432";
    
    // Запрос пароля с безопасным вводом
    string password = getPasswordSecure();
    
    if (password.empty()) {
        cerr << "Ошибка: пароль не введен." << endl;
        return 1;
    }
    
    // Добавляем пароль к строке подключения
    conninfo += " password=" + password;
    
    // Очищаем пароль из памяти для безопасности
    password.clear();
    
    CinemaDatabase db(conninfo);
    
    if (db.isConnected()) {
        cout << "\nУспешное подключение к базе данных!" << endl;
        
        // Выполняем 10 основных запросов для лабораторной работы
        cout << "\n--- ВЫПОЛНЕНИЕ 10 ОСНОВНЫХ ЗАПРОСОВ ---" << endl;
        db.getAllFilmsSortedByRating();
        db.getFilmsByGenre();
        db.getScheduleWithDetails();
        db.getGenreStatistics();
        db.getHallsStatistics();
        db.getClientsWithTickets();
        db.getPremiumHalls();
        db.getFilmsAboveAverageRating();
        db.getCinemaStatistics();
        db.getDetailedTicketInfo();
        
        cout << "\n--- ДОПОЛНИТЕЛЬНЫЕ ЗАПРОСЫ ---" << endl;
        db.findFilmByTitle("бензопила");
        db.getSessionsForDate("2025-12-16");
        db.getMostPopularFilm();
        db.getRevenueByFilm();
        db.getAvailableSeats();
        db.getMostActiveClients();
        db.getStatisticsByDay();
    } else {
        cerr << "Ошибка подключения к базе данных. Проверьте параметры подключения." << endl;
        return 1;
    }
    
    return 0;
}