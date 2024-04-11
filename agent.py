#!/usr/bin/python3
#  agent.py
#  Nine-Board Tic-Tac-Toe Agent starter code
#  COMP3411/9814 Artificial Intelligence
#  CSE, UNSW

import socket
import sys
import numpy as np

# a board cell can hold:
EMPTY = 0
PLAYER = 1
OPPONENT = 2

MIN_EVAL = -1000000
MAX_EVAL =  1000000

WIN_EVAL = 1000
LOSS_EVAL = -1000
DRAW_EVAL = 0

INITIAL_DEPTH_LIMIT = 6

# the boards are of size 10 because index 0 isn't used
boards = np.zeros((10, 10), dtype="int8")
s = [".","X","O"]
curr = 0 # this is the current board to play in

m = 1
move = np.zeros(82,dtype=np.int32)
best_move = np.zeros(82,dtype=np.int32)
depth_limit = INITIAL_DEPTH_LIMIT

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

def alphabeta(player, m, curr, prev, alpha, beta, best_move, depth):

    opponent = OPPONENT if player == PLAYER else PLAYER

    # Terminal nodes
    # if game_won(player):
    #     return WIN_EVAL
    if game_won(opponent, prev):
        return LOSS_EVAL
    if board_full(curr):
        return DRAW_EVAL
    
    # Max depth reached
    if depth == depth_limit:
        return evaluate_game(player, curr)
    
    # Run alphabeta on each possible move
    this_move = 0
    best_eval = MIN_EVAL
    for r in range(1, 10):
        if boards[curr][r] == EMPTY:
            this_move = r
            boards[curr][r] = player # make move
            this_eval = -alphabeta(opponent, m + 1, r, curr, -beta, -alpha, best_move, depth + 1)
            boards[curr][r] = EMPTY # undo move
            if this_eval > best_eval:
                best_move[m] = this_move
                best_eval = this_eval
                if best_eval > alpha:
                    alpha = best_eval
                    if alpha >= beta:
                        return alpha

    return alpha

# choose a move to play
def play():
    # print_board(boards)

    # just play a random move for now
    n = np.random.randint(1,9)
    while boards[curr][n] != 0:
        n = np.random.randint(1,9)

    # print("playing", n)
    place(curr, n, 1)
    return n

# place a move in the global boards
def place( board, num, player ):
    global curr
    curr = num
    boards[board][num] = player

# read what the server sent us and
# parse only the strings that are necessary
def parse(string):
    global m, move, best_move
    prev = curr

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
    if command == "init." or command == "start" or len(command) == 0:
        return 0

    # second_move(K,L) means that the (randomly generated)
    # first move was into square L of sub-board K,
    # and we are expected to return the second move.
    if command == "second_move":
        # place the first move (randomly generated for opponent)
        place(int(args[0]), int(args[1]), 2)
        move[m] = int(args[1])
        m += 1

    # third_move(K,L,M) means that the first and second move were
    # in square L of sub-board K, and square M of sub-board L,
    # and we are expected to return the third move.
    elif command == "third_move":
        # place the first move (randomly generated for us)
        place(int(args[0]), int(args[1]), 1)
        # place the second move (chosen by opponent)
        place(curr, int(args[2]), 2)

        move[m] = int(args[1])
        m += 1
        move[m] = int(args[2])
        m += 1

    # nex_move(M) means that the previous move was into
    # square M of the designated sub-board,
    # and we are expected to return the next move.
    elif command == "next_move":
        # place the previous move (chosen by opponent)
        place(curr, int(args[0]), 2)

    elif command == "win":
        print("Yay!! We win!! :)")
        return -1

    elif command == "loss":
        print("We lost :(")
        return -1

    alphabeta(PLAYER, m, curr, prev, MIN_EVAL, MAX_EVAL, best_move, 0)
    move[m] = best_move[m]
    m += 1
    place(curr, move[m - 1], 1)
    return move[m - 1]

#**********************************************************
#   Return True if game won by player p on board prev_board
#
def game_won(p, prev_board):
    bd = boards[prev_board]
    if (   ( bd[1] == p and bd[2] == p and bd[3] == p )
        or ( bd[4] == p and bd[5] == p and bd[6] == p )
        or ( bd[7] == p and bd[8] == p and bd[9] == p )
        or ( bd[1] == p and bd[4] == p and bd[7] == p )
        or ( bd[2] == p and bd[5] == p and bd[8] == p )
        or ( bd[3] == p and bd[6] == p and bd[9] == p )
        or ( bd[1] == p and bd[5] == p and bd[9] == p )
        or ( bd[3] == p and bd[5] == p and bd[7] == p )):
            return True
    return False

#**********************************************************
#   Return True if board bd[] is full
#
def board_full(bd):
    board = boards[bd]
    for i in range(1,10):
        if board[i] == EMPTY:
            return False
    return True

#**********************************************************
#   Return heurisitc of board bd[] for player p when asked to make a move on sub-board n
#
def evaluate_game(p, current_board):
    FACTOR_TWO = 50
    FACTOR_CURRENT = 2
    o = OPPONENT if p == PLAYER else PLAYER # opponent of p
    result = 0

    p1 = 0 # number of lines with 1 player piece
    p2 = 0 # number of lines with 2 player pieces
    o1 = 0 # number of lines with 1 opponent piece
    o2 = 0 # number of lines with 2 opponent pieces

    for board in range(1, 10):
        p1Rows, p2Rows, o1Rows, o2Rows = check_rows(p, o, board)
        p1Columns, p2Columns, o1Columns, o2Columns = check_columns(p, o, board)
        p1Diagonals, p2Diagonals, o1Diagonals, o2Diagonals = check_diagonals(p, o, board)

        # Weight current board more
        if board == current_board:
            p1Rows *= FACTOR_CURRENT
            p2Rows *= FACTOR_CURRENT
            o1Rows *= FACTOR_CURRENT
            o2Rows *= FACTOR_CURRENT

            p1Columns *= FACTOR_CURRENT
            p2Columns *= FACTOR_CURRENT
            o1Columns *= FACTOR_CURRENT
            o2Columns *= FACTOR_CURRENT

            p1Diagonals *= FACTOR_CURRENT
            p2Diagonals *= FACTOR_CURRENT
            o1Diagonals *= FACTOR_CURRENT
            o2Diagonals *= FACTOR_CURRENT

        p1 += p1Rows + p1Columns + p1Diagonals
        p2 += p2Rows + p2Columns + p2Diagonals
        o1 += o1Rows + o1Columns + o1Diagonals
        o2 += o2Rows + o2Columns + o2Diagonals

    result = p1 + FACTOR_TWO * p2 - o1 - FACTOR_TWO * o2 # Forumla used to evaluate current state
         
    return result 

#**********************************************************
#   Return number of rows with 1 player piece, 2 player pieces, 1 opponent piece, 2 opponent pieces
#
def check_rows(p, o, bd):
    p1, p2, o1, o2 = 0, 0, 0, 0
    for r in range(0,3):
        pCount, oCount = 0, 0
        for c in range(1,4):
            if boards[bd][r*3 + c] == p:
                pCount += 1
            elif boards[bd][r*3 + c] == o:
                oCount += 1

        if pCount == 1 and oCount == 0:
            p1 += 1
        elif pCount == 2 and oCount == 0:
            p2 += 1
        
        if oCount == 1 and pCount == 0:
            o1 += 1
        elif oCount == 2 and pCount == 0:
            o2 += 1
    return p1, p2, o1, o2

#**********************************************************
#   Return number of columns with 1 player piece, 2 player pieces, 1 opponent piece, 2 opponent pieces
#
def check_columns(p, o, bd):
    p1, p2, o1, o2 = 0, 0, 0, 0
    for c in range(1,4):
        pCount, oCount = 0, 0
        for r in range(0,3):
            if boards[bd][r*3 + c] == p:
                pCount += 1
            elif boards[bd][r*3 + c] == o:
                oCount += 1
        
        if pCount == 1 and oCount == 0:
            p1 += 1
        elif pCount == 2 and oCount == 0:
            p2 += 1
        if oCount == 1 and pCount == 0:
            o1 += 1
        elif oCount == 2 and pCount == 0:
            o2 += 1
    return p1, p2, o1, o2

#**********************************************************
#   Return number of diagonals with 1 player piece, 2 player pieces, 1 opponent piece, 2 opponent pieces
#
def check_diagonals(p, o, bd):
    p1, p2, o1, o2 = 0, 0, 0, 0
    pCount, oCount = 0, 0
    for i in range(0,3):
        if boards[bd][i*4 + 1] == p:
            pCount += 1
        elif boards[bd][i*4 + 1] == o:
            oCount += 1
    if pCount == 1 and oCount == 0:
        p1 += 1
    elif pCount == 2 and oCount == 0:
        p2 += 1
    if oCount == 1 and pCount == 0:
        o1 += 1
    elif oCount == 2 and pCount == 0:
        o2 += 1

    pCount, oCount = 0, 0
    for i in range(3,8,2):
        if boards[bd][i] == p:
            pCount += 1
        elif boards[bd][i] == o:
            oCount += 1

    if pCount == 1 and oCount == 0:
        p1 += 1
    elif pCount == 2 and oCount == 0:
        p2 += 1
    if oCount == 1 and pCount == 0:
        o1 += 1
    elif oCount == 2 and pCount == 0:
        o2 += 1
    return p1, p2, o1, o2

# connect to socket
def main():
    global depth_limit
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = int(sys.argv[2]) # Usage: ./agent.py -p (port)

    s.connect(('localhost', port))
    while True:
        text = s.recv(1024).decode()

        # Increase depth limit towards the end of the game
        if m == 15 or m == 30:
            depth_limit += 1

        if not text:
            continue
        for line in text.split("\n"):
            response = parse(line)
            if response == 0: 
                continue
            elif response == -1:
                s.close()
                return
            elif response > 0:
                s.sendall((str(response) + "\n").encode())

if __name__ == "__main__":
    main()
