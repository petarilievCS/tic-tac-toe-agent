/*********************************************************
*   ttt.c
*
*   UNSW CSE
*   COMP3411/9814
*   Code for Tic-Tac-Toe with Alpha-Beta search
*/
#include <stdio.h>

#define FALSE          0
#define TRUE           1

#define EMPTY          2

#define ILLEGAL_MOVE   0
#define STILL_PLAYING  1
#define WIN            2
#define LOSS           3
#define DRAW           4

#define MAX_MOVE       9

#define MIN_EVAL -1000000
#define MAX_EVAL  1000000

typedef int Value;

void reset_board( int board[10] );
void print_board( int board[10] );
Value  alphabeta( int player, int m, int board[10],
                  Value alpha, Value beta, int best_move[MAX_MOVE+1] );
int    make_move( int player, int m, int move[MAX_MOVE+1], int board[10] );
int   full_board( int board[10] );
int     game_won( int player, int board[10] );

int main( void )
{
  int board[10];
  int move[MAX_MOVE+1]={0};
  int best_move[MAX_MOVE+1]={0};
  int is_human[2]={TRUE,FALSE};
  int game_status = STILL_PLAYING;
  int player=1;
  int m=0;
  
  reset_board( board );

  while( m < MAX_MOVE && game_status == STILL_PLAYING ) {
    m++;
    player = 1-player;
    if( is_human[player] ) {
      print_board( board );
      do {
	printf("Enter Move [1-9]: ");
	scanf("%d",&move[m]);
      } while( move[m] < 1 || move[m] > 9 || board[move[m]] != EMPTY );
    }
    else {
      alphabeta( player,m,board,MIN_EVAL,MAX_EVAL,best_move );
      move[m] = best_move[m];
    }
    game_status = make_move( player,m,move,board );
  }

  print_board( board );

  return 0;
}

void reset_board( int board[10] )
{
  int d;
  for( d = 1; d <= 9; d++ ) {
    board[d] = EMPTY;
  }
}

/*********************************************************
*    Print the board
*/
void print_board( int bd[10] )
{
  const char *sb = "XO.";
  printf("| %c %c %c |\n",sb[bd[1]],sb[bd[2]],sb[bd[3]]);
  printf("| %c %c %c |\n",sb[bd[4]],sb[bd[5]],sb[bd[6]]);
  printf("| %c %c %c |\n",sb[bd[7]],sb[bd[8]],sb[bd[9]]);

  fflush(stdout);
}

/*********************************************************
   Negamax formulation of alpha-beta search
*/
Value alphabeta(
                int player,
                int m,
                int board[10],
                Value alpha,
                Value beta,
                int best_move[10]
               )
{
  Value this_eval;
  Value best_eval = MIN_EVAL;
  int this_move;
  int r;

  if( game_won( 1-player, board )) { // LOSS
    return -1000 + m; // better to win faster (or lose slower)
  }
  this_move = 0;
  for( r = 1; r <= 9; r++ ) {
    if( board[r] == EMPTY ) {        // move is legal
      this_move = r;
      board[this_move] = player;     // make move
      this_eval = -alphabeta(1-player,m+1,board,-beta,-alpha,best_move);
      board[this_move] = EMPTY;      // undo move
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

/*********************************************************
   Make specified move on the board and return game status
*/
int make_move(
              int player,
              int m,
              int move[MAX_MOVE+1],
              int board[10]
             )
{
  if( board[move[m]] != EMPTY ) {
    printf("Illegal Move.\n");
    return( ILLEGAL_MOVE );
  }
  else {
    board[move[m]] = player;

    if( game_won( player, board )) {
      return( WIN );
    }
    else if( full_board( board )) {
      return( DRAW );
    }
    else {
      return STILL_PLAYING;
    }
  }
}

/*********************************************************
   Return TRUE if the board is full
*/
int full_board( int board[10] )
{
  int b=1;
  while( b <= 9 && board[b] != EMPTY ) {
    b++;
  }
  return( b == 10 );
}

/*********************************************************
   Return TRUE if game won by player p on board bd[]
*/
int game_won( int p, int bd[10] )
{
  return(  ( bd[1] == p && bd[2] == p && bd[3] == p )
         ||( bd[4] == p && bd[5] == p && bd[6] == p )
         ||( bd[7] == p && bd[8] == p && bd[9] == p )
         ||( bd[1] == p && bd[4] == p && bd[7] == p )
         ||( bd[2] == p && bd[5] == p && bd[8] == p )
         ||( bd[3] == p && bd[6] == p && bd[9] == p )
         ||( bd[1] == p && bd[5] == p && bd[9] == p )
         ||( bd[3] == p && bd[5] == p && bd[7] == p ));
}