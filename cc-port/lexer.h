#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using str_set = std::unordered_set<std::string>;
using chr_set = std::unordered_set<char>;

namespace LexicalAnalyzer {

    enum TYPE {letter = 0, digit, underscore, 
            dot, symbol, slash, asterisk, whitespace, bad};

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

    struct Token {
        std::string state;
        std::string buf;

        Token(std::string _s, std::string _b) : state(_s), buf(_b) {}
    };

    class Lexer {
    private:
        chr_set w, d, s, ws;
        str_set separators, operators, keywords;
        std::unordered_map<size_t, std::string> accepting_states = 
                    {
                        {1, "Identifier"}, {2, "Integer"}, {4, "Real"},
                        {5, "Operator"},   {6, "Operator"},{9, "Comment"},
                        {10, "Separator"}, {11, "Keyword"}
                    };

        static const size_t NUM_STATES = 10,
                     NUM_TRANSITIONS = 7;
        std::array<std::array<int, NUM_TRANSITIONS>, NUM_STATES> tmat =
                    {{/*    w  d  _  .  s  /  *   */
                    /*0*/  {1, 2, 0, 0, 5, 6, 5},
                    /*1*/  {1, 1, 1, 0, 0, 0, 0},
                    /*2*/  {0, 2, 0, 3, 0, 0, 0},
                    /*3*/  {0, 4, 0, 0, 0, 0, 0},
                    /*4*/  {0, 4, 0, 0, 0, 0, 0},
                    /*5*/  {0, 0, 0, 0, 5, 0, 5},
                    /*6*/  {0, 0, 0, 0, 0, 0, 7},
                    /*7*/  {7, 7, 7, 7, 7, 7, 8},
                    /*8*/  {7, 7, 7, 7, 7, 9, 7},
                    /*9*/  {0, 0, 0, 0, 0, 0, 0}
                    }};

        std::string raw_inp;
        std::string tok_buf;
        size_t cstate, inp_idx;
    
        void readin(std::string filename) {
            std::ifstream input_file(filename);
            std::stringstream buffer;
            buffer << input_file.rdbuf();
            this->raw_inp = buffer.str();
        }

        TYPE classifyChar(const char& c) {
            if(w.count(c)) {
                return TYPE::letter;
            }
            else if(d.count(c)) {
                return TYPE::digit;
            }
            else if(c == '_') {
                return TYPE::underscore;
            }
            else if(c == '/') {
                return TYPE::slash;
            }
            else if(c == '.') {
                return TYPE::dot;
            }
            else if(c == '*') {
                return TYPE::asterisk;
            }
            else if(s.count(c)) {
                return TYPE::symbol;
            }
            else if(ws.count(c)) {
                return TYPE::whitespace;
            }
            return TYPE::bad;
        }


        Token* step() {
            /*  use transition matrix and current state 
                alongside next character in raw input */

            if(this->inp_idx >= this->raw_inp.size()) {
                this->cstate = 0;
                Token* tmp = new Token("Eof", "$");
                return tmp;
            }; // reached end of input
            char nc = this->raw_inp[this->inp_idx++];

            if(this->cstate == 0) {
                this->tok_buf.clear();
            }

            this->tok_buf += nc;
            TYPE inp_type = classifyChar(nc);
            int new_state;
            if(inp_type == TYPE::bad) {
                std::cout << "bad char: " << nc << "\n";
            }
            else if(inp_type == TYPE::whitespace && this->cstate != 7) {
                new_state = -1;
            }
            else {
                new_state = this->tmat[this->cstate][inp_type];
                if(new_state != -1)
                    this->cstate = new_state;
            }

            if(this->separators.count(this->tok_buf)) {
                Token* tmp = new Token("Separator", this->tok_buf);
                this->tok_buf.clear();
                this->cstate = 0;
                return tmp;
            }

            Token* nt = nullptr;
            if(new_state == -1 && this->cstate) { // no transition found
                this->tok_buf.pop_back();
                if(accepting_states[this->cstate].size() ) { // are we in an accepting state?
                    if(accepting_states[this->cstate] != "Comment") {
                        if(accepting_states[this->cstate] == "Identifier" && this->keywords.count(this->tok_buf)) {
                            this->cstate = 11;
                        }
                        else if(accepting_states[this->cstate] == "Operator" && this->separators.count(this->tok_buf)) {
                            this->cstate = 10;
                        }
                        nt = new Token(accepting_states[this->cstate], this->tok_buf);
                        // std::cout << "\n" <<accepting_states[this->cstate] << " *=>* " << this->tok_buf << "\n";
                    }
                } else { // not in an accepting state (improper token)
                    std::cout << "Bad Token => " << this->tok_buf << " cstate: " << this->cstate << "\n";
                }
                this->cstate = 0;
                this->tok_buf.clear();
                this->inp_idx -= 1;
            }
            return nt;
        }

    public:
        

        Lexer() {
            throw std::invalid_argument("missing filename... (example: \"test.rat\")\n");
        }

        Lexer(std::string fn) : cstate(0), inp_idx(0), tok_buf("") {
            util::fillLetters(this->w);
            util::fillDigits(this->d);
            /* fill symbols */
            this->s = {';','(',')','+', '*','/', 
                '\\', '-', ',', '`', '~',
                '[', ']', '!', '>', '<', /*'$',*/
                '{', '}', '%', '&', '|', '^','#',
                '=', '.', '?', ':', '\'', '\"'   };
            this->ws = {' ', '\n', '\t'};
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
            if(this->raw_inp.size() < 1) {
                throw std::domain_error("\nFile is empty! Try again.\n");
            }
            this->raw_inp.push_back(' ');

        }

        Token* next() { // lookahead 1
            /* snapshot everything we want to remain */
            auto tok_buf_snapshot = this->tok_buf;
            auto inp_idx_snapshot = this->inp_idx;
            auto cstate_snapshot = this->cstate;
            /* scan next token */
            auto ret = this->scan();
            /* restore snapshots */
            this->tok_buf = tok_buf_snapshot;
            this->inp_idx = inp_idx_snapshot;
            this->cstate = cstate_snapshot;
            return ret;
        }

        Token* scan() {
            Token* ret = nullptr;
            while(ret == nullptr) {
                ret = this->step();
            }
            return ret;
        }

        

    }; // end Lexer class

} // end Lexer namespace

