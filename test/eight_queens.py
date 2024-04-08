# Function to print chessboard
def printSolution(board):
    for i in range(8):
        for j in range(8):
            print(board[i][j], end=" ")
        print()


# Function to check if a queen can be placed on board[row][col]
def isSafe(board, row, col):
    # Check this row on left side
    for i in range(col):
        if board[row][i] == 1:
            return False

    # Check upper diagonal on left side
    for i, j in zip(range(row, -1, -1), range(col, -1, -1)):
        if board[i][j] == 1:
            return False

    # Check lower diagonal on left side
    for i, j in zip(range(row, 8, 1), range(col, -1, -1)):
        if board[i][j] == 1:
            return False

    return True


# Recursive function to solve N Queen problem
def solveNQUtil(board, col):
    # base case: If all queens are placed
    # then return True
    if col >= 8:
        return True

    # Consider this column and try placing
    # this queen in all rows one by one
    for i in range(8):
        if isSafe(board, i, col):
            # Place this queen in board[i][col]
            board[i][col] = 1

            # recur to place rest of the queens
            if solveNQUtil(board, col + 1) == True:
                return True

            # If placing queen in board[i][col
            # doesn't lead to a solution, then
            # queen from board[i][col]
            board[i][col] = 0

    # if the queen can not be placed in any row in
    # this column col then return False
    return False


# Main function to solve N Queen problem
def solveNQ():
    board = [[0 for _ in range(8)] for _ in range(8)]

    if solveNQUtil(board, 0) == False:
        print("Solution does not exist")
        return False

    printSolution(board)
    return True


# Driver Code
solveNQ()

