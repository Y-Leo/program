#include "ThreeChess.h"

/*
void InitBoard(char board[][COL], int row, int col)
{
	int i = 0;
	for (i; i <= ROW; i++)
	{
		int j = 0;
		for (j; j <= COL; j++)
		{
			board[i][j] = ' ';//全部置为空格
		}
	}
}
*/


void InitBoard(char board[][COL], int row, int col)
{
	memset(board, ' ', sizeof(board[0][0])* row*col);
}
//memset函数用来对一段内存空间全部设置为某个字符


void ShowBoard(char board[][COL], int row, int col)
{
	printf("   |  1 |  2 | 3\n");
	printf("---|----|----|----\n");
	int i = 1;
	for (i = 1; i <= ROW; i++)
	{
		printf(" %d | %c  | %c  | %c\n",i, board[i-1][0], board[i-1][1], board[i-1][2]);
		if (i!=ROW)
		{
			printf("---|----|----|----\n");
		}
	}
}


char Judge(char board[][COL], int row, int col)
{
	int i = 0;
	for (i; i < row; i++)
	{
		if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])//行
		{
			return board[i][0];
		}
	}

	for (i = 0; i < col; i++)
	{
		if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])//列
		{
			return board[0][i];
		}
	}
	if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
	{
		return board[1][1];
	}
	if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
	{
		return board[1][1];
	}
	for (i = 0; i < row; i++)
	{
		int j = 0;
		for (; j < col; j++)
		{
			if (board[i][j] == ' ')
			{
				return 'N';
			}
		}
	}
	return 'E';
}


void PlayerMove(char board[][COL], int row, int col)
{
	while (1)
	{
		int x = 0;
		int y = 0;
		printf("Please input Pos<x,y>:");
		scanf("%d %d", &x, &y);
		if (x >= 1 && x <= 3 && y >= 1 && y <= 3)
		{
			if (board[x - 1][y - 1] == ' ')
			{
				board[x - 1][y - 1] = PLAYER_COLOR;
				printf("\n");
				Sleep(500);
				break;
			}
			else{
				printf("Inter error,please try again！\n");
			}
		}
		else{
			printf("Enter wrongful!\n");
		}
	}
}



void ComputerMove(char board[][COL], int row, int col)
{
	while (1)
	{
		//int x = rand()%row;[0,2]
		//int y = rand()%col;[0,2]
		int x = GetRandom(1, 3);//随机数生成电脑落子的横坐标
		int y = GetRandom(1, 3);//随机数生成电脑落子的纵坐标
		if (board[x-1][y-1] == ' ')
		{
			board[x - 1][y - 1] = COMPUTER_COLOR;
			printf("\n");
			break;
		}
	}
}

int GetRandom(int start, int end)
{
	return rand() % (end - start + 1) + start;//固定公式，随机数生成区间
}


void Game()
{
	char result = '\0';
	char board[ROW][COL];
	InitBoard(board, ROW, COL);
	ShowBoard(board, ROW, COL);
	srand((unsigned int)time(NULL));
	while (1){
		PlayerMove(board, ROW, COL);
		ShowBoard(board, ROW, COL);
		result = Judge(board, ROW, COL);
		if (result != 'N'){//next
			break;
		}
		ComputerMove(board, ROW, COL);
		ShowBoard(board, ROW, COL);
		result = Judge(board, ROW, COL);
		if (result != 'N'){
			break;
		}
	}
	switch (result){
	case 'X': //you win
		printf("You win!\n");
		break;
	case 'O': //computer win
		printf("The computer win!\n");
		break;
	case 'E': //Equal
		printf("Equal!\n");
		break;
	default:
		printf("bug?!\n");
		break;
	}
	//memset(board, ' ', sizeof(board));
	printf("再来一把？\n");
}