-----------------------
Chaiatanya Sardesai
-----------------------

----------------------
Programming Language : 
----------------------
	C++ with gcc compiler. (g++ -o <file_name.out> <file_name>)
	
-------------------
Package Structure :
-------------------
	This project contains src folder
	
	src folder has 1 package app
	maxconnect4 pakage contains maxconnect4.cpp file, which is main file of application
	Path:: ..\src\app\maxconnect4\maxconnect4.cpp
	
	Note: keep input file (initial board state file) at above location path.

---------------
Code Structure:
---------------
	It will call aiPlay which is main function for AI, internally call alpha -beta pruning search algorithm routine.
	min function will be call for all possible moves available at start, then maximum of returned values by Min will be played as a final move.
	
--------------------
Evaluation Function:
--------------------
	Evaluation Function:
	If we look at board, we can infer that middle positions are most strategic position, as from middle there are many possible ways to get 4 adjacent positions and in turn points.
	So we can define a table in which there are values given to each position as per total points that position can achieve:
	3,  4,   5,  7,    5,  4,  3 
	4,  6,   8,  10,   8,  6,  4
	5,  8,  11,  12,  11,  8,  5
	5,  8,  11,  12,  11,  8,  5
	4,  6,   8,  10,   8,  6,  4
	3,  4,   5,  7,    5,  4,  3
	Total points will be 276, hence individual can achieve 138. So at terminal node we can calculate total points we achieved till that point and return that value. 
	Addition to it, in case if opponent have earned some points, we will decrement this evaluated value. In case we have earned some points and our points are higher than opponents we will add these in our evaluated value.
	This will help us to avoid corner positions and not getting any points.

	

------------------------
Running the Application:
------------------------ 
	1. Compile file.
	2. Copy input text file at appropriate location given above.
	3. Run compiled file with appropriate depth.
	