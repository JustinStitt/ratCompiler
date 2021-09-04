#=
    Deterministic Finite State Machine 
    uses Julia's multiple dispatch coupled with abstract types 
    to define states and transitions together alongisde a "step"
    function. This step function determines the best (most specific)
    types for the situation (current state, transition).
=#

abstract type State end # We will have various states all of type State
abstract type Transition end # ditto

InvalidState = ArgumentError # Define ad-hoc error piggybacking ArgumentError

#=
    STATES
=#
struct Start <: State # start inherits from State (is of type State)
    Start() = new()
end

struct Integer <: State
    raw::Array{Char}
    Integer() = new([])
end

struct Real <: State
    praw::Array{Char}
    araw::Array{Char}
    Real(x) = new(x, [])
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
    state = Integer()
    push!(state.raw, transition.raw)
    state
end

function step(state::Integer, transition::Digit)
    push!(state.raw, transition.raw) # cant push to string not mutable
    state
end

function step(state::Integer, transition::Transition)
    if transition.raw == '.' # formation of a Real
        state = Real(state.raw)
        return state
    end
    println("Found Integer: ", state.raw)
    state = Start()
end

function step(state::Start, transition::Letter)
    if transition.raw == '_' return state end
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

function step(state::Real, transition::Digit)
    push!(state.araw, transition.raw)
    state
end

function step(state::Real, ::Transition)
    println("Found Real: ", state.praw, state.araw)
    state = Start()
end

function step(state::Start, transition::Symbol)
    state = Operator()
    push!(state.raw, transition.raw)
    state
end

function step(state::Operator, transition::Operator)
    push!(state.raw, transition.raw)
    state
end

function step(state::Opeartor, ::Transition)
    println("Found Operator: ", state.raw)
    state = Start()
end

#=
    END STEPS
=#

