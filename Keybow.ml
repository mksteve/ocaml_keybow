external display:(int*int*int) array -> int -> unit = "keybow_display"

(*external key_press: unit -> int = "keybow_keypress"

external init :unit -> unit= "keybow_init"

external terminate : unit -> unit = "keybow_term"*)

let key_press = read_int

(*let () = init ()*)

let (.@()) = List.nth

let display_win state board = (*just display*)
    let image = Array.append (Array.map (function 
        true -> (255,0,0)
      | false -> (0,0,0)) (Array.sub state (board *9) (9+board*9))) [|0,0,0;0,0,0;0,0,0|] in
    for i = 0 to Notaktodef.boards do
        for j = 0 to 7 do
            if List.fold_left (fun a j -> a&&state.(i*9+j)) false Notaktodef.lines.@(j) then
                for k = 0 to 2 do
                    image.(i*9+Notaktodef.lines.@(j).@(k)) <- (0,255,0)
                done
        done
    done;
    (try
        image.(board+9) <- (255,0,0)
    with
        Invalid_argument "index out of bounds" -> ());
    print_int @@Array.length(image);
    (assert (Array.length(image) = 12));
    Array.length(image)
 |> display image

let display_i state = (*display and input*)
    let board = ref 0
    and posMoves = Notaktodef.moves state 
    and input = Array.copy state in
    while not (List.mem input posMoves) do
(*        let image = Array.make (0,0,0) 12 in
        for i=9*board to 8+i*board do
            if state.(i) then
                image.(i) = (255,0,0)
        done;
        display image;*)
        display_win state !board;
        let keypress = key_press () in
        Array.blit state 0 input 0 (Notaktodef.boards*9);
        input.(keypress+ !board*9) <- true;
    done;
    input
