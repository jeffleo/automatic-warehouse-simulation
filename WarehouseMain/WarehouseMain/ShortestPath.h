#pragma once
#ifndef SHORTESTPATH_H
#define SHORTESTPATH_H

#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"
#include <stdlib.h>
#include <array>

/// <summary>
/// Summary description for MazeSolver.
/// URL: Programming Home "http://www.geocities.com/smehrozalam/"
/// 
/// Constructors:
/// 	( int[,] ):	takes 2D integer array	
/// 	( int Rows, int Cols )	initializes the dimensions, indexers may be used 
/// 							to set individual elements' values
/// 
/// Properties:
/// 	Rows: returns the no. of rows in the current maze
/// 	Cols: returns the no. of columns in the current maze
/// 	Maze: returns the current maze as a 2D array
/// 	PathCharacter: to get/set the value of path tracing character
/// 	AllowDiagonal: whether diagonal paths are allowed
/// 
/// Indexers:
/// 	[i,j] = used to set/get elements of maze
/// 
/// Public Methods (Description is given with respective methods' definitions)
///		int[,] FindPath(int iFromY, int iFromX, int iToY, int iToX)
/// 
/// Private Methods
///		void GetNodeContents(int[,] iMaze, int iNodeNo)
///		void ChangeNodeContents(int[,] iMaze, int iNodeNo, int iNewValue)
///		int[,] Search(int iBeginningNode, int iEndingNode)
/// 
/// </summary>

class MazeSolver
{

	/// <summary>
	/// Class attributes/members
	/// </summary>
	int m_iMaze[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];
	//std::array<std::array<int, MAX_LAYOUT_SIZE>, MAX_LAYOUT_SIZE> m_iMaze;
	int m_iRows;
	int m_iCols;
	bool diagonal = false;


	/// <summary>
	/// Constructor 1: takes a 2D integer array
	/// </summary>

public :
	int iMazeSolved[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];
	//int iPath = 254;									// FILLS found location with this number,
	int iPath = 10;

	//template <size_t rows, size_t cols>
	//MazeSolver(int (&iMaze)[rows][cols], int rows, int cols)  : m_iRows(rows), m_iCols(cols)
	MazeSolver(int rows, int cols, int memoryindex) : m_iRows(rows), m_iCols(cols)
	{
		//cpen333::process::mutex mutex_(WAREHOUSE_MUTEX_NAME);
		//cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
		//SharedData* shared = (SharedData*)memory.get();;	// get pointer to shared memory

		SharedData* shared = shares.at(memoryIndex);
		cpen333::process::mutex mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryIndex));

		for (int i = 0; i < MAX_LAYOUT_SIZE; ++i) {
			for ( int j = 0; j < MAX_LAYOUT_SIZE; ++j) {
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				//m_iMaze[i][j] = shared->layinfo.access[i][j];	// copy robot accessibility over
				m_iMaze[i][j] = shared->layinfo.access[j][i];	// copy robot accessibility over	//FLIPPED ROWS WITH COLUMNS
			}
		}


	}

	/// <summary>
	/// The function is used to get the contents of a given node in a given maze,
	///  specified by its node no.
	/// </summary>
	int GetNodeContents(int maze[][MAX_LAYOUT_SIZE], int iNodeNo)
	//int GetNodeContents(std::array<std::array<int, MAX_LAYOUT_SIZE>, MAX_LAYOUT_SIZE> maze, int iNodeNo)
	{
	//FIXME	?
		//int iColslength = maze.size();
		return maze[iNodeNo / m_iCols][ iNodeNo - iNodeNo / m_iCols*m_iCols];
		//return maze.at(iNodeNo / m_iCols).at(iNodeNo - iNodeNo / m_iCols*m_iCols);
	}

	/// <summary>
	/// The function is used to change the contents of a given node in a given maze,
	///  specified by its node no.
	/// </summary>
	void ChangeNodeContents(int maze[][MAX_LAYOUT_SIZE], int iNodeNo, int iNewValue)
	//void ChangeNodeContents(int maze[][MAX_LAYOUT_SIZE], int iNodeNo, int iNewValue)
	{
		//FIXME
		maze[iNodeNo / m_iCols][ iNodeNo - iNodeNo / m_iCols*m_iCols] = iNewValue;
	}

	/// <summary>
	/// This public function finds the shortest path between two points
	/// in the maze and return the solution as an array with the path traced 
	/// by "iPath" (can be changed using property "PathCharacter")
	/// if no path exists, the function returns null
	/// </summary>
	bool FindPath(int iFromY, int iFromX, int iToY, int iToX)
	{
		int iBeginningNode = iFromY*m_iCols + iFromX;
		int iEndingNode = iToY*m_iCols + iToX;
		return (Search(iBeginningNode, iEndingNode));
	}


	/// <summary>
	/// Internal function for that finds the shortest path using a technique
	/// similar to breadth-first search.
	/// It assigns a node no. to each node(2D array element) and applies the algorithm
	/// </summary>
	struct Status
	{
		int Ready = 0;
		int Waiting = 1;
		int Processed = 2;
	} status;

	bool Search(int iStart, int iStop)
	{
		const int empty = 0;		// change to string ' ' later

		int iRows = m_iRows;
		int iCols = m_iCols;
		int iMax = iRows*iCols;
		//std::vector<int> Q;
		//std::vector<int> Origin;


		int* Queue = new int[iMax];
		int* Origin = new int[iMax];

		int iFront = 0, iRear = 0;

		//check if starting and ending points are valid (open)
		if (GetNodeContents(m_iMaze, iStart) != empty || GetNodeContents(m_iMaze, iStop) != empty)
		{
			//std::array<std::array<int, MAX_LAYOUT_SIZE>, MAX_LAYOUT_SIZE> empty_array{};


			//return 0;
			return false;
		}

		//create dummy array for storing status
		//int** iMazeStatus = new int*[iRows];	
		//for (int i = 0; i < iRows; i++) {
		//	iMazeStatus[i] = new int[iCols];
		//}

		int iMazeStatus[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];


		//initially all nodes are ready
		for (int i = 0; i<iRows; i++)
			for (int j = 0; j<iCols; j++)
				iMazeStatus[i][j] = status.Ready;

		//add starting node to Q
		Queue[iRear] = iStart;
		Origin[iRear] = -1;

		iRear++;
		int iCurrent, iLeft, iRight, iTop, iDown, iRightUp, iRightDown, iLeftUp, iLeftDown;

		while (iFront != iRear)	// while Q not empty	
		{
			if (Queue[iFront] == iStop)		// maze is solved
				break;

			iCurrent = Queue[iFront];

			iLeft = iCurrent - 1;
			if (iLeft >= 0 && iLeft / iCols == iCurrent / iCols) 	//if left node exists
				if (GetNodeContents(m_iMaze, iLeft) == empty) 	//if left node is open(a path exists)
					if (GetNodeContents(iMazeStatus, iLeft) == (int)status.Ready)	//if left node is ready
					{
						Queue[iRear] = iLeft; //add to Q
						Origin[iRear] = iCurrent;
						ChangeNodeContents(iMazeStatus, iLeft, (int)status.Waiting); //change status to waiting
						iRear++;
					}

			iRight = iCurrent + 1;
			if (iRight<iMax && iRight / iCols == iCurrent / iCols) 	//if right node exists
				if (GetNodeContents(m_iMaze, iRight) == empty) 	//if right node is open(a path exists)
					if (GetNodeContents(iMazeStatus, iRight) == (int)status.Ready)	//if right node is ready
					{
						Queue[iRear] = iRight; //add to Q
						Origin[iRear] = iCurrent;
						ChangeNodeContents(iMazeStatus, iRight, (int)status.Waiting); //change status to waiting
						iRear++;
					}

			iTop = iCurrent - iCols;
			if (iTop >= 0) 	//if top node exists
				if (GetNodeContents(m_iMaze, iTop) == empty) 	//if top node is open(a path exists)
					if (GetNodeContents(iMazeStatus, iTop) == (int)status.Ready)	//if top node is ready
					{
						Queue[iRear] = iTop; //add to Q
						Origin[iRear] = iCurrent;
						ChangeNodeContents(iMazeStatus, iTop, (int)status.Waiting); //change status to waiting
						iRear++;
					}

			iDown = iCurrent + iCols;
			if (iDown<iMax) 	//if bottom node exists
				if (GetNodeContents(m_iMaze, iDown) == empty) 	//if bottom node is open(a path exists)
					if (GetNodeContents(iMazeStatus, iDown) == (int)status.Ready)	//if bottom node is ready
					{
						Queue[iRear] = iDown; //add to Q
						Origin[iRear] = iCurrent;
						ChangeNodeContents(iMazeStatus, iDown, (int)status.Waiting); //change status to waiting
						iRear++;
					}
			if (diagonal == true)
			{
				iRightDown = iCurrent + iCols + 1;
				if (iRightDown<iMax && iRightDown >= 0 && iRightDown / iCols == iCurrent / iCols + 1) 	//if bottom-right node exists
					if (GetNodeContents(m_iMaze, iRightDown) == empty) 	//if this node is open(a path exists)
						if (GetNodeContents(iMazeStatus, iRightDown) == (int)status.Ready)	//if this node is ready
						{
							Queue[iRear] = iRightDown; //add to Q
							Origin[iRear] = iCurrent;
							ChangeNodeContents(iMazeStatus, iRightDown, (int)status.Waiting); //change status to waiting
							iRear++;
						}

				iRightUp = iCurrent - iCols + 1;
				if (iRightUp >= 0 && iRightUp<iMax && iRightUp / iCols == iCurrent / iCols - 1) 	//if upper-right node exists
					if (GetNodeContents(m_iMaze, iRightUp) == empty) 	//if this node is open(a path exists)
						if (GetNodeContents(iMazeStatus, iRightUp) == (int)status.Ready)	//if this node is ready
						{
							Queue[iRear] = iRightUp; //add to Q
							Origin[iRear] = iCurrent;
							ChangeNodeContents(iMazeStatus, iRightUp, (int)status.Waiting); //change status to waiting
							iRear++;
						}

				iLeftDown = iCurrent + iCols - 1;
				if (iLeftDown<iMax && iLeftDown >= 0 && iLeftDown / iCols == iCurrent / iCols + 1) 	//if bottom-left node exists
					if (GetNodeContents(m_iMaze, iLeftDown) == empty) 	//if this node is open(a path exists)
						if (GetNodeContents(iMazeStatus, iLeftDown) == (int)status.Ready)	//if this node is ready
						{
							Queue[iRear] = iLeftDown; //add to Q
							Origin[iRear] = iCurrent;
							ChangeNodeContents(iMazeStatus, iLeftDown, (int)status.Waiting); //change status to waiting
							iRear++;
						}

				iLeftUp = iCurrent - iCols - 1;
				if (iLeftUp >= 0 && iLeftUp<iMax && iLeftUp / iCols == iCurrent / iCols - 1) 	//if upper-left node exists
					if (GetNodeContents(m_iMaze, iLeftUp) == empty) 	//if this node is open(a path exists)
						if (GetNodeContents(iMazeStatus, iLeftUp) == (int)status.Ready)	//if this node is ready
						{
							Queue[iRear] = iLeftUp; //add to Q
							Origin[iRear] = iCurrent;
							ChangeNodeContents(iMazeStatus, iLeftUp, (int)status.Waiting); //change status to waiting
							iRear++;
						}
			}


			//change status of current node to processed
			ChangeNodeContents(iMazeStatus, iCurrent, (int)status.Processed);
			iFront++;

		}

		//create an array(maze) for solution
		/*int iMazeSolved[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];*/

		for (int i = 0; i<iRows; i++)
			for (int j = 0; j<iCols; j++)
				iMazeSolved[i][j] = m_iMaze[i][j];

		//make a path in the Solved Maze
		iCurrent = iStop;
		ChangeNodeContents(iMazeSolved, iCurrent, iPath++);
		for (int i = iFront; i >= 0; i--)
		{
			if (Queue[i] == iCurrent)
			{
				iCurrent = Origin[i];
				if (iCurrent == -1)		// maze is solved
					//return iMazeSolved;
					return true;
				ChangeNodeContents(iMazeSolved, iCurrent, iPath++);
			}
		}

		//no path exists
		//return null;
		return false;
	}
};




#endif //SHORTESTPATH_H