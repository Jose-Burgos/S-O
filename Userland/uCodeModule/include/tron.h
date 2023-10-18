#ifndef _TRON_H_
#define _TRON_H_

#include <color.h>
#include <lib.h>

#define MAX_WIDTH 1024
#define MAX_HEIGHT 768

#define	UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define LEFT_KEY 1
#define A_KEY 'a'
#define RIGHT_KEY 2
#define D_KEY 'd'
#define UP_KEY 3
#define W_KEY 'w'
#define DOWN_KEY 4
#define S_KEY 's'

typedef struct {
 /**
  * 	Player coordinates
  */ 
 int x, y;

 /**
  * 	Player direction
  */ 
 int dir;

 Color color;

} Player;

typedef struct {
 /**
  * 	Canvas dimensions
  */ 
 int width, height;

 /**
  * 	Logic board
  */ 
 char board[MAX_WIDTH][MAX_HEIGHT];

} Canvas;

/**
   * @brief Function that clears screen and begins the game. If the user presses 'q' the game is exited.
*/
int mainTron();

/**
   * @brief Function that initializes the canvas, both players and displays them on screen before the game actually starts.
   *        It also shows a countdown.
*/
void startGame();

/**
   * @brief Function that shows each players scores.
*/
void drawScores();

/**
   * @brief Function that verifies game progress and logic.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param canvas structure that contains the game board and its dimensions.
*/
void playTron(Player *p1, Player *p2, Canvas *canvas);

/**
   * @brief Function that verifies if the positions of both players are still valid.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param canvas structure that contains the game board and its dimensions.
   * 
   * @return 1 if both positions are valid, 0 if any position is invalid.
*/
int validPositions(Player *p1, Player *p2, Canvas *canvas);

/**
   * @brief Function that sets both players' colors to white, draws them and shows a message.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param canvas structure that contains the game board and its dimensions.
   * 
   * @return always returns 0, given that the game is over.
*/
int gameTied(Player *p1, Player *p2, Canvas *canvas);

/**
   * @brief Function that initializes the width and height of a canvas structure using a scale of the screen dimensions.
   * 
   * @param canvas structure that contains the game board dimensions.
*/
void dimensions(Canvas *canvas);

/**
   * @brief Function that draws a player point on the game canvas.
   *
   * @param p player.
   * @param canvas structure that contains the game board and its dimensions.
*/
void drawPlayer(Player * p, Canvas *canvas);

/**
   * @brief Function that draws both players.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param canvas structure that contains the game board and its dimensions.
*/
void drawPlayers(Player *p1, Player *p2, Canvas *canvas);

/**
   * @brief Function that draws the game board on screen given a canvas with dimensions and a color.
   * 
   * @param canvas structure that contains the dimensions of the game board
   * @param color game board color
*/
void drawCanvas(Canvas *canvas, Color color);

/**
   * @brief Function that returns a key or 0 if no key is pressed in a given time frame.
   * 
   * @return ascii of the pressed key or 0 if no key was pressed.
*/
int getKey();

/**
   * @brief Function that pause the game and shows a message.
*/
void pause();

/**
   * @brief Function that shows the game result and beeps.
   * 
   * @param string message to be shown on screen.
   * @param canvas structure that contains the game board dimensions. 
*/
void endGame(char* string, Canvas *canvas);

/**
   * @brief Function that shows in game messages.
*/
void drawInfo();

/**
   * @brief Function that removes previous messages.
*/
void clearInfo();

/**
   * @brief Function that displays end of game messages/information.
*/
void endInfo();

/**
   * @brief Function that displays a countdown on the center of the screen.
*/
void countdown();

/**
   * @brief Function that modifies the coordinates of a player a unit.
   *
   * @param player
*/
void tick(Player *player);

/**
   * @brief Function that initializes a matrix with zeros, given width and height.
   *
   * @param canvas structure that contains the matrix to be initialized and its dimensions.
*/
void initializeCanvas(Canvas *canvas);

/**
   * @brief Function that defines the starting color, coordinates & directions of both players.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param canvas structure that contains a matrix in which the game will take place,
   *               and its dimensions are used to define player starting positions.
*/
void initializePlayers(Player *p1, Player *p2, Canvas *canvas);

/**
   * @brief Function that validates if a point is within a given matrix.
   *
   * @param x coordinate x of the point.
   * @param y coordinate y of the point.
   * @param canvas structure that contains the matrix.
   * 
   * @return 1 if the point is within the matrix/valid, 0 if the point is invalid.
*/
int isValidPoint(int x, int y, Canvas *canvas);

/**
   * @brief Function that analyzes a new key press and modifies, or not, the direction in which the player points towards.
   *		   It also allows the user to pause or restart the game.
   *
   * @param p1 player 1.
   * @param p2 player 2.
   * @param oldKey last key pressed.
   * @param newKey new key pressed.
   * 
   * @return 1 if the user pressed 'r', 0 if not.
*/
int input(Player *p1, Player *p2, int *oldKey, int newKey);

/**
   * @brief Function that resets both players scores to zero.
   * 
   * @return ascii value of the last key pressed.
*/
int resetScore();
#endif
