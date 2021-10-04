include("./fsm.jl")

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
            '{', '}', '%', '&', '|', '^','#',
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
            println("**bad input: "*c)
        end
    end
end

function main()
    println("\nEnter RAT21F input file: ")
    input_file = readline()

    try
        _f = open("./"*input_file) # try to open file
    catch e
        println("Cannot open input file \""*input_file*"\"") # couldn't find file in pwd
        main()
        return
    end

    raw_input = read(input_file, String) # parse file to string raw

    Σ = alphabet() # create alphabet sigma

    println("Input: ", raw_input)
    parseInput(Start(), raw_input, Σ) # ask lexer to step through input buffer
    display() # show token=>lexeme store buffer
    empty!() # empty our token=>lexeme store buffer
    println()

    println("\n\n Press any key to quit... ")
    _t = readline()
end

main()
