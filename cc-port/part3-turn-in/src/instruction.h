#pragma once

#include <string>

struct Instruction {
    static uint32_t static_address;
    uint32_t address, oprnd;
    std::string op;

    Instruction(std::string _op, uint32_t _oprnd) : 
                 op(_op), oprnd(_oprnd) {
        
        address = Instruction::static_address++;
    }

};

uint32_t Instruction::static_address = 1;