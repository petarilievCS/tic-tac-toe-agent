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

#define MAX_MOVE 81
#define MAX_EVAL     1000000
#define MIN_EVAL    -1000000


int board[10][10];
int move[MAX_MOVE+1];
int best_move[MAX_MOVE+1];
int player;
int m;
int DEPTH = 10;

// We are player 0 (X) and the other player is 1 (O)

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
  do {
    alphabeta(player, m, board, prev_move, MIN_EVAL, MAX_EVAL, best_move, DEPTH);
    this_move = best_move[m];
  } while( board[prev_move][this_move] != EMPTY );
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
  do {
    alphabeta(player, m, board, prev_move, MIN_EVAL, MAX_EVAL, best_move, DEPTH);
    this_move = best_move[m];
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  if (m == 15){
    DEPTH = 12;
  }
  if (m == 25){
    DEPTH = 13;
  }
  if (m == 29){
    DEPTH = 16;
  }
  if (m == 33){
    DEPTH = 17;
  }
  printf("m = %d\n", m);
  printf("DEPTH = %d\n", DEPTH);
  int this_move;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  m++;
  do {
    alphabeta(player, m, board, prev_move, MIN_EVAL, MAX_EVAL, best_move, DEPTH);
    this_move = best_move[m];
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
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

int alphabeta(
                int player,
                int m,
                int board[10][10],
                int previous_move,
                int alpha,
                int beta,
                int best_move[82],
                int depth
               )
{
  int this_eval;
  int best_eval = MIN_EVAL;
  int this_move;
  int r;
  int i;
  for (i = 1; i <= 9; i++){
    if( gamewon( 1-player, board[i] )) { // LOSS
      return -1000 + m; // better to win faster (or lose slower)
    }
  }

  if (depth == 0){
    return evaluate_board(player, board);
  }

  this_move = 0;
  for( r = 1; r <= 9; r++ ) {
    if( board[previous_move][r] == EMPTY ) {        // move is legal
      this_move = r;
      board[previous_move][this_move] = player;     // make move
      this_eval = -alphabeta(1-player,m+1,board,this_move, -beta,-alpha, best_move, depth-1);
      board[previous_move][this_move] = EMPTY;      // undo move
      if( this_eval > best_eval ) {
        best_move[m] = this_move;
        best_eval = this_eval;
        if( best_eval > alpha ) {
          alpha = best_eval;
          if( alpha >= beta ) {      // cutoff
            return( alpha );
          }
        }
      }
    }
  }
  if( this_move == 0 ) { // no legal moves
    return( 0 );         // DRAW
  }
  else {
    return( alpha );
  }
}

int evaluate_board(int player, int bds[10][10]) {
    int heuristic_value = 0;
    int X1, X2, O1, O2;
    
    for (int i = 1; i < 10; i++) {
        count_symbols(bds[i], &X1, &X2, &O1, &O2);
        if (player == 0) {
            heuristic_value += (50 * X2 + X1 - (50 * O2 + O1));
        } else {
            heuristic_value += (50 * O2 + O1 - (50 * X2 + X1));
        }
    }
    return heuristic_value;
}

void count_symbols(int bd[10], int *X1, int *X2, int *O1, int *O2) {
    int potential_lines[8][3] = {{1,2,3}, {4,5,6}, {7,8,9}, {1,4,7}, {2,5,8}, {3,6,9}, {1,5,9}, {3,5,7}};
    *X1 = *X2 = *O1 = *O2 = 0;
    
    for (int i = 0; i < 8; i++) {
        int X_count = 0, O_count = 0;
        for (int j = 0; j < 3; j++) {
            if (bd[potential_lines[i][j]] == 0) {
                X_count++;
            } else if (bd[potential_lines[i][j]] == 1) {
                O_count++;
            }
        }
        if (X_count == 1 && O_count == 0) {
            (*X1)++;
        } else if (X_count == 2 && O_count == 0) {
            (*X2)++;
        }
        if (O_count == 1 && X_count == 0) {
            (*O1)++;
        } else if (O_count == 2 && X_count == 0) {
            (*O2)++;
        }
    }
}
