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

store = Array{Pair{DataType, String}, 1}([])

separators = Set{Char}([';','(',')','#',','])

keywords = Set{String}(["integer", "if", "else", "endif", 
             "while", "return", "get", "put", 
             "boolean", "real", "function",
             "true", "false"])

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

mutable struct Real <: State
    praw::Array{Char}
    araw::Array{Char}
    raw::Array{Char}
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

struct IntegerIntermezzo <: State
    raw::Array{Char}
    IntegerIntermezzo(x) = new(x)
end

struct Comment <: State
    raw::Array{Char}
    Comment() = new([])
end

struct Separator <: State
    raw::Array{Char}
    Separator() = new([])
end

struct Keyword <: State
    raw::Array{Char}
    Keyword(r) = new(r)
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

# Origin: Start
function step(state::Start, ::Whitespace) state end # whitespace at start is fine

function step(state::Start, transition::Letter)
    if transition == '_' return state end
    state = Identifier()
    push!(state.raw, transition.raw)
    state
end

function step(state::Start, transition::Digit)
    state = Integer()
    push!(state.raw, transition.raw)
    state
end

function step(state::Start, transition::Symbol)
    if transition.raw in separators
        sep_state = Separator()
        push!(sep_state.raw, transition.raw)
        save(store, sep_state)
    else
        state = Operator()
        push!(state.raw, transition.raw)
    end
    state
end

# End Origin: Start

# Origin: Identifier
function step(state::Identifier, transition::Letter)
    push!(state.raw, transition.raw)
    state
end

function step(state::Identifier, transition::Digit)
    push!(state.raw, transition.raw)
    state
end

function step(state::Identifier, transition::Symbol)
    save(store, state)
    #state = Operator()
    state = Start()
    step(state, transition)
    #push!(state.raw, transition.raw)
end

function step(state::Identifier, transition::Whitespace)
    save(store, state)
    state = Start()
end

# End Origin: Identifier

# Origin: Integer
function step(state::Integer, transition::Digit)
    push!(state.raw, transition.raw)
    state
end

function step(state::Integer, transition::Symbol)
    if transition.raw != '.'
        save(store, state)
        state = Start()
        return step(state, transition)
    end
    state = IntegerIntermezzo(state.raw)
end

function step(state::Integer, transition::Whitespace)
    save(store, state)
    state = Start()
    state
end

# End Origin: Integer

# Origin: IntegerIntermezzo
function step(state::IntegerIntermezzo, transition::Digit)
    state = Real(state.raw) # praw
    push!(state.araw, transition.raw)
    state
end

step(state::IntegerIntermezzo, ::Whitespace) = throw(InvalidState)

# End Origin: IntegerIntermezzo

# Origin: Real
function step(state::Real, transition::Digit)
    push!(state.araw, transition.raw)
    state
end

function step(state::Real, transition::Whitespace)
    state.raw = cat(state.praw, '.', state.araw, dims=1)
    save(store, state)
    state = Start()
end

function step(state::Real, transition::Symbol)
    state.raw = cat(state.praw, '.', state.araw, dims=1)
    save(store, state)
    state = Start()
    return step(state, transition)
end

# End Origin: Real

# Origin: Operator
function step(state::Operator, transition::Symbol)
    # change to Comment state if start of comment
    if length(state.raw) > 0 && String(cat(state.raw[end], transition.raw, dims=1)) == "/*"
        w = length(state.raw) - 1
        if w > 0
            preop_state = Operator()
            for c in state.raw[1:end-1] 
                push!(preop_state.raw, c)
            end
            save(store, preop_state)
        end
        state = Comment()
        return state
    end
    # check for separators
    if transition.raw in separators
        # save current operator state for transition to separator state
        save(store, state)
        sep_state = Separator()
        push!(sep_state.raw, transition.raw)
        save(store, sep_state)
        state = Start()
        return state
    end
    push!(state.raw, transition.raw)
    state
end

function step(state::Operator, transition::Letter)
    save(store, state)
    state = Identifier()
    push!(state.raw, transition.raw)
    state
end

function step(state::Operator, transition::Digit)
    save(store, state)
    state = Integer()
    push!(state.raw, transition.raw)
    state
end

function step(state::Operator, transition::Whitespace)
    save(store, state)
    state = Start()
end

# End Origin: Operator

# Origin: Comment
# if in comment state, no transition matters except exit comment state operator
function step(state::Comment, transition::Symbol)
    push!(state.raw, transition.raw)
    if length(state.raw) > 1 && String(state.raw[end-1:end]) == "*/"
        state = Start()
    end
    state
end

function step(state::Comment, ::Transition)
    state
end
# End Origin: Comment

#=
    END STEPS
=#

function save(store, state::Any)
    lex_buf = String(state.raw) # convert to String
    push!(store, typeof(state)=>lex_buf)
end

function save(store, state::Identifier)
    lex_buf = String(state.raw) # convert to String
    if lex_buf in keywords
        state = Keyword(state.raw)
    end
    push!(store, typeof(state)=>lex_buf)
end

function display()
    Base.show(stdout, "text/plain", store)
end

function empty!()
    Base.empty!(store)
end