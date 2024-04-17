#include <iostream>
#include <cmath>
#include <string>
#include "./utils.hpp" // Include the header file with color functions

template<typename T>
class Calculator {
public:
    void run() {
        char choice;
        do {
            displayMenu();
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            executeChoice(choice);
        } while (choice != 'q' && choice != 'Q');
    }

private:
    void displayMenu() {
        std::cout << "\n===== Calculator Menu =====\n";
        std::cout << "1. Addition\n";
        std::cout << "2. Subtraction\n";
        std::cout << "3. Multiplication\n";
        std::cout << "4. Division\n";
        std::cout << "5. Exponentiation\n";
        std::cout << "6. Square Root\n";
        std::cout << "7. Memory Operations\n";
        std::cout << "Q. Quit\n";
        std::cout << "===========================\n";
    }

    void executeChoice(char choice) {
        switch (choice) {
            case '1':
                performOperation(&Calculator::addition);
                break;
            case '2':
                performOperation(&Calculator::subtraction);
                break;
            case '3':
                performOperation(&Calculator::multiplication);
                break;
            case '4':
                performOperation(&Calculator::division);
                break;
            case '5':
                performOperation(&Calculator::exponentiation);
                break;
            case '6':
                performOperation(&Calculator::squareRoot);
                break;
            case '7':
                performMemoryOperations();
                break;
            case 'q':
            case 'Q':
                std::cout << "Exiting calculator...\n";
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
        }
    }

    void performOperation(T (Calculator::*operation)(T, T)) {
        T num1, num2;
        if (operation == &Calculator::squareRoot) {
            std::cout << "Enter a number: ";
            std::cin >> num1;
            std::cout << "Result: ";
            printColoredText(std::to_string((this->*operation)(num1, num1)), ANSI_COLOR_GREEN);
            std::cout << std::endl;
        } else {
            std::cout << "Enter two numbers: ";
            std::cin >> num1 >> num2;
            std::cout << "Result: ";
            printColoredText(std::to_string((this->*operation)(num1, num2)), ANSI_COLOR_GREEN);
            std::cout << std::endl;
        }
    }

    T addition(T a, T b) {
        return a + b;
    }

    T subtraction(T a, T b) {
        return a - b;
    }

    T multiplication(T a, T b) {
        return a * b;
    }

    T division(T a, T b) {
        if (b == 0) {
            std::cout << ANSI_COLOR_RED << "Error! Division by zero." << ANSI_COLOR_RESET << std::endl;
            return std::numeric_limits<T>::quiet_NaN();
        } else {
            return a / b;
        }
    }

    T exponentiation(T base, T exponent) {
        return std::pow(base, exponent);
    }

    T squareRoot(T num1, T num2) {
        if (num1 < 0) {
            std::cout << ANSI_COLOR_RED << "Error! Cannot find square root of a negative number." << ANSI_COLOR_RESET << std::endl;
            return std::numeric_limits<T>::quiet_NaN();
        } else {
            return std::sqrt(num1);
        }
    }

    void performMemoryOperations() {
        static T memory = 0.0;
        char choice;
        std::cout << "M. Store result in memory\n";
        std::cout << "R. Recall value from memory\n";
        std::cout << "C. Clear memory\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        switch (choice) {
            case 'M':
            case 'm':
                std::cout << "Enter value to store in memory: ";
                std::cin >> memory;
                std::cout << "Value stored in memory.\n";
                break;
            case 'R':
            case 'r':
                std::cout << "Value in memory: " << memory << std::endl;
                break;
            case 'C':
            case 'c':
                memory = 0.0;
                std::cout << "Memory cleared.\n";
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
        }
    }
};

int main() {
    Calculator<double> calc;
    calc.run();
    return 0;
}
