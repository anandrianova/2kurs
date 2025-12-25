#include "DB.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    (void)argc; 
    (void)argv;
    
    Database db;
    string command;
    
    cout << "=== Система управления базами данных контейнеров ===" << endl;
    cout << "Введите HELP для списка команд" << endl;
    cout << "Введите EXIT для выхода" << endl << endl;
    
    while (true) {
        cout << "> ";
        getline(cin, command);
        
        if (command == "EXIT" || command == "exit") {
            break;
        }
        
        string result = db.executeCommand(command);
        cout << result << endl;
    }
    
    return 0;
}