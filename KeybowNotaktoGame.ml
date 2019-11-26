module NS = Solver.Make(Notaktodef)

let () =
    NS.fill();
    let state = ref Notaktodef.inital
    and computer = ref false in
    while Notaktodef.win !state = 0 do
        if !computer then begin
            state := NS.makeMove !state
        end
        else begin
            state := Keybow.display_i !state
        end;
        computer := not !computer
    done;
    Keybow.display_i !state;()
