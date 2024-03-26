#include <iostream>
constexpr std::string_view ANSI_COLOR_RED     = "\x1b[31m";
constexpr std::string_view ANSI_COLOR_GREEN   = "\x1b[32m";
constexpr std::string_view ANSI_COLOR_YELLOW  = "\x1b[33m";
constexpr std::string_view ANSI_COLOR_BLUE    = "\x1b[34m";
constexpr std::string_view ANSI_COLOR_MAGENTA = "\x1b[35m";
constexpr std::string_view ANSI_COLOR_CYAN    = "\x1b[36m";
constexpr std::string_view ANSI_COLOR_RESET   = "\x1b[0m"; 
inline void printColoredText(std::string text, std::string_view colorCode) {
    std::cout << colorCode << text << ANSI_COLOR_RESET ;
}
void clearScreen()
{
#ifdef _WIN32
    {

        std::system("cls");
    }
#else
    {
        std::system("clear");
    }
#endif
}
void checkForEmptyField(std::string field)
{
    if (field.empty())
    {
        std::cout << "\n\n\tthis field cannot be empty!" << std::endl;
        exit(0);
    }
}
