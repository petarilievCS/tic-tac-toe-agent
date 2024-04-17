#!/usr/bin/python3
#  agent.py
#  Nine-Board Tic-Tac-Toe Agent starter code
#  COMP3411/9814 Artificial Intelligence
#  CSE, UNSW

import socket
import sys
import numpy as np

# a board cell can hold:
#   0 - Empty
#   1 - We played here
#   2 - Opponent played here

# Global variables
# the boards are of size 10 because index 0 isn't used
boards = np.zeros((10, 10), dtype="int8")
s = [".","X","O"]
curr = 0 # this is the current board to play in

#move = np.zeros(82,dtype=np.int32)
best_move = np.zeros(82,dtype=np.int32)

MIN_EVAL = -1000000
MAX_EVAL =  1000000

DEPTH = 6

M = 0

PLAYERS = [1,2] #1 is us (X) and 2 is opponent (O)

# print a row
def print_board_row(bd, a, b, c, i, j, k):
    print(" "+s[bd[a][i]]+" "+s[bd[a][j]]+" "+s[bd[a][k]]+" | " \
             +s[bd[b][i]]+" "+s[bd[b][j]]+" "+s[bd[b][k]]+" | " \
             +s[bd[c][i]]+" "+s[bd[c][j]]+" "+s[bd[c][k]])

# Print the entire board
def print_board(board):
    print_board_row(board, 1,2,3,1,2,3)
    print_board_row(board, 1,2,3,4,5,6)
    print_board_row(board, 1,2,3,7,8,9)
    print(" ------+-------+------")
    print_board_row(board, 4,5,6,1,2,3)
    print_board_row(board, 4,5,6,4,5,6)
    print_board_row(board, 4,5,6,7,8,9)
    print(" ------+-------+------")
    print_board_row(board, 7,8,9,1,2,3)
    print_board_row(board, 7,8,9,4,5,6)
    print_board_row(board, 7,8,9,7,8,9)
    print()



#**********************************************************
#   Negamax formulation of alpha-beta search, like in TTT.py
#
def alphabeta( player, m, alpha, beta, previous_move, depth):

    best_eval = MIN_EVAL

    if game_won( 1-player, boards ):   # LOSS
        
        return -1000 + m  # better to win faster (or lose slower)
    
    if depth == 0:
        return evaluate_board(player, boards)

    this_move = 0
    for r in range( 1, 10):
        if boards[previous_move][r] == 0:         # move is legal
            this_move = r
            boards[previous_move][this_move] = PLAYERS[player] # make move
            this_eval = -alphabeta(1-player,m+1,-beta,-alpha, this_move, depth-1)
            boards[previous_move][this_move] = 0  # undo move
            if this_eval > best_eval:
                best_move[m] = this_move
                best_eval = this_eval
                if best_eval > alpha:
                    alpha = best_eval
                    if alpha >= beta: # cutoff
                        return( alpha )

    if this_move == 0:  # no legal moves
        return( 0 )     # DRAW
    else:
        return( alpha )
    
#**********************************************************
#   Calculate the heuristic value when the board reaches maximum depth
#
def evaluate_board(player, bds):
    heuristic_value = 0
    for i in range(1,10):
        X1, X2, O1, O2 = count_symbols(bds[i])
        if player == 0:
            heuristic_value += (50*X2 + X1 - (50*O2 + O1))
        else:
            heuristic_value += (50*O2 + O1 - (50*X2 + X1))
    return heuristic_value

            
def count_symbols(bd):
    X1, X2, O1, O2 = 0, 0, 0, 0
    #We need to check all three rows, all three columns and the two dioagonals
    potential_lines = [[1,2,3], [4,5,6], [7,8,9], #rows
             [1,4,7], [2,5,8], [3,6,9], #columns
             [1,5,9], [3,5,7]] #diagonals

    for line in potential_lines:
        X_count = sum(bd[i] == 1 for i in line)
        O_count = sum(bd[i] == 2 for i in line)

        if X_count == 1 and O_count == 0:
            X1 += 1
        elif X_count == 2 and O_count == 0:
            X2 += 1
        if O_count == 1 and X_count == 0:
            O1 += 1
        elif O_count == 2 and X_count == 0:
            O2 += 1
        
    return X1, X2, O1, O2

def game_won( p, bd ):
    p = PLAYERS[p]
    for i in range(1,10):
        if(   ( bd[i][1] == p and bd[i][2] == p and bd[i][3] == p )
            or( bd[i][4] == p and bd[i][5] == p and bd[i][6] == p )
            or( bd[i][7] == p and bd[i][8] == p and bd[i][9] == p )
            or( bd[i][1] == p and bd[i][4] == p and bd[i][7] == p )
            or( bd[i][2] == p and bd[i][5] == p and bd[i][8] == p )
            or( bd[i][3] == p and bd[i][6] == p and bd[i][9] == p )
            or( bd[i][1] == p and bd[i][5] == p and bd[i][9] == p )
            or( bd[i][3] == p and bd[i][5] == p and bd[i][7] == p )):
            return True
    return False



# place a move in the global boards
def place( board, num, player ):
    global curr
    curr = num
    boards[board][num] = player


# choose a move to play
def play():
    # print_board(boards)

    # negamax alpha beta to find the move
    alphabeta(0, M, MIN_EVAL, MAX_EVAL, curr, DEPTH)
    n = best_move[M]

    # print("playing", n)
    place(curr, n, 1)
    return n

# read what the server sent us and
# parse only the strings that are necessary
def parse(string):
    if "(" in string:
        command, args = string.split("(")
        args = args.split(")")[0]
        args = args.split(",")
    else:
        command, args = string, []

    # init tells us that a new game is about to begin.
    # start(x) or start(o) tell us whether we will be playing first (x)
    # or second (o); we might be able to ignore start if we internally
    # use 'X' for *our* moves and 'O' for *opponent* moves.

    # second_move(K,L) means that the (randomly generated)
    # first move was into square L of sub-board K,
    # and we are expected to return the second move.
    if command == "second_move":
        # place the first move (randomly generated for opponent)
        place(int(args[0]), int(args[1]), 2)
        return play()  # choose and return the second move

    # third_move(K,L,M) means that the first and second move were
    # in square L of sub-board K, and square M of sub-board L,
    # and we are expected to return the third move.
    elif command == "third_move":
        # place the first move (randomly generated for us)
        place(int(args[0]), int(args[1]), 1)
        # place the second move (chosen by opponent)
        place(curr, int(args[2]), 2)
        return play() # choose and return the third move

    # nex_move(M) means that the previous move was into
    # square M of the designated sub-board,
    # and we are expected to return the next move.
    elif command == "next_move":
        # place the previous move (chosen by opponent)
        place(curr, int(args[0]), 2)
        return play() # choose and return our next move

    elif command == "win":
        print("Yay!! We win!! :)")
        return -1

    elif command == "loss":
        print("We lost :(")
        return -1

    return 0

# connect to socket
def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = int(sys.argv[2]) # Usage: ./agent.py -p (port)

    s.connect(('localhost', port))
    while True:
        text = s.recv(1024).decode()
        if not text:
            continue
        global M
        global DEPTH
        M += 1
        if M == 15:
            DEPTH = DEPTH + 2
        if M == 20:
            DEPTH = DEPTH + 1
        if M == 30:
            DEPTH = DEPTH + 2

        
        for line in text.split("\n"):
            response = parse(line)
            if response == -1:
                s.close()
                return
            elif response > 0:
                s.sendall((str(response) + "\n").encode())
                print_board(boards)

if __name__ == "__main__":
    main()
