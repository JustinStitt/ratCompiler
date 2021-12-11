#pragma once
#include "lexer.h"
#include "rules.h"
#include "symbol_table.h"
#include <map>

using namespace LexicalAnalyzer;

/* recursive descent parser */
class Parser {
private:
    Lexer lex;
    SymbolTable symbol_table;
    

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

public:
    Parser(std::string fn) : lex(fn) {
        start();
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
        IDs();
    }



    void IDs() { // R13
        printRule(13);

        if(nextState() == "Identifier") {
            scan();
            if(nextState() == "Separator" && nextBuf() == ",") { // id list
                scan();
                IDs();
            }
        }
        else printError("Type{" + nextState() + "} not allowed in ID list.");
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
            scan();
            if(nextState() == "Operator" && nextBuf() == "=") {
                scan();
                Expression(); // TO-DO
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
                Condition();
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    Statement();
                    if(nextState() == "Keyword" && nextBuf() == "endif") {
                        scan();
                    }
                    else if(nextState() == "Keyword" && nextBuf() == "else") {
                        scan();
                        Statement();
                        if(nextState() == "Keyword" && nextBuf() == "endif") {
                            scan();
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
                IDs();
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
            scan();
            if(nextState() == "Separator" && nextBuf() == "(") {
                scan();
                Condition();
                if(nextState() == "Separator" && nextBuf() == ")") {
                    scan();
                    Statement();
                } else printError("Missing closing ')' for while.");
            } else printError("Missing opening '(' for while.");
        }
    }

    void Condition() { // R23
        printRule(23);
        Expression(); // TO-DO
        Relop();
        Expression(); // TO-DO
    }

    void Relop() { // R24
        printRule(24);

        auto nb = nextBuf();
        if(nextState() == "Operator" && (nb == "==" || nb == "!=" || nb == ">" || nb == "<" || nb == "<=" || nb == "=>")) {
            scan();
        } else printError("Invalid relational operator.");
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
                Expression_Prime();
            }
            else if(nextBuf() == "-") {
                scan();
                Term();
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
                Term_Prime();
            }
            else if(nextBuf() == "/") {
                scan();
                Factor();
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