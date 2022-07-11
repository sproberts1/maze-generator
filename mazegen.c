/*************************************/
/*Spencer Roberts                    */
/*                                   */
/*This file creates a maze and       */
/* solves it                         */
/*************************************/


#include <stdio.h>
#include <stdlib.h>
#include "mazegen.h"
 
int numRows, numCols, startPoint, endPoint, randomDir, newRandomDir, wayPointx, wayPointy;
int wayPointAlleylength, printAlgorithmsteps;
double straightprobability, wayPointDirectionpercent;
unsigned char **maze;
//used to store free directions for a given position
int freeDirections[TOTAL_DIRECTIONS + 1];
//used as a flag if there are available directions to go (1) or not (0)
int isFreeDir = 1;
//used as a flag if the maze is full (1) or not (0)
int isFull = 0;
//used as a mask to only get the color bits of the maze
int colorMask = 48;
//used as a flag to set the color if solveMaze was called
int calledSolveMaze = 0;
//used as a mask to only get the directions of the maze
int directionMask = 15;
//used as a flag if the maze has been solved
int solvedMaze = 0;
//used to store the directions possible to move while solving for a given position
int freeSolveDirections[TOTAL_DIRECTIONS + 1];
//used as a flag if there are available directions to move (1) or not (0) while solving 
int isFreeSolveDir = 1;

/*************************************/
/* This function checks to see if    */
/*  the maze is full, if it is       */
/*  it will set the flag to true     */
/*  if not, it remains false         */
/*************************************/
void isMazeFull()
{
    for(int row = 0; row < numRows; row++)
    {
        for(int col = 0; col < numCols; col++)
        {
            //if there is a position in the maze that is 0, the maze is not full
            if(maze[row][col] == 0)
            {
                isFull = 0;
                return;
            }
            else
            {
                //if there are no positions in the maze that are equal to 0, the maze is full
                isFull = 1;
            }
        }
    }
}

/*************************************/
/*This function prints out the maze  */
/* in the given format, per the      */
/* slides                            */
/*************************************/
void mazePrint()
{
    textcolor(TEXTCOLOR_WHITE);
    printf("\n\n");
    printf("========================\n");
    printf("Maze(%d x %d): (%d, %d)\n", numCols, numRows, wayPointx, wayPointy);
    for(int row = 0; row < numRows; row++)
    {
        for(int col = 0; col < numCols; col++)
        {
            //print maze
            //printf("printing maze\n");
            int mazeValue = maze[col][row];
            //printf("mazeValue = %d\n", mazeValue);
            if(col == (wayPointx - 1) && row == (wayPointy - 1))
            {
                textcolor(TEXTCOLOR_RED);
            }
            else if((mazeValue & colorMask) == 16)
            {
                textcolor(TEXTCOLOR_GREEN);
            }
            else
            {
                textcolor(TEXTCOLOR_WHITE);
            }
            printf("%c", pipeList[mazeValue & directionMask]); //pipeList[mazeValue]);//maze[col][row]); //pipeList[mazeValue]); //maze[col][row]); //pipeList[mazeValue]);
        }
        printf("\n");
    }
    textcolor(TEXTCOLOR_WHITE);
    printf("========================\n");
}

/*************************************/
/*This function fills the maze with  */
/* all zeros                         */
/*************************************/
void setMazeEmpty()
{
    for(int row = 0; row < numRows; row++)
    {
        for(int col = 0; col < numCols; col++)
        {
            //initialize every position to 0
            maze[col][row] = 0;
        }
    } 
}

/***********************************************/
/*int x is the current x position in the maze  */
/*int y is the current y position in the maze  */
/*                                             */
/*This function checks if the current          */
/* position in the maze has any direction to   */
/* move                                        */
/***********************************************/
void checkFreeDir(int x, int y)
{
    //sets all of the elements in freeDirections[] to zero
    for(int j = 0; j < (sizeof(freeDirections)/sizeof(int)); j++)
    {
        freeDirections[j] = 0;
    }
    //used to index the freeDirections array
    int freeDirIndex = 0;
    //checks if there is an empty space to the right of the 
    // current space 
    if((x+1) < numCols && (x+1) >=0)
    {
        if(maze[x+1][y] == 0)
        {
            freeDirections[freeDirIndex] = 2;
            freeDirIndex++;
            isFreeDir = 1;    
        }
    }
    //checks if there is an empty space to the left of the 
    // current space
    if((x-1) >= 0 && (x-1) < numCols)
    {
        if(maze[x-1][y] == 0)
        {
            freeDirections[freeDirIndex] = 8;
            freeDirIndex++;
            isFreeDir = 1;
        }
    }
    //checks if there is an empty space above the current
    // space
    if((y+1) < numRows && (y+1) >= 0)
    {
        if(maze[x][y+1] == 0)
        {
            freeDirections[freeDirIndex] = 4;
            freeDirIndex++;
            isFreeDir = 1;  
        }
    }
    //checks if there is an empty space below the current 
    // space
    if((y-1) >= 0 && (y-1) < numRows)
    {
        if(maze[x][y-1] == 0)
        {
            freeDirections[freeDirIndex] = 1;
            freeDirIndex++;
            isFreeDir = 1;
        }
    }
    //there is no open direction
    if(freeDirIndex == 0)
    {
        //set the flag to false
        isFreeDir = 0;
    }
    //add the terminating symbol to the end of the array
    freeDirections[freeDirIndex] = '\0';
}

/***********************************************/
/*int x is the current x position in the maze  */
/*int y is the current y position in the maze  */
/*int dir is the direction to move in          */
/*This function will move into the given       */
/* position in the maze, and recursively calls */
/* itself until the maze is full               */
/***********************************************/
void carveMaze(int x, int y, int dir)
{
    //checks if the maze is not full
    if(!isFull)
    {
        //goingDownRecursion = 0;
        //checks if the current x and y are wthin the bounds of the maze and 
        // makes sure there is a free direction to move
        if(x >= 0 && x < numCols && y >=0 && y < numRows && isFreeDir == 1)
        {
            //checks if the current position is 0 (has not been moved into)
            if(maze[x][y] == 0)
            {
                //if so, add that direction to the current position in the maze
                maze[x][y] += dir;
            }
            //check free directions for the given postion in the maze
            checkFreeDir(x, y);
            //pick a random number from the available directions to move
            randomDir = freeDirections[rand() % 4];
            while(randomDir == 0 && isFreeDir)
            {
                //if the random direction is 0 (that is not a valid direction), so 
                // pick a valid direction (1, 2, 4, or 8)
                randomDir = freeDirections[rand() % 4];
            }
            //add that random direction to the current position in the maze
            // (which creates openings)
            maze[x][y] += randomDir;
            //based on the random direction, move in the opposite way, so if the 
            // random direction was to move down, the new position in the maze 
            // will have a opening at the top
            switch(randomDir)
            {
                case 1:
                    carveMaze(x, (y-1), 4);
                    break;
                case 2:
                    carveMaze((x+1), y, 8);
                    break;
                case 4:
                    carveMaze(x, (y+1), 1);
                    break;
                case 8:
                    carveMaze((x-1), y, 2);
                    break;
            }
            
        }
        //checks if there is not a free direction to move
        if(!isFreeDir)
        {
            //check if there is an open direction
            checkFreeDir(x, y);
            //checks if there is now an open direction
            if(isFreeDir == 1)
            {
                //if so, pick a random direction to move
                randomDir = freeDirections[rand() % 4];
                while(randomDir == 0 && isFreeDir)
                {
                    randomDir = freeDirections[rand() % 4];
                }
                //move in that direction
                carveMaze(x, y, randomDir);
            }
        }
    }
    //printf("exiting carveMaze\n");
}

/**************************************************/
/*int width is the number of columns in the maze  */
/*int height is the number of rows in the maze    */
/*all other parameters are not used yet           */
/*This function allocates memory for the maze,    */
/* sets the number of rows and columns,           */
/* and randomly picks an entry and exit point     */
/***************************************************/
int mazeGenerate(int width, int height, int wayPointX, int wayPointY,
                 int wayPointAlleyLength, double wayPointDirectionPercent,
                 double straightProbability, int printAlgorithmSteps)
{
    if(width < 3 || width > 1000 || height < 3 || height > 1000) 
    {
        printf("error\n");
        return 1;
    }
    //free the last maze from memory
    mazeFree();
    //reset the global variables
    isFreeDir = 1;
    isFull = 0;
    calledSolveMaze = 0;
    solvedMaze = 0;
    isFreeSolveDir = 1;

    //set number of rows and columns based on parameters
    numRows = height;
    numCols = width;
    wayPointx = wayPointX;
    wayPointy = wayPointY;
    if(wayPointX < 1 || wayPointX > numCols || wayPointY < 1 || wayPointY > numRows)
    {   
        printf("error\n");
        return 1;
    }
    //allocates memory based on given dimensions
    maze = malloc(numCols * sizeof(*maze));
    for(int i = 0; i < numCols; i++)
    {
        maze[i] = malloc(numRows * sizeof(int));
    }
    //fills maze with all zeros
    setMazeEmpty(maze);
    //creates a random starting point for the maze
    startPoint = rand() % numCols;
    //creates a random ending point for the maze
    endPoint = rand() % numCols;
    //checkFreeDir(startPoint, 0);
    //int dir = freeDirections[0];
    carveMaze(startPoint, 0, 1);
    //makes the starting point have an opening at the top
    //maze[startPoint][0] += 1;
    //makes the end point have an opening at the bottom
    maze[endPoint][numRows-1] += 4;
    //printf("end of maze generate\n");
}

/***********************************************/
/*int x is the current x position in the maze  */
/*int y is the current y position in the maze  */
/*                                             */
/*This function checks if the current          */
/* position in the maze has any direction to   */
/* move while solving it (i.e if the above     */
/* cell has an opening at the bottom)          */
/***********************************************/
void checkFreeSolve(int x, int y)
{
    //checks if the exit has been found, if so the maze is solved
    if(x == endPoint && y == numRows - 1)
    {
        solvedMaze = 1;
        return;
    }
    //sets all of the elements in freeSolveDirections array to zero
    for(int j = 0; j < (sizeof(freeSolveDirections)/sizeof(int)); j++)
    {   
        freeSolveDirections[j] = 0;
    }
    //used to index the freeSolveDirections array
    int freeDirIndex = 0;
    //checks if the cell above the current one has an opening at the bottom
    if((y-1) >= 0)
    {
        if((maze[x][y-1] & 4) == 4 && (maze[x][y-1] & 48) == 0)
        {
            freeSolveDirections[freeDirIndex] = 1;
            freeDirIndex++;
            isFreeSolveDir = 1;
        }
    }
    //checks if the cell to the right of the current one has an opening to the left
    if((x+1) < numCols)
    {
        if((maze[x+1][y] & 8) == 8 && (maze[x+1][y] & 48) == 0)
        {
            freeSolveDirections[freeDirIndex] = 2;
            freeDirIndex++;
            isFreeSolveDir = 1;
        }
    }
    //checks if the cell below the current one has an opening at the top
    if((y+1) < numRows)
    {
        if((maze[x][y+1] & 1) == 1 && (maze[x][y+1] & 48) == 0)
        {
            freeSolveDirections[freeDirIndex] = 4;
            freeDirIndex++;
            isFreeSolveDir = 1;
        }
    }
    //checks if the cell to the left of the current one has an opening to the right
    if((x-1) >= 0)
    {
        if((maze[x-1][y] & 2) == 2 && (maze[x-1][y] & 48) == 0)
        {
            freeSolveDirections[freeDirIndex] = 8;
            freeDirIndex++;
            isFreeSolveDir = 1;
        }
    }
    //if there is no open direction to move
    if(freeDirIndex == 0)
    {
        //set the flag
        isFreeSolveDir = 0;
    }
}

/***********************************************/
/*int x is the current x position in the maze  */
/*int y is the current y position in the maze  */
/*int dir is the direction to move in          */
/*                                             */
/*This function will solve the maze that is    */
/* carved                                      */
/***********************************************/
void mazeSolver(int x, int y, int dir)
{
    if(!solvedMaze)
    {   
        //checks if the given x and y are within the maze, is unvisited, and there is a direction to move 
        if(x >= 0 && x < numCols && y >=0 && y < numRows && (maze[x][y] & 48) == 0 && isFreeSolveDir == 1) 
        {
            //if so, add it to the solution
            maze[x][y] += 16; 
        }
        //see if there is an unvisited cell that it can move into
        checkFreeSolve(x, y);
        //set the direction to move
        newRandomDir = freeSolveDirections[0];
        //call based on the unvisited cell direction
        switch(newRandomDir)
        {
            case 1:
                mazeSolver(x, (y-1), 4);
                break;
            case 2:
                mazeSolver((x+1), y, 8);
                break;
            case 4:
                mazeSolver(x, (y+1), 1);
                break;
            case 8:
                mazeSolver((x-1), y, 2);
                break;

       }
        if(!isFreeSolveDir)
        {
            //add 16 to make 32, which is the bit I use to tell if a cell has been visited
            maze[x][y] += 16;
            //check if there is an unvisited cell to move into
            checkFreeSolve(x,y);
            //if there is an empty cell to move into
            if(isFreeSolveDir)
            {
                //subtract 16 to make 16, which is the bit I use to tell if a cell is part of the solution
                maze[x][y] -= 16;
                //move in that direction
                mazeSolver(x, y, freeSolveDirections[0]);
            }
        } 
    }
}

/*************************************/
/*This function calls mazeSolver     */
/* when the maze needs to be solved  */
/*************************************/
void mazeSolve()
{
    //set the flag that this has been called
    calledSolveMaze = 1;
    //checks for free direcions to move into from the way point
    checkFreeSolve(startPoint, 0);
    //choose the first open direction
    int dir = freeSolveDirections[0];
    //start solving the maze from the way point
    mazeSolver(startPoint, 0, dir);
}

/*************************************/
/*This function frees the memory     */
/* that was allocated for the maze   */
/*************************************/
void mazeFree()
{
    //printf("calling mazeFree\n");
    if(maze != NULL)
    {
        for(int i = 0; i < numCols; i++)
        {
            free(maze[i]);
        }
        free(maze);
    }
    maze = NULL;
}
