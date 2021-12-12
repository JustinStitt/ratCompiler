#pragma once
#include "lexer.h"
#include "rules.h"
#include "symbol_table.h"
#include "instruction.h"
#include <map>
#include <iomanip>
#include <stack>

using namespace LexicalAnalyzer;

/* recursive descent parser */
class Parser {
private:
    Lexer lex;
    SymbolTable symbol_table;
    std::vector<Instruction> instr_table;    

    std::string most_recent_qualifier = "";

    std::stack<uint32_t> jumpstack;

    std::string nextState() {
        return this->lex.next()->state;
    }

    std::string nextBuf() {
        return this->lex.next()->buf;
    }

    void scan() {
        printToken();
        auto r = lex.scan();
    }

    void boxMessage(std::string msg) {
        std::cout << "\n" << std::string(49, '=') 
                        << "\n" << "|" << "\t\t\t\t\t\t" << "|" 
                        << "\n|" << "\t\t\t\t\t\t" << "|" 
                        << "\n|\t\t" << msg << std::string(abs(19-msg.size()), ' ')
                        << "\t\t|" 
                        << "\n" << "|" << "  \t\t\t\t\t\t" << "|" 
                        << "\n" << "|" << "\t\t\t\t\t\t" << "|"
                        << "\n" << std::string(49, '=') << "\n" << std::flush;
    }

    void start() {
        Rat21F(); // starting state
        if(nextState() == "Eof" && nextBuf() == "$") {
            boxMessage("Parsing Successful!");
        }
        else {
            boxMessage("Parsing Unsuccessful. See Errors.");
        }
    }

    void printRule(int idx) {
        printToken();
        std::cout << rule[idx] << "\n" << std::flush;
    }

    void printError(std::string msg) {
        std::cout << "\n***" << msg << "***\n" << std::flush;
    }

    void printToken() {
        std::cout << nextState() << " => " << nextBuf() << "\n" << std::flush;
    }

    Instruction gen_instr(std::string instr_name, uint32_t addy) {
        Instruction instr(instr_name, addy);
        instr_table.push_back(instr);
        return instr;
    }

    void back_patch(uint32_t jump_address) {
        uint32_t addy = jumpstack.top(); jumpstack.pop();
        instr_table[addy-1].oprnd = jump_address;
    }

    
    

public:
    Parser(std::string fn) : lex(fn) {
        start();
    }

    void print_instr() {
        std::cout << "\nInstructions Table: \n" << std::string(50, '=') << "\n";
        for(auto&& instr : instr_table) {
            std::cout << std::setw(15) << instr.address << std::setw(6)
                        << instr.op << std::setw(5) << (instr.oprnd ? std::to_string(instr.oprnd) : "") << "\n";
        }
    }

    void print_symbol() {
        symbol_table.print();
    }

    void Rat21F() { // R1
        printRule(1);

        OptFunctionDefinitions();
        if(nextState() == "Separator" && nextBuf() == "#") {
            scan();
            OptDeclarationList();
            StatementList();
            if(nextState() == "Separator" && nextBuf() == "#") {
                scan();
            } else printError("Missing ending symbol '#'.");
        } else printError("Missing starting symbol '#' after function definitions.");
    }

    void OptFunctionDefinitions() { // R2        
        printRule(2);

        if(nextState() == "Keyword" && nextBuf() == "function") {
            FunctionDefinitions();
        }
        else {
            Empty();
        }

    }

    void FunctionDefinitions() { // R3
        printRule(3);

        if(Function()) {
            FunctionDefinitions();
        }
    }

    bool Function() { // R4
        printRule(4);
        
        if(nextState() == "Keyword" && nextBuf() == "function") {
            scan();
            if(nextState() == "Identifier") {
                scan();
                if(nextState() == "Separator" && nextBuf() == "(") {
                    scan();
                    OptParameterList();
                    if(nextState() == "Separator" && nextBuf() == ")") {
                        scan();
                        OptDeclarationList();
                        Body();
                        return true;
                    } else printError("Missing closing ')' for function.");
                } else printError("Missing opening '(' for function.");
            }
        }
        return false;
    }

    void OptParameterList() { // R5
        printRule(5);

        if(ParameterList()) {

        } else {
            Empty();
        }
    }

    bool ParameterList() { // R6
        printRule(6);

        auto nb = nextBuf();
        if(nextState() == "Identifier" || nb == "integer" || nb == "real" || nb == "boolean" ) {
            Parameter();
            ParameterList();
            return true;
        }
        return false;
    }

    void Parameter() { // R7
        printRule(7);

        IDs();
        Qualifier();
    }

  

        void Qualifier() { // R8
        printRule(8);
        
        auto nb = nextBuf();
        if(nextState() == "Keyword" && nb == "integer" || nb == "boolean" || nb == "real") {
            scan();
            most_recent_qualifier = nb;
        }
        else printError("Qualifier type not found in declaration.");
    }

    void Body() { // R9
        printRule(9);

        if(nextState() == "Separator" && nextBuf() == "{") {
            scan();
            StatementList();
            if(nextState() == "Separator" && nextBuf() == "}") {
                scan();
            } else printError("Missing closing '}' for Body.");
        } else printError("Missing opening '{' for Body.");
    }

    void OptDeclarationList() { // R10
        printRule(10);

        auto nb = nextBuf();
        // if qualifier
        if(nextState() == "Keyword" && (nb == "integer" || nb == "boolean" || nb == "real")) {
            DeclarationList();
        } else Empty();
    }

    void DeclarationList() { // R11
        printRule(11);
        
        Declaration();
        
        if(nextState() == "Separator" && nextBuf() == ";") {
            scan();
            auto nb = nextBuf();
            if(nextState() == "Keyword" && (nb == "integer" || nb == "boolean" || nb == "real")) {
                DeclarationList();
            }
        } else printError("Missing ';' in declaration list.");
    }

    void Declaration() { // R12
        printRule(12);

        Qualifier();
        auto ids = IDs();
        symbol_table.set_batch_id_types(ids, most_recent_qualifier);
    }



    std::vector<std::string> IDs() { // R13
        printRule(13);
        std::vector<std::string> ids;
        if(nextState() == "Identifier") {
            ids.push_back(nextBuf());
            scan();
            if(nextState() == "Separator" && nextBuf() == ",") { // id list
                scan();
                std::vector<std::string> v = IDs();
                ids.insert(ids.end(), v.begin(), v.end());
            }
            return ids;
        }
        else printError("Type{" + nextState() + "} not allowed in ID list.");
        return {};
    }

    void StatementList() { // R14
        printRule(14);

        if(Statement()) {
            StatementList();
        }

    }

    bool Statement() { // R15
        printRule(15);
        // compound, assign, If, Return, Print, Scan, While

        // identifiy compound
        if(nextState() == "Separator" && nextBuf() == "{") {
            Compound();
        }
        else if(nextState() == "Identifier") { // assign
            Assign();
        }
        else if(nextState() == "Keyword") {
            auto nb = nextBuf();
            if(nb == "if") {
                If();
            }
            else if(nb == "return"){
                Return();
            }
            else if(nb == "put") {
                Print();
            }
            else if(nb == "get") {
                Scan();
            }
            else if(nb == "while") {
                While();
            }
            else {
                printError("Wrong type {" + nextState() + "} inputed.");
            }
        }
        else {
            return false;
        }
        return true;
    }

    void Compound() { // R16
        printRule(16);

        if(nextState() == "Separator" && nextBuf() == "{") {
            scan();
            StatementList();
            if(nextState() == "Separator" && nextBuf() == "}") {
                scan();
            } else printError("Missing closing '}' for Compound.");
        }
    }

    void Assign() { // R17
        printRule(17);

        if(nextState() == "Identifier") {
            std::string lexeme_being_assigned = nextBuf();
            scan();
            if(nextState() == "Operator" && nextBuf() == "=") {
                /* store lexeme_being_assigned to symbol table */
                symbol_table.add(lexeme_being_assigned);
                scan();
                Expression();
                auto i = gen_instr("POPM", symbol_table.find(lexeme_being_assigned));
                if(nextState() == "Separator" && nextBuf() == ";") {
                    scan();
                } else printError("Missing ';' at assign end.");
            }
        }
    }

    void If() { // R18
        printRule(18);

        if(nextState() == "Keyword" && nextBuf() == "if") {
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();
                auto i = Condition();
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    Statement();
                    if(nextState() == "Keyword" && nextBuf() == "endif") {
                        scan();
                        auto ii = gen_instr("LABEL", 0);
                        back_patch(ii.address);
                    }
                    else if(nextState() == "Keyword" && nextBuf() == "else") {
                        scan();
                        Statement();
                        if(nextState() == "Keyword" && nextBuf() == "endif") {
                            scan();
                            auto ii = gen_instr("LABEL", 0);
                            back_patch(ii.address);
                        } else printError("Missing 'endif' after if.");
                    }
                } else printError("Missing closing ')' for if statement.");
            } else printError("Missing opening '(' for if statement.");
        }
    }

    void Return() { // R19
        printRule(19);

        if(nextState() == "Keyword" && nextBuf() == "return") {
            scan();
            if(nextState() == "Separator" && nextBuf() == ";") {
                scan();
            } else {
                Expression();
                if(nextState() == "Separator" && nextBuf() == ";") {
                    scan();
                } else printError("Missing ';' after return expression.");
            }
        }
    }

    void Print() { // R20
        printRule(20);

        if(nextState() == "Keyword" && nextBuf() == "put") {
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();
                Expression();
                auto i = gen_instr("STDOUT", 0);
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    if(nextState() == "Separator" && nextBuf() == ";") {
                        scan();
                    } else printError("Missing ';' after put.");
                } else printError("Missing closing ')' after put.");
            } else printError("Missing opening '(' after put.");
        }
    }

    void Scan() { // R21
        printRule(21);

        if(nextState() == "Keyword" && nextBuf() == "get") {
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();                
                auto ids = IDs();
                for(auto id : ids) {
                    symbol_table.add(id);
                    symbol_table.set_id_to_type(id, "integer");
                    auto addy = symbol_table.find(id);
                    auto i = gen_instr("STDIN", 0);
                    gen_instr("POPM", addy);
                }
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    if(nextState() == "Separator" && nextBuf() == ";") {
                        scan();
                    } else printError("Missing ';' after get.");
                } else printError("Missing closing ')' after get.");
            } else printError("Missing opening '(' after get.");
        }
    }

    void While() { // R22
        printRule(22);

        if(nextState() == "Keyword" && nextBuf() == "while") {
            auto i = gen_instr("LABEL", 0);
            uint32_t address = i.address;
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();
                Condition();
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    Statement();
                    auto ii = gen_instr("JUMP", address);
                    back_patch(ii.address + 1);
                } else printError("Missing closing ')' for while.");
            } else printError("Missing opening '(' for while.");
        }
    }

    Instruction Condition() { // R23
        printRule(23);
        Expression();
        auto r = Relop();
        if(r == "NULL OPERATOR") return Instruction("", 0);
        scan();
        Expression();

        auto i = gen_instr(r, 0);
        auto ii = gen_instr("JUMPZ", 0);

        jumpstack.push(ii.address);

        return ii;
    }

    std::string Relop() { // R24
        printRule(24);

        auto nb = nextBuf();
        if(nextState() != "Operator") {
            printError("Invalid relational operator.");
            return "NULL OPERATOR";
        }
        if(nb == "<") {
            return "LES";
        }
        else if(nb == ">") {
            return "GRT";
        }
        else if(nb == "==") {
            return "EQU";
        }
        else if(nb == "!=") {
            return "NEQ";
        }
        return "NULL OPERATOR";
    }
/*
    void Expression() { // R25
        //TO-DO remove left recursion.
        printRule(25);

        if(Term()) {
            if(nextState() == "Operator") {
                if(nextBuf() == "+") {
                    scan();
                    Expression();
                }
                else if(nextBuf() == "-") {
                    scan();
                    Expression();
                } //else printError("Unrecognized operator in expression.");
            }
        }
        
    }*/

    void Expression() { // R25
        printRule(25);

        Term();
        Expression_Prime();
    }

    void Expression_Prime() { // R25.1
        printRule(251);
        if(nextState() == "Operator") {
            if(nextBuf() == "+"){
                scan();
                Term();
                auto i = gen_instr("ADD", 0);
                Expression_Prime();
            }
            else if(nextBuf() == "-") {
                scan();
                Term();
                auto i = gen_instr("SUB", 0);
                Expression_Prime();
            }
        }
    }
/*
    bool Term() { // R26
        //TO-DO remove left recursion.
        printRule(26);

        if(Factor()) {
            if(nextState() == "Operator") {
                if(nextBuf() == "*") {
                    scan();
                    Term();
                }
                else if(nextBuf() == "/") {
                    scan();
                    Term();
                }
            }   
        }
        else return false;

        return true;
        
    }*/

    void Term() { // R26
        printRule(26);
        Factor();
        Term_Prime();
    }

    void Term_Prime() { // R 26.1
        printRule(261);
        if(nextState() == "Operator") {
            if(nextBuf() == "*") {
                scan();
                Factor();
                auto i = gen_instr("MUL", 0);
                Term_Prime();
            }
            else if(nextBuf() == "/") {
                scan();
                Factor();
                auto i = gen_instr("DIV", 0);
                Term_Prime();
            }
        }
        else Factor();
    }

    bool Factor() { // R27
        printRule(27);

        if(nextState() == "Operator" && nextBuf() == "-") {
            scan();
            return Primary();
        } 

        return Primary();
    }

    bool Primary() { // R28
        printRule(28);

        if(nextState() == "Identifier") {
            auto i = gen_instr("PUSHM", symbol_table.find(nextBuf()));
            std::cout << "\nnext buf is: " << nextBuf() << "\n";
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();
                IDs();
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                } else printError("Missing Closing Parenthesis for ID list, next state was actually: " + nextState() + " => " + nextBuf());
            }   
        }
        else if(nextState() == "Integer") {
            auto i = gen_instr("PUSHI", std::stoi(nextBuf()));
            scan();
        }
        else if(nextState() == "Separator" && nextBuf() == "(") {
            scan();
            Expression();
            if(nextState() == "Separator" && nextBuf() == ")") {
                scan();
            } else printError("Missing closing parenthesis for primary expression.");
        }
        else if(nextState() == "Real") {
            scan();
        }
        else if(nextState() == "Keyword" && (nextBuf() == "true" || nextBuf() == "false")) {
            scan();
        } else return false;

        return true;

    }

    void Empty() { // R29
        printRule(29);
    }


}; // end Parser class