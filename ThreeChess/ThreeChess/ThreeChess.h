#ifndef _ThreeChess_H_
#define _ThreeChess_H_


#include <stdio.h>
#include <windows.h>
#include <time.h>
#pragma warning (disable:4996)


#define ROW 3
#define COL 3

#define PLAYER_COLOR 'X'
#define COMPUTER_COLOR 'O'


void Game();

void InitBoard(char board[][COL],int row, int col);

void ShowBoard(char board[][COL], int row, int col);

char Judge(char board[][COL], int row, int col);

void PlayerMove(char board[][COL], int row, int col);

void ComputerMove(char board[][COL], int row, int col);

int GetRandom(int start, int end);

#endif 

