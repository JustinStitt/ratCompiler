abstract type State end
abstract type Transition end

InvalidState = ArgumentError

#=
    STATES
=#
struct Start <: State # start inherits from State (is of type State)
    Start() = new()
end

struct Real <: State
    raw::Array{Char}
    Real() = new([])
end

struct Identifier <: State
    raw::Array{Char}
    Identifier() = new([])
end

struct Operator <: State
    raw::Array{Char}
    Operator() = new([])
end

#=
    END STATES
=#

#=
    TRANSITIONS
=#
struct Digit <: Transition
    raw::Char
    Digit(x) = new(x)
end

struct Symbol <: Transition
    raw::Char
    Symbol(x) = new(x)
end

struct Letter <: Transition
    raw::Char
    Letter(x) = new(x)
end

struct Whitespace <: Transition end

#=
    END TRANSITIONS
=#

#=
    STEPS
=#
step(::State, ::Transition) = throw(InvalidState) # default InvalidState catch-all
function step(state::Start, ::Whitespace) state end # whitespace at start is fine

function step(state::Start, transition::Digit)
    state = Real()
    push!(state.raw, transition.raw)
    state
end

function step(state::Real, transition::Digit)
    push!(state.raw, transition.raw) # cant push to string not mutable
    state
end

function step(state::Real, ::Transition)
    println("Found Real: ", state.raw)
    state = Start()
end

function step(state::Start, transition::Letter)
    state = Identifier()
    push!(state.raw, transition.raw)
    state
end

function step(state::Identifier, transition::Letter)
    push!(state.raw, transition.raw)
    state
end

function step(state::Identifier, transition::Digit)
    push!(state.raw, transition.raw)
    state
end

function step(state::Identifier, ::Transition)
    println("Found Identifier: ", state.raw)
    state = Start()
end

#=
    END STEPS
=#

q0 = Start() # starting state

transition_buffer = [Digit('2'), Digit('3'), Digit('1'), Symbol('+'), 
                        Letter('n'), Letter('u'), Letter('m'), Symbol('+'), 
                        Letter('f'), Letter('o'), Letter('o'), Symbol(';')]

reduce(step, transition_buffer, init=q0)