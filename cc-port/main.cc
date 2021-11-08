#include "parser.h"

using namespace LexicalAnalyzer;

int main(int arg, char** args) {
   if(arg < 2) {
        std::cout << "Please provide a filename... (example: \"test.rat\")\n";
        return -1;
    }
    else if(arg > 2) {
        std::cout << "Too many arguments provided.. (expected: {filename})\n"; 
        return -1;
    }

    Parser parser(args[1]);

    return 0;
}