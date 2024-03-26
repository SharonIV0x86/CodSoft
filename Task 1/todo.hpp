#include "./src/sqlite3.h"
#include "./utils.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <tuple>
#include <format>
#include <random>
#include <stdexcept>

static bool isDBOpen = false;

class databaseActions
{
public:
    sqlite3 *databaseMain;
    sqlite3_stmt *preparedStatement;
    char *errormessage;
    databaseActions(){}
    databaseActions *openDB()
    {
        /*int result = */ sqlite3_open("todo.db", &databaseMain);

        isDBOpen = true;
        return this;
    }
    databaseActions(databaseActions const &) = delete;
    void operator=(databaseActions const &) = delete;
    bool checkTable()
    {
        std::string pragmaQuery = "PRAGMA table_info(";
        pragmaQuery += "todos);";
        std::cout << "Pragma query is: " << pragmaQuery << std::endl;
        int prep = sqlite3_prepare_v2(databaseMain, pragmaQuery.c_str(), -1, &preparedStatement, nullptr);
        if (prep != SQLITE_OK)
        {
            std::cout << "Failed To Execute Statement! " << std::endl;
            return false;
        }
        int stepResult = sqlite3_step(preparedStatement);
        if (stepResult == SQLITE_ROW)
        {
            return true;
        }
        else if (stepResult == SQLITE_DONE)
        {
            std::string createTableQuery = "CREATE TABLE todos("
                                           "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                           "TODOTITLE TEXT NOT NULL,"
                                           "PRIORITY INT NOT NULL,"
                                           "STATUS BOOLEAN NOT NULL)";
            int execCreateTable = sqlite3_exec(databaseMain, createTableQuery.c_str(), 0, 0, &errormessage);

            if (execCreateTable != SQLITE_OK)
            {
                printColoredText("\n\n\tSQL ERROR--> ", ANSI_COLOR_RED);
                printColoredText(static_cast<std::string>(errormessage), ANSI_COLOR_RED);
                sqlite3_free(errormessage);
            }
            else
            {
                printColoredText("\n\n\tTable created successfully! ", ANSI_COLOR_GREEN);
            }
        }
        else
        {
            std::cout << "Error executing statement! " << sqlite3_errmsg(databaseMain) << std::endl;
            sqlite3_finalize(preparedStatement);
            return false;
        }
        sqlite3_finalize(preparedStatement);
        return true;
    }
    bool checkDB()
    {
        if (!std::filesystem::exists("todo.db"))
        {
            std::cout << "Database doesnt exists!" << std::endl;
            std::cout << "Creating the database now" << std::endl;
            int dbHealth = sqlite3_open("todo.db", &databaseMain);
            if (dbHealth != SQLITE_OK)
            {
                std::cout << ": " << sqlite3_errmsg(databaseMain) << std::endl;
                printColoredText("Failed to open database", ANSI_COLOR_RED);
                printColoredText(sqlite3_errmsg(databaseMain), ANSI_COLOR_RED);
                exit(0);
            }
            checkTable();
            return false;
        }
        else
        {
            return true;
        }
    }
    ~databaseActions()
    {
        std::cout << "\n\n\tClosed Database successfully." << std::endl;
        sqlite3_close(databaseMain);
        exit(0);
    }
};
class Todo
{
public:
    std::vector<std::tuple<int, std::string, int, bool>> todo;
    std::vector<std::tuple<int, std::string, int, bool>> retrievedTodos;
    void addTodo(std::string, int, databaseActions &, bool);
    void removeTodo(int, databaseActions &);
    void showToDos(databaseActions &);
    void highPriorityTodos(databaseActions &);
    void viewCompletedTodos(databaseActions &);
};
void Todo::removeTodo(int id, databaseActions &dbj)
{
    if (!isDBOpen)
    {
        dbj.openDB();
    }
    std::string query1 = "SELECT 1 FROM todos WHERE ID = ?";
    int rowCheckPrep = sqlite3_prepare_v2(dbj.databaseMain, query1.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (rowCheckPrep != SQLITE_OK)
    {
        printColoredText("\n\n\tFailed to prepare SELECT statement! ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
        return;
    }
    sqlite3_bind_int(dbj.preparedStatement, 1, id);
    int rowCheckStep = sqlite3_step(dbj.preparedStatement);
    if (rowCheckStep != SQLITE_ROW)
    {
        clearScreen();
        std::cout << "\n\n\t";
        printColoredText("NO SUCH TODO EXIST! ", ANSI_COLOR_YELLOW);
        return;
    }
    std::string query = "DELETE FROM todos WHERE ID = ?";

    int removePrep = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (removePrep != SQLITE_OK)
    {
        printColoredText("\n\n\tFailed to prepare Statement! ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
        return;
    }
    sqlite3_bind_int(dbj.preparedStatement, 1, id);
    int removeStep = sqlite3_step(dbj.preparedStatement);
    if (removeStep != SQLITE_DONE)
    {
        printColoredText("\n\n\tFailed to execute statement! ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
    }
    else
    {
        clearScreen();
        std::cout << "\n\n\t";
        printColoredText("Todo Removed Successfully! Press Enter To Continue ", ANSI_COLOR_GREEN);
        std::cin.ignore();
        std::cin.ignore();
    }
}
void Todo::addTodo(std::string title, int priority, databaseActions &dbj, bool status = false)
{

    if (!isDBOpen)
    {
        dbj.openDB();
    }

    std::string query = "INSERT INTO todos(TODOTITLE, PRIORITY, STATUS) VALUES(?, ?, ?)";
    int addPrep = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (addPrep != SQLITE_OK)
    {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(dbj.databaseMain) << std::endl;
        return;
    }

    sqlite3_bind_text(dbj.preparedStatement, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(dbj.preparedStatement, 2, priority);
    sqlite3_bind_int(dbj.preparedStatement, 3, status ? 1 : 0);
    int addStep = sqlite3_step(dbj.preparedStatement);
    if (addStep != SQLITE_DONE)
    {
        clearScreen();
        printColoredText("\n\n\tFailed to execute statement: ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
        return;
    }
    else
    {
        clearScreen();
        printColoredText("\n\n\tAdded todo. Press enter to continue", ANSI_COLOR_GREEN);
        std::cin.ignore();
        return;
    }

    sqlite3_finalize(dbj.preparedStatement);
}
void Todo::showToDos(databaseActions &dbj)
{
    todo.clear();
    clearScreen();
    if (!isDBOpen)
    {
        dbj.openDB();
    }
    std::cout << "SHOWING TODOS!! ";
    std::string query = "SELECT * FROM todos ORDER BY PRIORITY DESC LIMIT 20";

    int showResult = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (showResult != SQLITE_OK)
    {
        printColoredText("Failed to prepare statement: ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
        return;
    }
    auto retrieveLambda = [&dbj, this]()
    {
        while (sqlite3_step(dbj.preparedStatement) == SQLITE_ROW)
        {
            auto id = sqlite3_column_int(dbj.preparedStatement, 0);
            std::string title = ((const char *)sqlite3_column_text(dbj.preparedStatement, 1));
            auto priority = sqlite3_column_int(dbj.preparedStatement, 2);
            auto status = static_cast<bool>(sqlite3_column_int(dbj.preparedStatement, 3));

            auto tuple = std::make_tuple(id, title, priority, status);
            todo.push_back(tuple);
        }
    };
    auto showRetrievedValues = [&dbj, this]()
    {
        std::cout << std::endl
                  << std::setw(5) << "ID"
                  << " " << std::setw(25) << "TITLE"
                  << " " << std::setw(10) << "\tPRIORITY"
                  << " " << std::setw(15) << "\t\tSTATUS\n";
        for (size_t i = 0; i < todo.size(); i++)
        {
            std::cout << std::setw(5) << std::get<0>(todo.at(i))
                      << "  " << std::setw(25) << std::get<1>(todo.at(i))
                      << "  " << std::setw(10) << std::get<2>(todo.at(i));

            if (std::get<3>(todo.at(i)))
            {
                std::cout << "\x1b[32m" << std::setw(15) << "\t\tCompleted"
                          << " \x1b[0m\n";
            }
            else
            {
                std::cout << "\x1b[31m" << std::setw(15) << "\t\t\tNot Completed"
                          << " \x1b[0m\n";
            }
        }
    };

    retrieveLambda();
    showRetrievedValues();

    auto editTodos = [&dbj, this]()
    {
        int ID;
        bool found = false;
        std::cout << "\n\n\t";
        printColoredText("Enter the ID of the Todo you want to edit: ", ANSI_COLOR_YELLOW);
        std::cin >> ID;
        int index = 0;
        clearScreen();
        while (sqlite3_step(dbj.preparedStatement) == SQLITE_ROW || sqlite3_column_int(dbj.preparedStatement, 0) == ID)
        {
            if (sqlite3_column_int(dbj.preparedStatement, 0) == ID)
            {
                std::cout << "\n\n\t";
                printColoredText("EDITING TODO: ", ANSI_COLOR_YELLOW);
                std::cout << std::endl
                          << std::setw(5) << "ID"
                          << " " << std::setw(25) << "TITLE"
                          << " " << std::setw(10) << "\tPRIORITY"
                          << " " << std::setw(15) << "\t\tSTATUS\n";

                auto iterator = std::find_if(todo.begin(), todo.end(), [&](const auto &tuple)
                                             { return std::get<0>(tuple) == ID; });
                if (iterator != todo.end())
                {
                    index = std::distance(todo.begin(), iterator);
                }
                else
                {
                    index = todo.size() - 1;
                }
                std::cout << std::setw(5) << std::get<0>(todo.at(index))
                          << "  " << std::setw(25) << std::get<1>(todo.at(index))
                          << "  " << std::setw(10) << std::get<2>(todo.at(index));

                if (std::get<3>(todo.at(index)))
                {
                    std::cout << "\x1b[32m" << std::setw(15) << "\t\tCompleted"
                              << " \x1b[0m\n";
                }
                else
                {
                    std::cout << "\x1b[31m" << std::setw(15) << "\t\t\tNot Completed"
                              << " \x1b[0m\n";
                }
                found = true;
            }
        }
        if (found)
        {
            std::string newTitle, newPriority;
            int newPRIORITY;
            std::cout << "\n\n\t";
            printColoredText("Leave fields blank that you dont want to edit! ", ANSI_COLOR_CYAN);
            std::cin.ignore();
            printColoredText("\n\n\tEnter New Title: ", ANSI_COLOR_YELLOW);
            std::getline(std::cin, newTitle);
            printColoredText("\n\n\tEdit Priority of the todo: ", ANSI_COLOR_YELLOW);
            std::getline(std::cin, newPriority);

            std::string query = "UPDATE todos SET TODOTITLE = ?, PRIORITY = ? WHERE ID = ?";

            if (newTitle.empty() && newPriority.empty())
            {
                clearScreen();
                printColoredText("\n\n\tNo changes were made. ", ANSI_COLOR_RED);
            }
            else
            {
                if (std::stoi(newPriority) >= 1 && std::stoi(newPriority) <= 10)
                {
                    newPRIORITY = std::stoi(newPriority);
                }
                else if (newTitle.empty())
                {
                    newTitle = ((const char *)sqlite3_column_text(dbj.preparedStatement, 1));
                }
            }
            int updatePrep = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
            sqlite3_bind_text(dbj.preparedStatement, 1, newTitle.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(dbj.preparedStatement, 2, newPRIORITY);
            sqlite3_bind_int(dbj.preparedStatement, 3, ID);
            int updateStep = sqlite3_step(dbj.preparedStatement);
            if (updateStep != SQLITE_DONE)
            {
                clearScreen();
                printColoredText("\n\n\tFailed to execute statement! ", ANSI_COLOR_RED);
                printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
                printColoredText("\n\n\tPress enter to continue! ", ANSI_COLOR_CYAN);
                std::cin.ignore();
                std::cin.ignore();
            }
            else
            {
                clearScreen();
                printColoredText("\n\n\tChanges made successfully! ", ANSI_COLOR_GREEN);
                printColoredText("\n\n\tPress enter to continue! ", ANSI_COLOR_CYAN);
                std::cin.ignore();
                std::cin.ignore();
                return;
            }
        }
        else
        {
            clearScreen();
            std::cout << "\n\n\t";
            printColoredText("Todo Not Found! ", ANSI_COLOR_RED);
            return;
        }
    };

    int ae;
    printColoredText("\t\tEnter any Todo ID to mark it completed \n", ANSI_COLOR_CYAN);
    printColoredText("\n\t\tPress 0 to edit todos by ID or press -1 to go back: ", ANSI_COLOR_CYAN);
    std::cin >> ae;
    if (ae == 0)
    {
        editTodos();
    }
    else if (ae == -1)
    {
        return;
    }
    else if (ae != 0 && ae != -1)
    {
        std::string query = "UPDATE todos SET STATUS = 1 WHERE ID = ? ";
        int updateStatusPrep = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
        if (updateStatusPrep != SQLITE_OK)
        {
            clearScreen();
            printColoredText("\n\n\tFailed to execute statement! ", ANSI_COLOR_RED);
            printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
            printColoredText("\n\n\tPress enter to continue! ", ANSI_COLOR_CYAN);
            std::cin.ignore();
            std::cin.ignore();
        }
        sqlite3_bind_int(dbj.preparedStatement, 1, ae);
        int updateStatusStep = sqlite3_step(dbj.preparedStatement);
        if (updateStatusStep != SQLITE_DONE)
        {
            clearScreen();
            printColoredText("\n\n\tFailed to execute statement! ", ANSI_COLOR_RED);
            printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
            printColoredText("\n\n\tPress enter to continue! ", ANSI_COLOR_CYAN);
            std::cin.ignore();
            std::cin.ignore();
            return;
        }
        else
        {
            clearScreen();
            printColoredText("\n\n\tChanges made successfully! ", ANSI_COLOR_GREEN);
            printColoredText("\n\n\tPress enter to continue! ", ANSI_COLOR_CYAN);
            std::cin.ignore();
            std::cin.ignore();
        }
    }
}
void Todo::highPriorityTodos(databaseActions &dbj)
{
    if (!isDBOpen)
    {
        dbj.openDB();
    }
    std::string query = "SELECT * FROM todos WHERE STATUS = 0 ORDER BY PRIORITY DESC LIMIT 3";

    int highPriorityPrep = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (highPriorityPrep != SQLITE_OK)
    {
        printColoredText("Failed to prepare statement: ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
        return;
    }
    int rowcount = 0;
    while (sqlite3_step(dbj.preparedStatement) == SQLITE_ROW)
    {
        rowcount++;
    }

    if (rowcount >= 1 && rowcount <= 3)
    {
        while (sqlite3_step(dbj.preparedStatement) == SQLITE_ROW)
        {
            std::string title = ((const char *)sqlite3_column_text(dbj.preparedStatement, 1));
            bool status = static_cast<bool>(sqlite3_column_int(dbj.preparedStatement, 3));
            printColoredText("\t\t\t" + title + "\t\t\t\t", ANSI_COLOR_GREEN);
            std::cout << std::setw(50);
            printColoredText(status ? "Completed" : "Not Completed", ANSI_COLOR_RED);
            std::cout << "\n";
        }
    }
}
void Todo::viewCompletedTodos(databaseActions &dbj)
{
    clearScreen();
    std::string query = "SELECT * FROM todos WHERE STATUS = 1 ORDER BY PRIORITY DESC ";
    if (!isDBOpen)
    {
        // printColoredText("Opening database! \n", ANSI_COLOR_GREEN);
        dbj.openDB();
    }
    int completedTodos = sqlite3_prepare_v2(dbj.databaseMain, query.c_str(), -1, &dbj.preparedStatement, nullptr);
    if (completedTodos != SQLITE_OK)
    {
        printColoredText("\n\n\tFailed to prepare statement: ", ANSI_COLOR_RED);
        printColoredText(sqlite3_errmsg(dbj.databaseMain), ANSI_COLOR_RED);
    }
    std::cout << std::endl
              << std::setw(5) << "ID"
              << " " << std::setw(25) << "TITLE"
              << " " << std::setw(10) << "\tPRIORITY"
              << " " << std::setw(15) << "\t\tSTATUS\n";
    while (sqlite3_step(dbj.preparedStatement) == SQLITE_ROW)
    {
        auto id = sqlite3_column_int(dbj.preparedStatement, 0);
        std::string title = ((const char *)sqlite3_column_text(dbj.preparedStatement, 1));
        auto priority = sqlite3_column_int(dbj.preparedStatement, 2);
        auto status = static_cast<bool>(sqlite3_column_int(dbj.preparedStatement, 3));

        std::cout << std::setw(5) << id
                  << "  " << std::setw(25) << title
                  << "  " << std::setw(10) << priority;

        if (status)
        {
            std::cout << "\x1b[32m" << std::setw(15) << "\t\tCompleted"
                      << " \x1b[0m\n";
        }
    }
    std::cout << "\n\n\t";
    printColoredText("Press Enter to continue! ", ANSI_COLOR_CYAN);
    std::cin.ignore();
    std::cin.ignore();
    return;
}