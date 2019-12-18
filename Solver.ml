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
    
    val transformed: t -> (t*transformation)
    
    val invtransform: transformation -> t -> t
end

module type Solver = sig
    type state
    
    val fill: unit -> unit
    
    val makeMove: state -> state
    
    module StateMap:Map.S with type key=state
    
    val states:(state*int ref)list StateMap.t ref
end

module Make(Game:Game):Solver with type state = Game.t = struct
    module StateMap = Map.Make(Game)
    module StateSet = Set.Make(Game)
    type state = Game.t
    
    let rec mapToSetSeq (mapSeq:(state*((state*int ref) list)) Seq.t) () =
        match mapSeq() with
            Seq.Cons ((key,value),next) -> Seq.Cons (key,mapToSetSeq next)
          | Seq.Nil -> Nil
    
    let states = ref StateMap.empty
    
    let notDone = ref StateSet.(empty |> add Game.inital)
    
    let complete state =
        let moves = StateMap.find state !states
        and zeros = ref 0 in
        List.filter (fun (_,a) -> !a = 0) moves
        |> List.iter (fun _ -> incr zeros);
        !zeros = 0
    
    let addState state =
        let moves :(state*int ref)list ref = ref [] in
        List.iteri (fun move tstate ->
            let outcome = Game.win tstate in
            moves := (tstate,ref outcome)::!moves;
            if Game.win tstate = 0 then
                notDone := StateSet.add tstate !notDone)
            (Game.moves state);
        states := StateMap.add state !moves !states
    
    let maximise state =
        let (_,init)::moves = StateMap.find state !states in
        let outcome = List.fold_left (fun a (_,b)-> max a !b) (!init) moves in
        try List.iter (fun move ->
            let pState = StateMap.find move !states in
            (List.assoc state pState) := -outcome) (Game.revmoves state)
        with
            Not_found -> List.iter Game.prettyPrint (Game.revmoves state)
    
    let fill() = 
        while not (StateSet.is_empty !notDone) do
            let prev = !notDone in
            notDone:=StateSet.empty;
            StateSet.iter addState prev
        done;
        notDone := StateMap.to_seq !states |> mapToSetSeq |> StateSet.of_seq;
        let current = ref (StateSet.find_first_opt complete !notDone) in
        while !current <> None do
            match !current with None -> () |Some c ->
            maximise c;
            notDone := StateSet.remove c !notDone;
            current := StateSet.find_first_opt complete !notDone
        done
    
    let makeMove state =
        let state, transformation = Game.transformed state in
        let init::moves = StateMap.find state !states in
        List.fold_left (fun a b -> if snd a > snd b then a else b) init moves |> fst |> Game.invtransform transformation
end
