#include "DB.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    Database* db = create_database();
    string filename = "database.txt";
    
    cout << "=== Data Structures Database Interface ===" << endl;
    cout << "Type 'HELP' for available commands" << endl;
    cout << "Type 'EXIT' to quit" << endl << endl;
    
    // Автозагрузка базы данных при запуске
    if (load_database(db, filename)) {
        cout << "Database loaded from " << filename << endl;
    }
    
    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);
        
        if (command == "EXIT" || command == "exit") {
            break;
        }
        
        if (!command.empty()) {
            string result = execute_command(db, command);
            cout << result << endl;
            
            // Автосохранение после каждой команды
            save_database(db, filename);
        }
    }
    
    // Финальное сохранение
    save_database(db, filename);
    free_database(db);
    
    cout << "Database saved to " << filename << endl;
    cout << "Goodbye!" << endl;
    
    return 0;
}