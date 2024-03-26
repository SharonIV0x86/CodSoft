#include "./todo.hpp"

int main()
{
    Todo todoObj;

    static databaseActions dbj;
    std::string todoTitle;
    std::string priority;
    int Priority;
    int dummyID;
    int ch = 1;
    dbj.checkDB();
    while (ch >= 1 && ch <= 5)
    {
        clearScreen();
        printColoredText("\n\n\t\t\tTOP TODOS \n", ANSI_COLOR_CYAN);
        todoObj.highPriorityTodos(dbj);
        printColoredText("\n\t\t\t\t\tTASK 1 TODOS", ANSI_COLOR_GREEN);
        printColoredText("\n\n\t 1. Add a todo ", ANSI_COLOR_BLUE);
        printColoredText("\n\n\t 2. Remove a todo ", ANSI_COLOR_BLUE);
        printColoredText("\n\n\t 3. View/Edit active todos ", ANSI_COLOR_BLUE);
        printColoredText("\n\n\t 4. View completed todos ", ANSI_COLOR_BLUE);
        printColoredText("\n\n\t 5. Exit ", ANSI_COLOR_BLUE);
        printColoredText("\n\n\tEnter a choice: ", ANSI_COLOR_BLUE);
        std::cin >> ch;

        switch (ch)
        {
        case 1:
            std::cin.ignore();
            clearScreen();
            printColoredText("\n\n\tEnter the name of the todo: ", ANSI_COLOR_BLUE);
            std::getline(std::cin, todoTitle);
            checkForEmptyField(todoTitle);
            printColoredText("\n\n\tEnter priority between 1 - 10, (optional) leave blank if wanted: ", ANSI_COLOR_BLUE);
            std::getline(std::cin, priority);
            if (priority.empty())
            {
                Priority = 10;
            }
            else if (std::stoi(priority) >= 1 && std::stoi(priority) <= 10)
            {
                Priority = std::stoi(priority);
            }
            else
            {
                clearScreen();
                printColoredText("\n\n\tEnter a valid value of priority! ", ANSI_COLOR_RED);
                exit(EXIT_FAILURE);
            }
            todoObj.addTodo(todoTitle, Priority, dbj, false);
            break;
        case 2:
            printColoredText("\n\n\tEnter ID of the Todo to remove: ", ANSI_COLOR_BLUE);
            std::cin >> dummyID;
            todoObj.removeTodo(dummyID, dbj);
            break;
        case 3:
            todoObj.showToDos(dbj);
            break;
        case 4:
            todoObj.viewCompletedTodos(dbj);
            break;
        case 5: 
            goto exit_loop;
            break;
        default:
        {
            goto exit_loop;
        }
        break;
        }
    }
exit_loop:
    return 0;
}
