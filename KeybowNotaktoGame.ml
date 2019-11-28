module NS = Solver.Make(Notaktodef)

let () =
    NS.fill();
    let state = ref Notaktodef.inital
    and computer = ref false in
    while Notaktodef.win !state = 0 do
        if !computer then begin
            print_endline "computer";
            state := NS.makeMove !state
        end
        else begin
            print_endline "human";
            state := Keybow.display_i !state
        end;
        computer := not !computer;
        Notaktodef.prettyPrint !state
    done;
    Keybow.display_win !state 0;
    Keybow.terminate()
