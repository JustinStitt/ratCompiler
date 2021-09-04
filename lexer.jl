include("fsm.jl")

struct alphabet
    letters::Set{Char}
    digits::StepRange{Int64, Int64}
    symbols::Set{Char}
    whitespace::Set{Char}
    alphabet() = new(
        Set([[x for x in 'a':'z']..., 
             [x for x in 'A':'Z']...,
             '_']),
        range(1, 10, step=1),
        Set([';', '+', '*','/', 
             '\\', '-', ',', '(', ')', 
             '[', ']', '!', '>', '<', 
             '{', '}', '%', '&', '|', '^',
             '=', '.', '?', ':', '\'', '\"']),
        Set([' ', '\n'])
    )
end

# define alphabet (set of valid characters)
Σ = alphabet() 

q0 = Start() # starting state

transition_buffer = [Digit('2'), Digit('3'), Digit('1'), Symbol('+'), 
                Letter('n'), Letter('u'), Letter('m'), Symbol('+'), 
                Letter('f'), Letter('o'), Letter('o'), Symbol('+'),
                Digit('3'), Letter('.'), Digit('1'), Digit('4'), 
                Symbol(';')]

reduce(step, transition_buffer, init=q0)
 
#=
231+num+foo+3.14;
=#