#!/bin/bash

# Создаем backup
cp DB.cpp DB.cpp.backup2

# Используем sed для исправления формата
sed -i '
/else if (cmd == "LIST" || cmd == "list")/,/^[[:space:]]*}/ {
    # Добавляем переменную hasContainers
    /string result = "CONTAINERS:\\n";/a\
        bool hasContainers = false;
    
    # Исправляем формат Arrays
    s/result += "Arrays: "/result += "Arrays:\\n"/g
    s/result += pair.first + " "/result += "  " + pair.first + "\\n"/g
    /if (!arrays.empty())/,/result += "\\n";/ {
        s/result += "\\n";/hasContainers = true;/
    }
    
    # Исправляем формат Singly Linked Lists
    s/result += "Singly Linked Lists: "/result += "Singly Linked Lists:\\n"/g
    s/singly_lists {/singly_lists) {/
    
    # Исправляем формат Doubly Linked Lists  
    s/result += "Doubly Linked Lists: "/result += "Doubly Linked Lists:\\n"/g
    
    # Исправляем формат Stacks
    s/result += "Stacks: "/result += "Stacks:\\n"/g
    
    # Исправляем формат Queues
    s/result += "Queues: "/result += "Queues:\\n"/g
    
    # Исправляем формат Trees
    s/result += "Trees: "/result += "Binary Trees:\\n"/g
    
    # Исправляем формат Double Hash Tables
    s/result += "Double Hash Tables: "/result += "Double Hash Tables:\\n"/g
    
    # Исправляем условие пустого результата
    s/if (result == "CONTAINERS:\\n") {/if (!hasContainers) {/
}
' DB.cpp

echo "Исправление применено"
