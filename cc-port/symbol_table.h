#pragma once

#include <map>
#include <string>
#include <iomanip>

const uint32_t INITIAL_MEMORY_ADDRESS = 7000;


class SymbolTable {
private:
    std::map<std::string, uint32_t> table;
    uint32_t next_mem_address;

public:
    SymbolTable() {
        next_mem_address = INITIAL_MEMORY_ADDRESS;
    }

    bool add(std::string lexeme) {

        if(this->table.count(lexeme)) {
            std::cout << "\n\n**ERROR** symbol '" << lexeme << 
                    "' already exists in the symbol table with memory address: " 
                        << this->table[lexeme] << "\n\n"; 
            return false;
        }

        this->table[lexeme] = next_mem_address++;
        return true;
    }

    uint32_t find(std::string lexeme) {
         auto f = this->table.find(lexeme);
         if(f == this->table.end()) return 0;
         
         return f->second;
    }

    void print() {
        std::cout << "\nSYMBOL TABLE: \n" << std::string(50, '=') << "\n";
        
        for(auto it = this->table.begin(); 
                    it != this->table.end(); ++it) {
            std::cout << std::setw(15) << it->first 
                    << " : " << it->second << "\n";
        }
    }

    /* not needed for project 3 */
    bool pop(std::string lexeme) {
        auto f = this->table.find(lexeme);
        
        if(f == this->table.end()) {
            std::cout << "\n\n**ERROR** symbol '" 
                    << lexeme << "'"  
                        << " not found in symbol table.\n\n";
            return false;
        }
        this->table.erase(f);
        return true;
    }



};