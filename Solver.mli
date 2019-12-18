module type Game = sig
    type t
    
    type transformation
    
    val compare: t -> t -> int
    
    val moves: t -> t list
    
    val revmoves: t -> t list
    
    val win: t -> int
    (** only returns a positive integer with terminal winning states, a higher value win will be prioratised **)
    
    val inital: t
    
    val prettyPrint: t -> unit
    
(*    val move: int -> t -> t*)
    
    val transformed: t -> t*transformation
    
    val invtransform: transformation -> t -> t
end

module type Solver = sig
    type state
    
    val fill: unit -> unit
    
    val makeMove: state -> state
    
    module StateMap:Map.S with type key=state
    
    val states:(state*int ref)list StateMap.t ref
end

module Make (Game:Game): Solver with type state = Game.t
