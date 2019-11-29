type t = bool array

type transformation = int

let boards=1

let fact = let rec fact x = if x = 0 then 1 else (fact (x-1)) *x in fact boards

let compare = compare

let lines=[[0;1;2];[3;4;5];[6;7;8];[0;4;8];[2;4;6];[0;3;6];[1;4;7];[2;5;8] ]

let transformationsxy = [|
    [|0;1;2;3;4;5;6;7;8|];
    [|2;1;0;5;4;3;8;7;6|];
    [|6;7;8;3;4;5;0;1;2|];
    [|8;7;6;5;4;3;2;1;0|]|]
let transformationsrot = [|
    [|0;1;2;3;4;5;6;7;8|];
    [|0;3;6;1;4;7;2;5;8|];
    [|6;3;0;7;4;1;8;5;2|]
    |]

let inital = Array.make (9*boards) false

(*let lines = 
    let lines = ref [] in
    for i=0 to boards-1 do
        lines := (List.map (List.map (fun x->x+(i*9))) oneBoardLines) @ !lines
    done;
    !lines*)

let prettyPrint state =
    for i=0 to boards -1 do
        for j=i*3 to i*3+2 do
            if j<>0 then print_endline "-+-+-";
            for k=j*3 to j*3+2 do
                if k<>j*3 then print_string "|";
                match state.(k) with
                    false -> print_int k
                  | true -> print_string "X"
            done;
            print_newline ()
        done
    done

let rec checkl state board = function
    [] -> true
  | hd::tl -> state.(hd+board)&&checkl state board tl
and checkb state board = function
    [] -> false
  | hd::tl -> checkl state board hd || checkb state board tl
and check state boards =
    if boards = 0 then true else
    (check state (boards-1)) && (checkb state ((boards-1)*9) lines)

let moves state =
    List.init (boards*9) Fun.id
    |> List.filter_map (fun move ->
        match state.(move)||checkb state ((move / 9)*9) lines with
            true -> None
          | false -> let nstate = Array.copy state in
                nstate.(move) <- true;Some nstate)

let permutateBoardReflections (state,pNumber) board number :(t*int) =
    let tempBoard = Array.make 9 false in
    for i=0 to 8 do 
        tempBoard.(transformationsxy.(number mod 4).(i)) <- state.(i+(boards-1)*9)
    done;
    let newBoard = Array.copy state in
    for i=0 to 8 do
        newBoard.(transformationsrot.(number / 4).(i)+boards*9) <- tempBoard.(i)
    done;
    (newBoard,pNumber*12+number)

let permutateBoards state permutation:(t*int) list=
    let newBoard = Array.copy state
    and tempBoard = Array.make 9 false
    and permutation = ref permutation in
    if boards > 1 then begin
        for i=boards downto 2 do
            let pos = !permutation mod i in
            permutation := !permutation / i;
            Array.blit newBoard (pos * 9) tempBoard 0 9;
            Array.blit newBoard (i * 9) newBoard (pos*9) 9;
            Array.blit tempBoard 0 newBoard (i*9) 9
        done
    end;
    let permutateBoard board state=
        permutateBoardReflections state board
      |>List.init 12 in
    let boardPermutations:(t*int)list ref=ref [newBoard,0] in
    for i=0 to boards-1 do
        boardPermutations := List.concat (List.map (permutateBoard i) !boardPermutations)
    done;
    !boardPermutations

let transformed state=
    List.init fact (permutateBoards state)
    |> List.concat
    |> List.fast_sort (fun a b -> compare (snd a) (snd b))
    |> List.hd
let invtransform state =
    ()

let revmoves state =
    let moves = ref [] in
    Array.iteri (fun move cell -> if cell then begin
        let pState = Array.copy state in
        pState.(move) <- false;
        if not (check pState boards) then
        moves := pState :: !moves end) state;
    !moves

let win state =
    match check state boards with
        true -> -1
      | false -> 0
