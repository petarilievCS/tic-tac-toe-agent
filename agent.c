/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define FALSE          0
#define TRUE           1
#define WIN            2
#define LOSS           3
#define DRAW           4

#define MIN_EVAL -1000000
#define MAX_EVAL  1000000

// Parameters
#define FACTOR_TWO 50
#define FACTOR_CURRENT 2
#define INITIAL_DEPTH_LIMIT 9
#define LOSS_EVAL -10000
#define WIN_EVAL 10000

int board[10][10];
int move[MAX_MOVE+1];
int best_move[MAX_MOVE+1];
int player;
int m;

/*********************************************************
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************
   Parse command-line arguments
*/
void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

/*********************************************************
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
  reset_board( board );
  m = 0;
  move[m] = 0;
  player = this_player;
}

/*********************************************************
   Choose second move and return it
*/
int agent_second_move( int board_num, int prev_move )
{
  int this_move;
  move[0] = board_num;
  move[1] = prev_move;
  board[board_num][prev_move] = !player;
  m = 2;
  alphabeta(player, m, prev_move, 0, MIN_EVAL, MAX_EVAL, 0);
  this_move = best_move[m];
  move[m] = this_move;
  board[prev_move][this_move] = player;
  return( this_move );
}

/*********************************************************
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
  int this_move;
  move[0] = board_num;
  move[1] = first_move;
  move[2] = prev_move;
  board[board_num][first_move] =  player;
  board[first_move][prev_move] = !player;
  m=3;
  alphabeta(player, m, first_move, prev_move, MIN_EVAL, MAX_EVAL, 0);
  this_move = best_move[m];
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  int this_move;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  m++;
  alphabeta(player, m, move[m - 1], move[m - 2], MIN_EVAL, MAX_EVAL, 0);
  this_move = best_move[m];
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
}

/*********************************************************
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
  // nothing to do here
}

/*********************************************************
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
}

/*********************************************************
   Heuristic function - returns a value for the board
*/
int evaluate_game(int player, int current_board) {

  int result = 0;
  int p1 = 0; // number of 1 in a row
  int p2 = 0; // number of 2 in a row
  int o1 = 0; // number of 1 in a row
  int o2 = 0; // number of 2 in a row

  for (int subboard = 1; subboard < 10; subboard++) {

    CountValues rowCounts = count_rows(subboard, player);
    CountValues columnCounts = count_columns(subboard, player);
    CountValues diagonalCounts = count_diagonals(subboard, player);
    
    int p1Rows = rowCounts.p1;
    int p2Rows = rowCounts.p2;
    int o1Rows = rowCounts.o1;
    int o2Rows = rowCounts.o2;

    int p1Columns = columnCounts.p1;
    int p2Columns = columnCounts.p2;
    int o1Columns = columnCounts.o1;
    int o2Columns = columnCounts.o2;

    int p1Diagonals = diagonalCounts.p1;
    int p2Diagonals = diagonalCounts.p2;
    int o1Diagonals = diagonalCounts.o1;
    int o2Diagonals = diagonalCounts.o2;

    // Increase weight of current_board
    if (subboard == current_board) {
      p1Rows *= FACTOR_CURRENT;
      p2Rows *= FACTOR_CURRENT;
      o1Rows *= FACTOR_CURRENT;
      o2Rows *= FACTOR_CURRENT;

      p1Columns *= FACTOR_CURRENT;
      p2Columns *= FACTOR_CURRENT;
      o1Columns *= FACTOR_CURRENT;
      o2Columns *= FACTOR_CURRENT;

      p1Diagonals *= FACTOR_CURRENT;
      p2Diagonals *= FACTOR_CURRENT;
      o1Diagonals *= FACTOR_CURRENT;
      o2Diagonals *= FACTOR_CURRENT;
    }

    p1 += p1Rows + p1Columns + p1Diagonals;
    p2 += p2Rows + p2Columns + p2Diagonals;
    o1 += o1Rows + o1Columns + o1Diagonals;
    o2 += o2Rows + o2Columns + o2Diagonals;
    
  }
  
  result = p1 + p2 * FACTOR_TWO - o1 - o2 * FACTOR_TWO;
  return result;

}

/*********************************************************
   Return the number of rows with 1 player piece, 2 player pieces, 1 opponent piece, and 2 opponent pieces
*/
CountValues count_rows(int bd, int player) {
  int p1 = 0; // number of 1 in a row
  int p2 = 0; // number of 2 in a row
  int o1 = 0; // number of 1 in a row
  int o2 = 0; // number of 2 in a row
  
  for (int r = 0; r < 3; r++) {
    int pCount = 0;
    int oCount = 0;

    // Check row
    for (int c = 1; c < 4; c++) {
      int piece = board[bd][r * 3 + c];
      if (piece == player) {
        pCount++;
      } else if (piece == !player) {
        oCount++;
      }
    }

    // Update counts
    if (pCount == 1 && oCount == 0) {
      p1++;
    } else if (pCount == 2 && oCount == 0) {
      p2++;
    } else if (pCount == 0 && oCount == 1) {
      o1++;
    } else if (pCount == 0 && oCount == 2) {
      o2++;
    }
  }
  
  CountValues result = {p1, p2, o1, o2};
  return result;
}


/*********************************************************
   Return the number of columns with 1 player piece, 2 player pieces, 1 opponent piece, and 2 opponent pieces
*/
CountValues count_columns(int bd, int player) {
  int p1 = 0; // number of 1 in a row
  int p2 = 0; // number of 2 in a row
  int o1 = 0; // number of 1 in a row
  int o2 = 0; // number of 2 in a row
  
  for (int c = 1; c < 4; c++) {
    int pCount = 0;
    int oCount = 0;

    // Check column
    for (int r = 0; r < 3; r++) {
      int piece = board[bd][r * 3 + c];
      if (piece == player) {
        pCount++;
      } else if (piece == !player) {
        oCount++;
      }
    }

    // Update counts
    if (pCount == 1 && oCount == 0) {
      p1++;
    } else if (pCount == 2 && oCount == 0) {
      p2++;
    } else if (pCount == 0 && oCount == 1) {
      o1++;
    } else if (pCount == 0 && oCount == 2) {
      o2++;
    }
  }
  
  CountValues result = {p1, p2, o1, o2};
  return result;
}

/*********************************************************
   Return the number of diagonals with 1 player piece, 2 player pieces, 1 opponent piece, and 2 opponent pieces
*/
CountValues count_diagonals(int bd, int player) {
  int p1 = 0; // number of 1 in a row
  int p2 = 0; // number of 2 in a row
  int o1 = 0; // number of 1 in a row
  int o2 = 0; // number of 2 in a row
  
  int pCount = 0;
  int oCount = 0;

  // Check diagonal 1
  for (int i = 0; i < 3; i++) {
    int piece = board[bd][i * 4 + 1];
    if (piece == player) {
      pCount++;
    } else if (piece == !player) {
      oCount++;
    }
  }

  // Update counts
  if (pCount == 1 && oCount == 0) {
    p1++;
  } else if (pCount == 2 && oCount == 0) {
    p2++;
  } else if (pCount == 0 && oCount == 1) {
    o1++;
  } else if (pCount == 0 && oCount == 2) {
    o2++;
  }

  pCount = 0;
  oCount = 0;

  // Check diagonal 2
  for (int i = 3; i < 8; i += 2) {
    int piece = board[bd][i];
    if (piece == player) {
      pCount++;
    } else if (piece == !player) {
      oCount++;
    }
  }

  // Update counts
  if (pCount == 1 && oCount == 0) {
    p1++;
  } else if (pCount == 2 && oCount == 0) {
    p2++;
  } else if (pCount == 0 && oCount == 1) {
    o1++;
  } else if (pCount == 0 && oCount == 2) {
    o2++;
  }
  
  CountValues result = {p1, p2, o1, o2};
  return result;
}

/*********************************************************
   Minimax algorithm with alpha-beta pruning
*/
int alphabeta(int p, int m, int curr, int prev, int alpha, int beta, int depth) {
  // Loss
  if (gamewon(!p, board[prev])) {
    return LOSS_EVAL;
  }

  // Win
  // if (gamewon(player, board[prev])) {
  //   return WIN_EVAL;
  // }

  // Draw
  if (full_board(board[curr])) {
    return 0;
  }

  // Max depth reached
  if (depth == INITIAL_DEPTH_LIMIT) {
    int res = evaluate_game(p, curr);
    return res;
  }

  int this_eval;
  int best_eval = MIN_EVAL;

  for (int i = 1; i < 10; i++) {
    if (board[curr][i] == EMPTY) {
      board[curr][i] = p;
      this_eval = -alphabeta(!p, m + 1, i, curr, -beta, -alpha, depth + 1);
      board[curr][i] = EMPTY;

      if (this_eval > best_eval) {
        best_eval = this_eval;
        best_move[m] = i;
        if (best_eval > alpha) {
          alpha = best_eval;
          if (alpha >= beta) {
            return alpha;
          }
        }
      }
    }
  }
  return alpha;
}