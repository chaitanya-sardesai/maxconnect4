// MaxConnect-4 code

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

long COMPUTER = 2;
long HUMAN = 1;
char COMP_OUTPUT_FILE[] = "computer.txt";
char HUMAN_OUTPUT_FILE[] = "human.txt";
const int evaluationTable[6][7] = 		 {{3, 4, 5, 7, 5, 4, 3}, 
                                          {4, 6, 8, 10, 8, 6, 4},
                                          {5, 8, 11, 12, 11, 8, 5}, 
                                          {5, 8, 11, 12, 11, 8, 5},
                                          {4, 6, 8, 10, 8, 6, 4},
                                          {3, 4, 5, 7, 5, 4, 3}};

class gameStatus
{
private:
  long * gameData;

public:
  long * gameBoard[6];
  long currentTurn;
  int player1Score;
  int player2Score;
  int pieceCount;
  int nextMove;
  FILE *gameFile;
  int preMoves[42];
  int depth;

  gameStatus()
  {
    gameData = new long[42];
    gameBoard[0] = &(gameData[0]);
    gameBoard[1] = &(gameData[7]);
    gameBoard[2] = &(gameData[14]);
    gameBoard[3] = &(gameData[21]);
    gameBoard[4] = &(gameData[28]);
    gameBoard[5] = &(gameData[35]);

    int i;
    for (i = 0; i < 42; i++)
    {
      gameData[i] = 0;
	  preMoves[i] = -1;
    }

    currentTurn = COMPUTER;
    player1Score = 0;
    player2Score = 0;
    pieceCount = 0;
    gameFile = 0;
	nextMove = 0;
	depth = 42;
  }

  ~gameStatus()
  {
    delete [] gameData;
  }
};
void countScore(gameStatus &currentGame);
int maxValue(gameStatus &tempCurrGame, int *alpha, int *beta, int *currDepth);//int *ptrAlpha, int *ptrBeta);
int minValue(gameStatus &tempCurrGame, int *alpha, int *beta, int *currDepth);//int *ptrAlpha, int *ptrBeta);
int deletePrevMove(gameStatus &tempCurrGame,  int colAction, int row);
void getCurrentStateFromFile(gameStatus &currentGame, char* fileName);
void getMoveFromHuman(gameStatus &currentGame, char* fileName);
void writeCurrentStateInFile(gameStatus &currentGame, char* fileName);
// Output current game status to console
void printGameBoard(gameStatus &currentGame)
{
  int i = 0;
  int j = 0;
  printf(" -----------------\n");
  for(i = 0; i < 6; i++)
  {
    printf(" | ");
    for(j = 0; j < 7; j++)
    {
      printf("%li ", currentGame.gameBoard[i][j]);
    }
    printf("| \n");
  }
  printf(" -----------------\n");
}

// Output current game status to file
void printGameBoardToFile(gameStatus &currentGame)
{
	int i = 0;
	int j = 0;
	for(i = 0; i < 6; i++)
	{
		for(j = 0; j < 7; j++)
		{
			fprintf(currentGame.gameFile, "%li", currentGame.gameBoard[i][j]);
		}
		fprintf(currentGame.gameFile, "\r\n");
	}
	fprintf(currentGame.gameFile, "%d\r\n", currentGame.currentTurn);
}

// Given a column and which player it is,
// place that player's piece in the requested column
int playPiece(int column, gameStatus &currentGame, char* fileName)
{
	// if column full, return 1
	if(currentGame.gameBoard[0][column] != 0)
  {
	  //printf("column full in play piece \n");
		return -1;
  }

  int i;
	// starting at the bottom of the board, place the piece into the
	// first empty spot
	for(i = 5; i >= 0; i--)
	{
		if(currentGame.gameBoard[i][column] == 0)
		{
			currentGame.gameBoard[i][column] = currentGame.currentTurn;
			//printf("played piece at %ld\n",currentGame.gameBoard[i][column]);
			currentGame.pieceCount++;
			if(fileName != NULL)
				writeCurrentStateInFile(currentGame, fileName);
			return i;
		}
	}
	//printf("Nothing\n");
  return -1;
}

int movePossible(gameStatus &currentGame, int column)
{
	// if column full, return 0
	if(currentGame.gameBoard[0][column] != 0)
	{
		return 0;
	}
	return 1;
}

int terminalTest(gameStatus &tempCurrGame, int currDepth)
{
	if(tempCurrGame.pieceCount == 42)
	{
		return 1;
	}
	else if(currDepth == tempCurrGame.depth)
		return 2;

	return 0;
}

int evalFunction(gameStatus &tempCurrGame)
{
	int utility = 138;
	int sum = 0;
	if(COMPUTER == 1)
	{
		for (int i = 0; i < 6; i++)
            for (int j = 0; j < 7; j++)
                if (tempCurrGame.gameBoard[i][j] == 1)
                    sum += evaluationTable[i][j];
                else if (tempCurrGame.gameBoard[i][j] == 2)
                    sum -= evaluationTable[i][j];
		if(tempCurrGame.player1Score>tempCurrGame.player2Score)
			sum+=50;
		else if(tempCurrGame.player1Score<tempCurrGame.player2Score)
			sum-=50;
		else
			sum+=0;
    }
	else
	{
		for (int i = 0; i < 6; i++)
            for (int j = 0; j < 7; j++)
                if (tempCurrGame.gameBoard[i][j] == 1)
                    sum += evaluationTable[i][j];
                else if (tempCurrGame.gameBoard[i][j] == 2)
                    sum -= evaluationTable[i][j];
				
		if(tempCurrGame.player1Score<tempCurrGame.player2Score)
			sum+=50;
		else if(tempCurrGame.player1Score>tempCurrGame.player2Score)
			sum-=50;
		else
			sum+=0;
    }
	return utility + sum;
}

int utility(gameStatus &tempCurrGame, int gameBoardFull)
{
	countScore(tempCurrGame);
	if(gameBoardFull == 1)
	{
		if(COMPUTER == 1)
		{
			if(tempCurrGame.player1Score>tempCurrGame.player2Score)
				return (tempCurrGame.player1Score+1);
			else if(tempCurrGame.player1Score<tempCurrGame.player2Score)
				return (tempCurrGame.player1Score-1);
			else
				return (tempCurrGame.player1Score+0);
		}
		else
		{
			if(tempCurrGame.player1Score<tempCurrGame.player2Score)
				return (tempCurrGame.player2Score+1);
			else if(tempCurrGame.player1Score>tempCurrGame.player2Score)
				return (tempCurrGame.player2Score-1);
			else
				return (tempCurrGame.player2Score+0);
		}
	}
	else
	{
		return evalFunction(tempCurrGame);
	}
}

int makeMove(gameStatus &tempCurrGame, int colAction)
{
	int result = playPiece(colAction, tempCurrGame, NULL);
	if(tempCurrGame.currentTurn == 1)
		tempCurrGame.currentTurn = 2;
	else if (tempCurrGame.currentTurn == 2)
		tempCurrGame.currentTurn = 1;
	return result;
}

int max(int val1, int val2)
{
    if(val1>val2)
	{
		return val1;
	}
    else if(val1<val2)
	{
		return val2;
	}
    else
	{
		return val2;
	}
}

int min(int val1, int val2)
{
    if(val1>val2)
	{
		return val2;
	}
    else if(val1<val2)
	{
		return val1;
	}
    else
    {
		return val1;
	}
}

int deletePrevMove(gameStatus &tempCurrGame,  int colAction, int row)
{
	int result = 0;
	int i;
	tempCurrGame.gameBoard[row][colAction] = 0;
	tempCurrGame.pieceCount--;
	result =1;
	if(tempCurrGame.currentTurn == 1)
		tempCurrGame.currentTurn = 2;
	else if (tempCurrGame.currentTurn == 2)
		tempCurrGame.currentTurn = 1;
	return result;
}


void findAllActions(gameStatus &tempCurrGame, int *actionState)
{
	int i = 0;
	int col;
	
	for(i=0;i<7;i++)
	{
		actionState[i] = -1;
	}
	i = 0;
	for(col = 0; col<7; col++)
	{
		if(tempCurrGame.gameBoard[0][col] == 0)
		{
			actionState[i] = col;
			i++;
		}
	}
	
}

int minValue(gameStatus &tempCurrGame, int *ptrAlpha, int *ptrBeta, int *currDepth)//int alphaMin, int betaMin)
{
	int actionState[7] = {-1,-1,-1,-1,-1,-1,-1};
	int utilityVal = -1;
	int gameBoardFull = 0;
	if(gameBoardFull = terminalTest(tempCurrGame, *currDepth))
	{
		utilityVal = utility(tempCurrGame, gameBoardFull);
		return utilityVal;
	}

	int colValueMin = 1000;//*ptrBeta;1000; //(-infinity)
	findAllActions(tempCurrGame, actionState);
	
	int i = 0;
	int row = 0;
	for(i = 0;i<7;i++)
	{
		if(actionState[i]!=-1)
		{
			if(movePossible(tempCurrGame, actionState[i]) == 0)
			{
				actionState[i] = -1;
				//i++;
				continue;
			}
			row = makeMove(tempCurrGame, actionState[i]);
			(*currDepth)++;
			colValueMin = min(colValueMin, maxValue(tempCurrGame, ptrAlpha, ptrBeta, currDepth));
			deletePrevMove(tempCurrGame, actionState[i], row);
			(*currDepth)--;
			if(colValueMin<=*ptrAlpha)
			{
				tempCurrGame.nextMove = actionState[i];
				return colValueMin;
			}
			*ptrBeta = min(*ptrBeta, colValueMin);
		}
		else
		{
			//printf("No action left in Min\n");
			break;
		}
	}
	return colValueMin;
}


int maxValue(gameStatus &tempCurrGame, int *ptrAlpha, int *ptrBeta, int *currDepth)//int alphaMax, int betaMax)
{
	int utilityVal = -1;
	int actionState[7] = {-1,-1,-1,-1,-1,-1,-1};
	int gameBoardFull = 0;
	if(gameBoardFull = terminalTest(tempCurrGame, *currDepth))
	{
		utilityVal = utility(tempCurrGame, gameBoardFull);
		return utilityVal;
	}
	int colValueMax = -1000;//*ptrAlpha;//-1000; //(-infinity)
	findAllActions(tempCurrGame, actionState);
	int i = 0;
	int row = 0;
	for(i = 0;i<7;i++)
	{
		if(actionState[i]!=-1)
		{
			if(movePossible(tempCurrGame, actionState[i]) == 0)
			{
				//printf("Move not possible\n");
				actionState[i] = -1;
				//i++;
				continue;
			}
			row = makeMove(tempCurrGame, actionState[i]);
			(*currDepth)++;
			colValueMax = max(colValueMax, minValue(tempCurrGame, ptrAlpha, ptrBeta, currDepth));
			deletePrevMove(tempCurrGame, actionState[i], row);
			(*currDepth)--;
			if(colValueMax>=*ptrBeta)
			{
				tempCurrGame.nextMove = actionState[i];
				return colValueMax;
			}
			*ptrAlpha = max(*ptrAlpha, colValueMax);
		}
		else
		{
			//printf("No action left in Max\n");
			break;
		}
	}
	return colValueMax;
}

int alphaBetaSearch(gameStatus &tempCurrGame, char* fileName)
{
	int column = 0;
	int i =0;
	int alpha = -1000, beta = 1000;
	int actionState[7] = {-1,-1,-1,-1,-1,-1,-1};
	int bestMove[7] = {-1,-1,-1,-1,-1,-1,-1};
	int currDepth = 0;
	
	findAllActions(tempCurrGame, actionState);
	for(i=0;actionState[i]!=-1 && i<7;i++)
	{
		makeMove(tempCurrGame, actionState[i]);
		bestMove[i] = minValue(tempCurrGame, &alpha, &beta, &currDepth);
		getCurrentStateFromFile(tempCurrGame, fileName);
		tempCurrGame.currentTurn = 1;
		currDepth = 0;
	}
	
	i=0;
	int Max = bestMove[i];
	int temp = i;
	while(i<7)
	{
		if(Max<bestMove[i])
		{
			temp = i;
			Max = bestMove[i];
		}
		i++;
	}
	tempCurrGame.nextMove = actionState[temp];
	column = tempCurrGame.nextMove;

	return column;
}

// The AI section.
void aiPlay(gameStatus &currentGame, char *inpuFileName, char *outputFileName)
{
	gameStatus tempCurrGame; //= currentGame;
	getCurrentStateFromFile(tempCurrGame, inpuFileName);
	tempCurrGame.depth = currentGame.depth;
	int column = alphaBetaSearch(tempCurrGame, inpuFileName);
	
	int result = 0;
	result = playPiece(column, currentGame, outputFileName);
	if(result == -1)
	{
		aiPlay(currentGame, inpuFileName, outputFileName);
	}
	else
	{
		printf("\n\nmove %li: Player %li, column %li\n",
           currentGame.pieceCount, currentGame.currentTurn, column+1);
		if(currentGame.currentTurn == 1)
			currentGame.currentTurn = 2;
		else if (currentGame.currentTurn == 2)
			currentGame.currentTurn = 1;
	}
}

void countScore(gameStatus &currentGame)
{
  currentGame.player1Score = 0;
  currentGame.player2Score = 0;

  //check horizontally
  int i;
  for(i = 0; i < 6; i++)
  {
		//check player 1
		if(currentGame.gameBoard[i][0] == 1 && currentGame.gameBoard[i][1] == 1
			&& currentGame.gameBoard[i][2] == 1 && currentGame.gameBoard[i][3] == 1)
			{currentGame.player1Score++;}
		if(currentGame.gameBoard[i][1] == 1 && currentGame.gameBoard[i][2] == 1
			&& currentGame.gameBoard[i][3] == 1 && currentGame.gameBoard[i][4] == 1)
			{currentGame.player1Score++;}
		if(currentGame.gameBoard[i][2] == 1 && currentGame.gameBoard[i][3] == 1
			&& currentGame.gameBoard[i][4] == 1 && currentGame.gameBoard[i][5] == 1)
			{currentGame.player1Score++;}
		if(currentGame.gameBoard[i][3] == 1 && currentGame.gameBoard[i][4] == 1
			&& currentGame.gameBoard[i][5] == 1 && currentGame.gameBoard[i][6] == 1)
			{currentGame.player1Score++;}
		//check player 2
		if(currentGame.gameBoard[i][0] == 2 && currentGame.gameBoard[i][1] == 2
			&& currentGame.gameBoard[i][2] == 2 && currentGame.gameBoard[i][3] == 2)
			{currentGame.player2Score++;}
		if(currentGame.gameBoard[i][1] == 2 && currentGame.gameBoard[i][2] == 2
			&& currentGame.gameBoard[i][3] == 2 && currentGame.gameBoard[i][4] == 2)
			{currentGame.player2Score++;}
		if(currentGame.gameBoard[i][2] == 2 && currentGame.gameBoard[i][3] == 2
			&& currentGame.gameBoard[i][4] == 2 && currentGame.gameBoard[i][5] == 2)
			{currentGame.player2Score++;}
		if(currentGame.gameBoard[i][3] == 2 && currentGame.gameBoard[i][4] == 2
			&& currentGame.gameBoard[i][5] == 2 && currentGame.gameBoard[i][6] == 2)
			{currentGame.player2Score++;}
	}

	//check vertically
	int j;
	for(j = 0; j < 7; j++)
	{
		//check player 1
		if(currentGame.gameBoard[0][j] == 1 && currentGame.gameBoard[1][j] == 1
			&& currentGame.gameBoard[2][j] == 1 && currentGame.gameBoard[3][j] == 1)
			{currentGame.player1Score++;}
		if(currentGame.gameBoard[1][j] == 1 && currentGame.gameBoard[2][j] == 1
			&& currentGame.gameBoard[3][j] == 1 && currentGame.gameBoard[4][j] == 1)
			{currentGame.player1Score++;}
		if(currentGame.gameBoard[2][j] == 1 && currentGame.gameBoard[3][j] == 1
			&& currentGame.gameBoard[4][j] == 1 && currentGame.gameBoard[5][j] == 1)
			{currentGame.player1Score++;}
		//check player 2
		if(currentGame.gameBoard[0][j] == 2 && currentGame.gameBoard[1][j] == 2
			&& currentGame.gameBoard[2][j] == 2 && currentGame.gameBoard[3][j] == 2)
			{currentGame.player2Score++;}
		if(currentGame.gameBoard[1][j] == 2 && currentGame.gameBoard[2][j] == 2
			&& currentGame.gameBoard[3][j] == 2 && currentGame.gameBoard[4][j] == 2)
			{currentGame.player2Score++;}
		if(currentGame.gameBoard[2][j] == 2 && currentGame.gameBoard[3][j] == 2
			&& currentGame.gameBoard[4][j] == 2 && currentGame.gameBoard[5][j] == 2)
			{currentGame.player2Score++;}
	}

	//check diagonally

	//check player 1
	if(currentGame.gameBoard[2][0] == 1 && currentGame.gameBoard[3][1] == 1
		&& currentGame.gameBoard[4][2] == 1 && currentGame.gameBoard[5][3] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][0] == 1 && currentGame.gameBoard[2][1] == 1
		&& currentGame.gameBoard[3][2] == 1 && currentGame.gameBoard[4][3] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][1] == 1 && currentGame.gameBoard[3][2] == 1
		&& currentGame.gameBoard[4][3] == 1 && currentGame.gameBoard[5][4] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][0] == 1 && currentGame.gameBoard[1][1] == 1
		&& currentGame.gameBoard[2][2] == 1 && currentGame.gameBoard[3][3] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][1] == 1 && currentGame.gameBoard[2][2] == 1
		&& currentGame.gameBoard[3][3] == 1 && currentGame.gameBoard[4][4] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][2] == 1 && currentGame.gameBoard[3][3] == 1
		&& currentGame.gameBoard[4][4] == 1 && currentGame.gameBoard[5][5] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][1] == 1 && currentGame.gameBoard[1][2] == 1
		&& currentGame.gameBoard[2][3] == 1 && currentGame.gameBoard[3][4] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][2] == 1 && currentGame.gameBoard[2][3] == 1
		&& currentGame.gameBoard[3][4] == 1 && currentGame.gameBoard[4][5] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][3] == 1 && currentGame.gameBoard[3][4] == 1
		&& currentGame.gameBoard[4][5] == 1 && currentGame.gameBoard[5][6] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][2] == 1 && currentGame.gameBoard[1][3] == 1
		&& currentGame.gameBoard[2][4] == 1 && currentGame.gameBoard[3][5] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][3] == 1 && currentGame.gameBoard[2][4] == 1
		&& currentGame.gameBoard[3][5] == 1 && currentGame.gameBoard[4][6] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][3] == 1 && currentGame.gameBoard[1][4] == 1
		&& currentGame.gameBoard[2][5] == 1 && currentGame.gameBoard[3][6] == 1)
			{currentGame.player1Score++;}

	if(currentGame.gameBoard[0][3] == 1 && currentGame.gameBoard[1][2] == 1
		&& currentGame.gameBoard[2][1] == 1 && currentGame.gameBoard[3][0] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][4] == 1 && currentGame.gameBoard[1][3] == 1
		&& currentGame.gameBoard[2][2] == 1 && currentGame.gameBoard[3][1] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][3] == 1 && currentGame.gameBoard[2][2] == 1
		&& currentGame.gameBoard[3][1] == 1 && currentGame.gameBoard[4][0] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][5] == 1 && currentGame.gameBoard[1][4] == 1
		&& currentGame.gameBoard[2][3] == 1 && currentGame.gameBoard[3][2] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][4] == 1 && currentGame.gameBoard[2][3] == 1
		&& currentGame.gameBoard[3][2] == 1 && currentGame.gameBoard[4][1] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][3] == 1 && currentGame.gameBoard[3][2] == 1
		&& currentGame.gameBoard[4][1] == 1 && currentGame.gameBoard[5][0] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[0][6] == 1 && currentGame.gameBoard[1][5] == 1
		&& currentGame.gameBoard[2][4] == 1 && currentGame.gameBoard[3][3] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][5] == 1 && currentGame.gameBoard[2][4] == 1
		&& currentGame.gameBoard[3][3] == 1 && currentGame.gameBoard[4][2] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][4] == 1 && currentGame.gameBoard[3][3] == 1
		&& currentGame.gameBoard[4][2] == 1 && currentGame.gameBoard[5][1] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[1][6] == 1 && currentGame.gameBoard[2][5] == 1
		&& currentGame.gameBoard[3][4] == 1 && currentGame.gameBoard[4][3] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][5] == 1 && currentGame.gameBoard[3][4] == 1
		&& currentGame.gameBoard[4][3] == 1 && currentGame.gameBoard[5][2] == 1)
			{currentGame.player1Score++;}
	if(currentGame.gameBoard[2][6] == 1 && currentGame.gameBoard[3][5] == 1
		&& currentGame.gameBoard[4][4] == 1 && currentGame.gameBoard[5][3] == 1)
			{currentGame.player1Score++;}

	//check player 2
	if(currentGame.gameBoard[2][0] == 2 && currentGame.gameBoard[3][1] == 2
		&& currentGame.gameBoard[4][2] == 2 && currentGame.gameBoard[5][3] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][0] == 2 && currentGame.gameBoard[2][1] == 2
		&& currentGame.gameBoard[3][2] == 2 && currentGame.gameBoard[4][3] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][1] == 2 && currentGame.gameBoard[3][2] == 2
		&& currentGame.gameBoard[4][3] == 2 && currentGame.gameBoard[5][4] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][0] == 2 && currentGame.gameBoard[1][1] == 2
		&& currentGame.gameBoard[2][2] == 2 && currentGame.gameBoard[3][3] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][1] == 2 && currentGame.gameBoard[2][2] == 2
		&& currentGame.gameBoard[3][3] == 2 && currentGame.gameBoard[4][4] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][2] == 2 && currentGame.gameBoard[3][3] == 2
		&& currentGame.gameBoard[4][4] == 2 && currentGame.gameBoard[5][5] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][1] == 2 && currentGame.gameBoard[1][2] == 2
		&& currentGame.gameBoard[2][3] == 2 && currentGame.gameBoard[3][4] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][2] == 2 && currentGame.gameBoard[2][3] == 2
		&& currentGame.gameBoard[3][4] == 2 && currentGame.gameBoard[4][5] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][3] == 2 && currentGame.gameBoard[3][4] == 2
		&& currentGame.gameBoard[4][5] == 2 && currentGame.gameBoard[5][6] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][2] == 2 && currentGame.gameBoard[1][3] == 2
		&& currentGame.gameBoard[2][4] == 2 && currentGame.gameBoard[3][5] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][3] == 2 && currentGame.gameBoard[2][4] == 2
		&& currentGame.gameBoard[3][5] == 2 && currentGame.gameBoard[4][6] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][3] == 2 && currentGame.gameBoard[1][4] == 2
		&& currentGame.gameBoard[2][5] == 2 && currentGame.gameBoard[3][6] == 2)
			{currentGame.player2Score++;}

	if(currentGame.gameBoard[0][3] == 2 && currentGame.gameBoard[1][2] == 2
		&& currentGame.gameBoard[2][1] == 2 && currentGame.gameBoard[3][0] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][4] == 2 && currentGame.gameBoard[1][3] == 2
		&& currentGame.gameBoard[2][2] == 2 && currentGame.gameBoard[3][1] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][3] == 2 && currentGame.gameBoard[2][2] == 2
		&& currentGame.gameBoard[3][1] == 2 && currentGame.gameBoard[4][0] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][5] == 2 && currentGame.gameBoard[1][4] == 2
		&& currentGame.gameBoard[2][3] == 2 && currentGame.gameBoard[3][2] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][4] == 2 && currentGame.gameBoard[2][3] == 2
		&& currentGame.gameBoard[3][2] == 2 && currentGame.gameBoard[4][1] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][3] == 2 && currentGame.gameBoard[3][2] == 2
		&& currentGame.gameBoard[4][1] == 2 && currentGame.gameBoard[5][0] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[0][6] == 2 && currentGame.gameBoard[1][5] == 2
		&& currentGame.gameBoard[2][4] == 2 && currentGame.gameBoard[3][3] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][5] == 2 && currentGame.gameBoard[2][4] == 2
		&& currentGame.gameBoard[3][3] == 2 && currentGame.gameBoard[4][2] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][4] == 2 && currentGame.gameBoard[3][3] == 2
		&& currentGame.gameBoard[4][2] == 2 && currentGame.gameBoard[5][1] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[1][6] == 2 && currentGame.gameBoard[2][5] == 2
		&& currentGame.gameBoard[3][4] == 2 && currentGame.gameBoard[4][3] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][5] == 2 && currentGame.gameBoard[3][4] == 2
		&& currentGame.gameBoard[4][3] == 2 && currentGame.gameBoard[5][2] == 2)
			{currentGame.player2Score++;}
	if(currentGame.gameBoard[2][6] == 2 && currentGame.gameBoard[3][5] == 2
		&& currentGame.gameBoard[4][4] == 2 && currentGame.gameBoard[5][3] == 2)
			{currentGame.player2Score++;}
}

void writeCurrentStateInFile(gameStatus &currentGame, char * fileName)
{
	currentGame.gameFile = fopen(fileName, "w");
	int i,j;
	
	if (currentGame.gameFile != 0)
	{
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < 7; j++)
			{
				fprintf (currentGame.gameFile, "%li",currentGame.gameBoard[i][j]);
			}
			fprintf (currentGame.gameFile, "\n");
		}
		if(currentGame.currentTurn == 1)
			fprintf(currentGame.gameFile, "2");
		else if (currentGame.currentTurn == 2)
			fprintf(currentGame.gameFile, "1");
		
		fclose(currentGame.gameFile);
	}
	else
		printf("Cannot open file %s\n", fileName);
}

void getCurrentStateFromFile(gameStatus &currentGame, char * fileName)
{
	currentGame.gameFile = fopen(fileName, "r");
	
	//set currentTurn
	char current = 0;
	int i, j;
	currentGame.pieceCount = 0;
	if (currentGame.gameFile != 0)
	{
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < 7; j++)
			{
				do
				{
					current = getc(currentGame.gameFile);
				}
				while ((current == ' ') || (current == '\n') || (current == '\r'));

				currentGame.gameBoard[i][j] = current - 48;

				if(currentGame.gameBoard[i][j] > 0)
				{
					currentGame.pieceCount++;
				}
			}
		}
		fclose(currentGame.gameFile);
	}
	else
		printf("Cant open file %s\n", fileName);
}

void getMoveFromHuman(gameStatus &currentGame, char *fileName)
{
	int humanMove = -1;
	int actualColNum = -1;
	int result = -1;
	printf("Choose a move: ");
	scanf("%d", &humanMove);
	fflush(stdin);
	actualColNum = humanMove-1;
	
	if((actualColNum<0) || (actualColNum>6))
	{
		printf("Invalid Move, play again\n");
		getMoveFromHuman(currentGame, fileName);
	}
	else
	{
		result = playPiece(actualColNum, currentGame, fileName);
		if(result == -1)
		{
			printf("Invalid Move, play again\n");
			getMoveFromHuman(currentGame, fileName);
		}
		else
		{
			printf("\n\nmove %li: Player %li, column %li\n",
			   currentGame.pieceCount, currentGame.currentTurn, humanMove);
			if(currentGame.currentTurn == 1)
				currentGame.currentTurn = 2;
			else if (currentGame.currentTurn == 2)
				currentGame.currentTurn = 1;
		}
	}
}

int main(int argc, char ** argv)
{

	char ** command_line = argv;
	gameStatus currentGame;	 // Declare current game
	if (argc != 5)
	{
		printf("Four command-line arguments are needed:\n");
		printf("Usage: %s interactive [input_file] [computer-next/human-next] [depth]\n", command_line [0]);
		printf("or:  %s one-move [input_file] [output_file] [depth]\n", command_line [0]);

		return 0;
	}

	char * game_mode = command_line [1];
	char *nextTurn = command_line[3];//char * output = command_line [3];
	char * fileName = command_line[2];
	char * tempDepth = command_line[4];
	
	int len = strlen(tempDepth);
	//printf("len %d\n", len);
	if(len == 1)
		currentGame.depth = (int)(tempDepth[0]-0x30);
	else if(len == 2)
		currentGame.depth = (int)( ((tempDepth[0]-0x30)*10) + (tempDepth[1]-0x30) );
 
	if (strcmp(game_mode, "interactive") == 0)
	{
		if(strcmp(nextTurn, "computer-next") == 0)
		{
			COMPUTER = 1;
			HUMAN = 2;
			currentGame.currentTurn = COMPUTER;
		}
		else if(strcmp(nextTurn, "human-next") == 0)
		{
			HUMAN = 1;
			COMPUTER = 2;
			currentGame.currentTurn = HUMAN;
		}
	
		currentGame.gameFile = fopen(fileName, "r");
		if(currentGame.gameFile!=0)
		{
			getCurrentStateFromFile(currentGame, fileName);
			fclose(currentGame.gameFile);
		}

		printf("\nMaxConnect-4 game\n");
		printf("game state before move:\n");
		printGameBoard(currentGame);
		countScore(currentGame);
		printf("Score: Player 1 = %d, Player 2 = %d\n\n", currentGame.player1Score, currentGame.player2Score);

		char humanFileExists = 0;
		//printf("Executing AI and next is %d, computer=%ld\n",currentGame.currentTurn,COMPUTER);
		while(currentGame.pieceCount != 42)
		{
			if(currentGame.currentTurn == COMPUTER)
				if(humanFileExists)
					aiPlay(currentGame, HUMAN_OUTPUT_FILE, COMP_OUTPUT_FILE);
				else
				{
					aiPlay(currentGame, fileName, COMP_OUTPUT_FILE);
					humanFileExists = 1;
				}
			else
				getMoveFromHuman(currentGame, HUMAN_OUTPUT_FILE);
			
			printGameBoard(currentGame);
			countScore(currentGame);
			printf("Score: Player 1 = %d, Player 2 = %d\n\n", currentGame.player1Score, currentGame.player2Score);
		}
		printf("Game over!\n\n");
		return 1;
	}
	else if (strcmp (game_mode, "one-move") != 0)
	{
		printf("%s is an unrecognized game mode\n", game_mode);
		return 0;
	}
  //one move game mode
	char *ouputFile = nextTurn;
	char *InputFile = fileName;
	char current = 0;
	
	//currentGame.gameFile = fopen(fileName, "r");
	//if(currentGame.gameFile!=0)
	//{
		getCurrentStateFromFile(currentGame, fileName);
		//fclose(currentGame.gameFile);
	//}

	currentGame.currentTurn = 1;
	COMPUTER = currentGame.currentTurn;
	
	printf("\nMaxConnect-4 game\n");
	printf("game state before move:\n");
	printGameBoard(currentGame);
	countScore(currentGame);
	printf("Score: Player 1 = %d, Player 2 = %d\n\n", currentGame.player1Score, currentGame.player2Score);

	if(currentGame.pieceCount == 42)
	{
		printf("\nBOARD FULL\n");
		printf("Game over!\n\n");

		return 1;
	}

	aiPlay(currentGame, InputFile, ouputFile);
	printf("game state after move:\n");
	printGameBoard(currentGame);
	countScore(currentGame);
	printf("Score: Player 1 = %d, Player 2 = %d\n\n", currentGame.player1Score, currentGame.player2Score);

	writeCurrentStateInFile(currentGame, ouputFile);
	
	return 1;
}
