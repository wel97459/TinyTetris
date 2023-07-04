#ifndef _TETRIS_H
#define _TETRIS_H
#include <stdint.h>
#include <nstdlib.h>

#define T (*t) //makes it look p
#define P (*p) //makes it look p

/*
   Piece data is store as upper nibbe X and lower nibble Y .
   With 0-7 as 0.5 or 0=0, 7=4.5
   This allows for 3 to be the center of rotation of all tetris pieces
   Since there is only 4 blocks in a tetris piece, this fits into 4 byte for each piece.
   Ordering, Low Y value frist order highest to last  
*/
uint8_t pieces[] ={
    0x33,0x33,0x13,0x53,0x15, //Orange Ricky L
    0x33,0x11,0x31,0x33,0x35, //Orange Ricky L
    0x33,0x51,0x33,0x53,0x13, //Orange Ricky L
    0x33,0x31,0x33,0x55,0x35, //Orange Ricky L

    0x33,0x33,0x53,0x13,0x55, //Blue Ricky J
    0x33,0x31,0x33,0x15,0x35, //Blue Ricky J
    0x33,0x11,0x33,0x13,0x53, //Blue Ricky J
    0x33,0x51,0x31,0x33,0x35, //Blue Ricky J

    0x30,0x12,0x32,0x34,0x54, //Cleveland Z 
    0x03,0x41,0x43,0x23,0x25, //Cleveland Z 
    0x30,0x12,0x32,0x34,0x54, //Cleveland Z 
    0x03,0x41,0x43,0x23,0x25, //Cleveland Z 

    0x30,0x52,0x32,0x34,0x14, //Rhode Island S 
    0x03,0x21,0x43,0x23,0x45, //Rhode Island S  
    0x30,0x52,0x32,0x34,0x14, //Rhode Island S 
    0x03,0x21,0x43,0x23,0x45, //Rhode Island S  

    0x43,0x43,0x23,0x63,0x03, //Hero I
    0x32,0x30,0x32,0x34,0x36, //Hero I
    0x43,0x43,0x23,0x63,0x03, //Hero I
    0x32,0x30,0x32,0x34,0x36, //Hero I

    0x33,0x33,0x13,0x53,0x35, //Teewee T
    0x33,0x31,0x33,0x13,0x35, //Teewee T
    0x33,0x13,0x31,0x33,0x53, //Teewee T
    0x33,0x31,0x33,0x53,0x35, //Teewee T

    0x22,0x22,0x42,0x44,0x24, //Smashboy O
    0x22,0x22,0x42,0x44,0x24, //Smashboy O
    0x22,0x22,0x42,0x44,0x24, //Smashboy O
    0x22,0x22,0x42,0x44,0x24, //Smashboy O
};

const uint8_t mul5lookup[35] = {
    0,5,10,15,20,25,30,35,40,45,50,
    55,60,65,70,75,80,85,90,95,100,
    105,110,115,120,125,130,135,140,
    145,150,155,160,165,170
};

const uint16_t scoreAmount[4] = {
    40,
    100,
    300,
    1200
};

const uint8_t speeds[] = {
    38,
    34,
    30,
    26,
    22,
    18,
    14,
    10,
    6,
    5,
    4,
    3,
    2,
    1,
    0
};

typedef struct __Blocks
{
    uint8_t x[4], y[4];
    uint8_t piece;
    uint8_t color;
} Blocks;

typedef struct __Tetris Tetris;
struct __Tetris
{
    //!!! Dont't move or re-order board or functions !!!
    uint8_t *board; //pointer to board array

    void (*Clear)(Tetris *t);
    void (*BuildPiece)(Tetris *t);
    void (*MovePiece)(Tetris *t);
    void (*UpdateBoard)(Tetris *t);
    void (*UpdateBoardBlock)(Tetris *, uint8_t, uint8_t, uint8_t);
    void (*RemoveLine)(Tetris *t, uint8_t yy);
    void (*ShowGhost)(Tetris *t);
    void (*ShowNext)(Tetris *t);
    void (*UpdateInfo)(Tetris *t);
    void (*PlaySound)(Tetris *t, uint8_t s);
    uint8_t board_min_y; //how high to move block down from

    uint16_t score;
    uint16_t linesRemoved;
    uint16_t linesRemovedOffset;

    uint8_t move;
    uint8_t moveLast;
    uint8_t fallHeld;

    uint8_t level;
    uint8_t speed;
    uint8_t ticks;
    uint8_t das;

    uint8_t pause;
    uint8_t gameover;

    uint8_t lockInSpeed;
    uint8_t lockIn;

    uint8_t piece_x;
    uint8_t piece_y;
    Blocks  piece;
    Blocks  pieceNext;
    uint8_t lastPiece;
    uint8_t nextPiece;

    uint8_t ghost_piece_x;
    uint8_t ghost_piece_y;
    Blocks  ghost_piece;
};

void tetris_init(Tetris *t, uint8_t *g);
void tetris_tick(Tetris *t);
uint8_t tetris_getRandomPiece(uint8_t exclude);
void tetris_buildPiece(Tetris *t, Blocks *p, uint8_t pnum);
void tetris_clearBoard(Tetris *t);
uint8_t tetris_checkPieceCollision(Tetris *t, Blocks *p, uint8_t ox, uint8_t oy);
void tetris_placeBlock(Tetris *t, uint8_t x, uint8_t y, uint8_t c);
void tetris_placePiece(Tetris *t, uint8_t x, uint8_t y);
uint8_t tetris_checkBoard(Tetris *t, uint8_t x, uint8_t y);
uint8_t tetris_checkPieceCollision(Tetris *t,Blocks *p, uint8_t ox, uint8_t oy);
void tetris_rotatePiece(Tetris *t, Blocks *p);
void tetris_checkLine(Tetris *t);
void tetris_get_ghost(Tetris *t);
#endif //_TETRIS_H
