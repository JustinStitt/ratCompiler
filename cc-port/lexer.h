#pragma once
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using str_set = std::unordered_set<std::string>;
using chr_set = std::unordered_set<char>;

namespace LexicalAnalyzer {

    namespace util {

        void fillLetters(chr_set& to_fill) {
            chr_set tmp;
            /* [a-z] */
            for(char c = 'a'; c <= 'z'; ++c) {
                tmp.insert(c);
            }

            /* [A-Z] */
            for(char c = 'A'; c <= 'Z'; ++c) {
                tmp.insert(c);
            }

            to_fill = std::move(tmp);
        }

        void fillDigits(chr_set& to_fill) {
            /* [0-9] */
            for(char c = '0'; c <= '9'; ++c) {
                to_fill.insert(c);
            }
        }

        template<typename T, size_t N, size_t M>
        void replaceZeroes(std::array<std::array<T, M>, N>& to_replace) {
            for(size_t r = 0; r < N; ++r) {
                for(size_t c = 0; c < M; ++c) {
                    if(to_replace[r][c] == 0) to_replace[r][c] = -1;
                }
            }
        }

        template<typename T>
        void print(const T& to_print) {
            /* print out elements of a set (debug) */
            std::cout << "\nset: ";
            for(auto it = to_print.begin(); it != to_print.end(); ++it) {
                std::cout << *it << " ";
            } std::cout << "\n";
        }

    } // end util namespace

    class Lexer {
    private:
        chr_set w, d, s;
        str_set separators, operators, keywords;

        std::string next_tok;
        std::string raw_inp;

        enum TYPE {letter = 0, digit, underscore, 
            dot, symbol, slash, asterisk};

        static const size_t NUM_STATES = 10,
                     NUM_TRANSITIONS = 7;
        std::array<std::array<int, NUM_TRANSITIONS>, NUM_STATES> tmat =
                    {{/*     w  d  _  .  s  /  *   */
                    /*0*/  {1, 2, 0, 0, 5, 6, 0},
                    /*1*/  {1, 1, 1, 0, 0, 0, 0},
                    /*2*/  {0, 2, 0, 3, 0, 0, 0},
                    /*3*/  {0, 4, 0, 0, 0, 0, 0},
                    /*4*/  {0, 4, 0, 0, 0, 0, 0},
                    /*5*/  {0, 0, 0, 0, 5, 0, 0},
                    /*6*/  {0, 0, 0, 0, 0, 0, 7},
                    /*7*/  {7, 7, 7, 7, 7, 7, 8},
                    /*8*/  {7, 7, 7, 7, 7, 9, 7},
                    /*9*/  {0, 0, 0, 0, 0, 0, 0}
                    }};
    
        void readin(std::string filename) {
            std::ifstream input_file(filename);
            std::stringstream buffer;
            buffer << input_file.rdbuf();
            this->raw_inp = buffer.str();
        }

    public:
        Lexer() {
            throw std::invalid_argument("Please provide a filename... (example: \"test.rat\")\n");
        }

        Lexer(std::string fn) : next_tok("") {
            util::fillLetters(this->w);
            util::fillDigits(this->d);
            /* fill symbols */
            this->s = {';','(',')','+', '*','/', 
                '\\', '-', ',', '`', '~',
                '[', ']', '!', '>', '<', '$',
                '{', '}', '%', '&', '|', '^','#',
                '=', '.', '?', ':', '\'', '\"'   };
            /* fill separators */
            this->separators = {";", "(", ")", "#", ",", "{", "}"};
            /* fill operators */
            this->operators = {"==", "<", ">", "<=", ">=", "!="
                "+", "-", "*", "/", "="};
            /* fill keywords */
            this->keywords = {"integer", "if", "else", "endif", 
                "while", "return", "get", "put", 
                "boolean", "real", "function",
                "true", "false"};
            /* replace 0's in transition matrix with -1's */
            util::replaceZeroes<int, NUM_STATES, NUM_TRANSITIONS>(this->tmat);
            this->readin(fn);
        }

        


    }; // end Lexer class

} // end Lexer namespace

