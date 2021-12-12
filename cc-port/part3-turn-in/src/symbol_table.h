#pragma once

#include <map>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>

const uint32_t INITIAL_MEMORY_ADDRESS = 7000;


class SymbolTable {
private:
    std::map<std::string, uint32_t> table;
    std::map<std::string, std::string> id_to_type;
    uint32_t next_mem_address;
    std::vector<std::string> errors;

public:
    SymbolTable() {
        next_mem_address = INITIAL_MEMORY_ADDRESS;
    }

    bool add(std::string lexeme, std::string _type = "") {

        if(this->table.count(lexeme)) {
            std::cout << "\n\n**ERROR** symbol '" << lexeme << 
                    "' already exists in the symbol table with memory address: " 
                        << this->table[lexeme] << "\n\n"; 
            return false;
        }

        this->table[lexeme] = next_mem_address++;

        if(_type != "") {
            id_to_type[lexeme] = _type;
        }

        return true;
    }

    uint32_t find(std::string lexeme) {
        auto f = this->table.find(lexeme);
        if(f == this->table.end()) {
            errors.push_back("**ERROR** symbol '" + lexeme + "'" + " not found in symbol table.");
            return 0;
        }
         
         return f->second;
    }

    void print() {
        std::cout << "\nSYMBOL TABLE: \n" << std::string(50, '=') << "\n";

        for(auto it = this->table.begin(); 
                    it != this->table.end(); ++it) {
            std::cout << std::setw(15) << it->first 
                    << " : " << it->second 
                    << std::setw(10) << id_to_type[it->first] << "\n";
        }
        std::cout << "" << std::string(50, '=') << "\n";
        for(auto&& err : this->errors) {
            std::cout << err << "\n";
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

    void set_batch_id_types(std::vector<std::string>& ids, std::string _type) {
        for(auto& id : ids) {
            id_to_type[id] = _type;
        }
    }

    void set_id_to_type(std::string& id, std::string _type) {
        id_to_type[id] = _type;
    }
};