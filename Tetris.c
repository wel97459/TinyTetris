#include <stdint.h>
#include "Tetris.h"

uint16_t randnum=0x2cf6;

void tetris_randomSeed(uint16_t s)
{
    randnum = s;
}

uint8_t tetris_getRand(){
    uint8_t i = (randnum&0x0002) ? 1 : 0;
    uint8_t j = (randnum&0x0800) ? 1 : 0;
    randnum = randnum << 1;
    randnum = randnum | (uint16_t) (!j^i);
    return randnum & 0x00ff;
}


void tetris_init(Tetris *t, uint8_t *g)
{
	if(randnum == 0){
		randnum = 0x2cf6;
	}

	T.board = g;
	T.board_min_y = 20;

	
	T.ticks=0;
	T.level=0;
	T.speed = speeds[T.level];
	T.lockInSpeed = 3;
	T.pause = 0;
	T.das = 0;

	T.gameover = 0;
	T.score = 0;
	T.linesRemoved = 0;
	T.linesRemovedOffset = 0;

	T.move=0;
	T.moveLast=0;
	T.fallHeld = 0;
	
	T.piece_x = 4;
	T.piece_y = 0;

    if(T.Clear) T.Clear(t);
	tetris_clearBoard(t);
	if(T.UpdateBoard) T.UpdateBoard(t);
	if(T.UpdateInfo) T.UpdateInfo(t);

	T.lastPiece = tetris_getRandomPiece(0);
	T.nextPiece = tetris_getRandomPiece(T.lastPiece);
	tetris_buildPiece(t, &T.pieceNext, T.nextPiece);
	tetris_buildPiece(t, &T.piece, T.lastPiece);

	if(T.BuildPiece) T.BuildPiece(t);
	if(T.ShowNext) T.ShowNext(t);
	if(T.MovePiece) T.MovePiece(t);
	tetris_get_ghost(t);
}

/*
	Main game tick
*/
void tetris_tick(Tetris *t)
{
	uint8_t pc, isTicked;
	
	if (!T.gameover && !T.pause)
		T.ticks++;

	isTicked = T.ticks > T.speed;
	if((isTicked || T.move > 0 || T.das) && (!T.gameover && !T.pause))
	{

		//Reset the hold on fall
		if (T.fallHeld && !checkBit(&T.move, 0))
			T.fallHeld = 0;

		if (isTicked || !T.fallHeld && checkBit(&T.move, 0))
		{
			pc = tetris_checkPieceCollision(t, &T.piece, T.piece_x, T.piece_y+1);
			//check to see if we can move down.
			if(!pc){
				//player want to make the piece to go donw fast.
				T.piece_y++;
				if(T.MovePiece) T.MovePiece(t);
				clearBit(&T.move, 0);
			} else if(!T.move){
				//start counting up the lock in.
				T.lockIn++;
			}
		}

		pc = tetris_checkPieceCollision(t, &T.piece, T.piece_x+1, T.piece_y);
		if (checkBit(&T.move, 1) && !checkBit(&T.moveLast, 1) && !pc)
		{
			T.piece_x++;
			T.lockIn=0;
			tetris_get_ghost(t);
			if(T.MovePiece) T.MovePiece(t);
		}

		pc = tetris_checkPieceCollision(t, &T.piece, T.piece_x-1, T.piece_y);
		if (checkBit(&T.move, 2) && !checkBit(&T.moveLast, 2) && !pc)
		{
			T.piece_x--;
			T.lockIn=0;
			tetris_get_ghost(t);
			if(T.MovePiece) T.MovePiece(t);
		}

		if (checkBit(&T.move, 3) && !checkBit(&T.moveLast, 3))
		{ 
			tetris_rotatePiece(t, &T.piece);
			T.lockIn = 0;
		}

		if(checkBit(&T.move, 2) || checkBit(&T.move, 1)){
			T.das++;
			pc = T.das & 0x07;
			if(T.das > 12 && pc == 6 || T.das == 12){
				clearBit(&T.move, 1);
				clearBit(&T.move, 2);
				T.das = 12;
			}
		} else {
			T.das = 0;
		} 

		pc = checkBit(&T.move, 0);
		if (T.lockIn > 1 || !T.fallHeld && pc)
		{
			if (pc){
				T.fallHeld = 1;
				T.score+=5;
				if(T.UpdateInfo) T.UpdateInfo(t);
				if(T.UpdateBoard) T.PlaySound(t, 2);
			}else{
				if(T.UpdateBoard) T.PlaySound(t, 3);
			}
			
			T.lockIn = 0;
			tetris_placePiece(t, T.piece_x, T.piece_y);
			tetris_checkLine(t);
			T.lastPiece = T.nextPiece;
			T.nextPiece = tetris_getRandomPiece(T.lastPiece);
			tetris_buildPiece(t, &T.pieceNext, T.nextPiece);
			tetris_buildPiece(t, &T.piece, T.lastPiece);
			if(T.BuildPiece) T.BuildPiece(t);
			if(T.ShowNext) T.ShowNext(t);
			T.piece_x = 4;
			T.piece_y = 0;
			if(T.MovePiece) T.MovePiece(t);
			T.move=0;
			pc = tetris_checkPieceCollision(t, &T.piece, T.piece_x, T.piece_y);
			if(pc){
				T.gameover = 1;
				if(T.UpdateInfo) T.UpdateInfo(t);
			}

			if(T.linesRemoved - T.linesRemovedOffset >= 10)
			{
				T.level++;
				T.speed = speeds[T.level];
				T.linesRemovedOffset = T.linesRemoved;
			}
		}
	}
	if(isTicked) T.ticks = 0;

	if (checkBit(&T.move, 4) && !checkBit(&T.moveLast, 4))
	{
		T.pause = !T.pause;
	}
	
	T.moveLast = T.move;
}


/*
	Get Random Piece
*/
uint8_t tetris_getRandomPiece(uint8_t exclude)
{
	uint8_t i;
	i = shiftLeft8(tetris_getRand() % 7, 2);
	if(exclude == i){
		i = shiftLeft8(tetris_getRand() % 7, 2);
	}
	return i;
}


/*
	Place block locations into piece.
*/
void tetris_buildPiece(Tetris *t, Blocks *p, uint8_t pnum)
{
	uint8_t i,j,k,l;
	l = pieces[mul5lookup[pnum]];
	P.piece = pnum;
	P.color = (pnum>>2);

	pnum = mul5lookup[pnum];
	for(i=0; i<4; i++)
    {
        j = pieces[pnum+i+1];

        k = ((j >> 4)<<2);
		if((l & 0x10)) {
			k-=((l & 0xf0)>>4)+1;
		}
		P.x[i] = k;

        k = ((j & 0x0f)<<2);
		if((l & 0x01)){
			k-=(l&0x0f)+1;
		}
		P.y[i] = k;
    }
}

/*
	Sets a block in the board at board X,Y of Palette color.
	And also calls the extern function UpdateBoardBlock.
*/
void tetris_placeBlock(Tetris *t, uint8_t xx, uint8_t yy, uint8_t c)
{
	uint8_t cc;
	uint8_t py;

	cc = c & 0x0f;
	cc |= c << 4;
	py = mul5lookup[yy]+(xx>>1);

	if(xx & 0x01){ 
		T.board[py] = (T.board[py] & 0xf0) | (cc & 0x0F);
	}else{
		T.board[py] = (T.board[py] & 0x0f) | (cc & 0xF0);
	}

	if(T.UpdateBoardBlock) T.UpdateBoardBlock(t, xx, yy, T.board[py]);
}

/*
	Places Current Piece at location X,Y;
*/
void tetris_placePiece(Tetris *t, uint8_t xx, uint8_t yy)
{
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		tetris_placeBlock(t, (T.piece.x[i]>>3)+xx-1, (T.piece.y[i]>>3)+yy-1, T.piece.color+3);
	}
	if(T.UpdateBoard) T.UpdateBoard(t);
}

/*
	Check board for a block at X,Y
	Return the block color
*/ 
uint8_t tetris_checkBoard(Tetris *t, uint8_t xx, uint8_t yy)
{
	uint8_t p = mul5lookup[yy]+(xx>>1);
	
	if (yy > 19) return 1;

	if(xx & 0x01){ 
		return T.board[p] & 0x0f;
	}else{
		return (T.board[p] & 0xf0)>>4;
	}
}


/*
	Check if there is a collision with the board and the current piece
	returns 0 if none, or the block that is hit
*/
uint8_t tetris_checkPieceCollision(Tetris *t, Blocks *p, uint8_t ox, uint8_t oy)
{
	uint8_t i;
	int8_t xx, yy;
	for (i = 0; i < 4; i++)
	{
		xx = (P.x[i]>>3)+ox-1;
		yy = (P.y[i]>>3)+oy-1;
		if(tetris_checkBoard(t, xx, yy) > 1) return i+1;
		if(xx > 9 || xx < 0 || yy > 19) return i+1;
	}

	return 0;
}

void tetris_rotatePiece(Tetris *t, Blocks *p)
{
	Blocks b;
	uint8_t piece = P.piece;
	P.piece = (P.piece & 0xfc) | (P.piece + 1) & 0x03;
	tetris_buildPiece(t, &b, P.piece);

	if(!tetris_checkPieceCollision(t, &b, T.piece_x, T.piece_y)){
		tetris_buildPiece(t, &T.piece, P.piece);
		if(T.BuildPiece) T.BuildPiece(t);
		if(T.MovePiece) T.MovePiece(t);
	}else{
		P.piece = piece;
	}
}

void tetris_removeLine(Tetris *t, uint8_t maxY)
{
	uint8_t my, bmy;
	uint8_t yy, xx;uint8_t level;
	if (T.RemoveLine) T.RemoveLine(t, maxY);
	
	for (yy = maxY; yy >= T.board_min_y; yy--)
	{
		my = mul5lookup[yy];
		bmy = mul5lookup[yy-1];
		for (xx = 0; xx < 5; xx++)
		{
			T.board[xx+my] = T.board[xx+bmy];
		}
		
	}
	T.board_min_y++;
	if (T.UpdateBoard) T.UpdateBoard(t);
}

void tetris_checkLine(Tetris *t)
{
	uint8_t x1, y1, yy, my, ly=255, i, c=0, b=0;
	for(i=0; i<4; i++)
	{
		y1= (T.piece.y[i]>>3)+T.piece_y-1;
		if(y1<T.board_min_y) T.board_min_y = y1;

		my = mul5lookup[y1];
		for(x1=0; x1<5; x1++)
		{
			yy = T.board[my+x1];

			if((yy & 0xf0) == 0x10) c++;
			if((yy & 0x0f) == 0x01) c++;
		}

		if(c == 0 && y1 != ly){
			tetris_removeLine(t, y1);
			if(T.UpdateBoard) T.PlaySound(t, 1);
			T.linesRemoved++;
			b++;
			ly = y1;
		}
		c=0;
	}
	if(b) T.score += scoreAmount[b-1];
	if(T.UpdateInfo) T.UpdateInfo(t);
}

void tetris_clearBoard(Tetris *t){
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		t->board[i] = 0x11;
	}
}

void tetris_get_ghost(Tetris *t)
{

}
