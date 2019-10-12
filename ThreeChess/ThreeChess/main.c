#include "ThreeChess.h"


void ShowMenu()
{
	printf("#########¡¶Èý×ÓÆåÓÎÏ·¡·##########\n");
	printf("#################################\n");
	printf("########## 1. Play  #############\n");
	printf("########## 2. Exit  #############\n");
	printf("#################################\n");
	printf("Please Select: ");
}


int main()
{
	int select = 0;
	int quit = 0;
	while (!quit){
		ShowMenu();
		scanf("%d", &select);
		Sleep(500);
		system("CLS");
		switch (select){
		case 1:
			Game();
			break;
		case 2:
			printf("Bye bye!\n");
			quit = 1;
			break;
		default:
			printf("select error, try again!\n");
			break;
		}
	}
	system("pause");
	return 0;
}