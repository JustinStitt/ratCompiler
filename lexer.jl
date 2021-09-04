include("fsm.jl")

struct alphabet
    letters::Set{Char}
    digits::Set{Char}
    symbols::Set{Char}
    whitespace::Set{Char}
    alphabet() = new(
        Set([[x for x in 'a':'z']..., 
             [x for x in 'A':'Z']...,
             '_']),
        Set(Char.('0' .+ range(0, 9, step=1))),
        Set([';','(',')','+', '*','/', 
             '\\', '-', ',', 
             '[', ']', '!', '>', '<', 
             '{', '}', '%', '&', '|', '^',
             '=', '.', '?', ':', '\'', '\"']),
        Set([' ', '\n']),
    )
end

function parseInput(q0, inp, Σ)
    for c in inp
        if c in Σ.letters
            q0 = step(q0, Letter(c))
        elseif c in Σ.digits
            q0 = step(q0, Digit(c))
        elseif c in Σ.symbols
            q0 = step(q0, Symbol(c))
        elseif c in Σ.whitespace
            q0 = step(q0, Whitespace())
        else
            println("bad input")
        end
    end
end

function test(test_cases)
    Σ = alphabet()
    for case in test_cases
        println("Input: ", case)
        parseInput(Start(), case, Σ) # ask lexer to step through input buffer
        display(store) # show token=>lexeme store buffer
        empty!(store) # empty our token=>lexeme store buffer
        println()
    end
end

test_cases = ["231+num+foo+3.14+(6-2.45+size);/*this is a comment 3.1415;*/", 
              "while  (fahr <= upper)   a = 23.00;  /* this is sample */"
             ]

test(test_cases)