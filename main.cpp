#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_set>

const int N = 3;
const int MAX_SIDE_STEPS = 5;

struct PuzzleState
{
    std::vector<std::vector<int>> board;
    int emptyRow, emptyCol; // Empty tile position
    int heuristic;          // Score based on the heuristic function

    PuzzleState(
        std::vector<std::vector<int>> _board,
        int _emptyRow,
        int _emptyCol)
        : board(_board), emptyRow(_emptyRow), emptyCol(_emptyCol)
    {
        calculateAndSetHeuristic();
    }

    // Calculate heuristic
    void calculateAndSetHeuristic()
    {
        int distance = 0;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (board[i][j] != 0)
                {
                    int targetRow = (board[i][j] - 1) / N;
                    int targetCol = (board[i][j] - 1) % N;
                    distance += abs(i - targetRow) + abs(j - targetCol);
                }
            }
        }
        heuristic = distance;
    }

    // Convert the board to a string for easy comparison
    std::string toString() const
    {
        std::string state = "";
        for (const auto& row : board)
        {
            for (int tile : row)
            {
                state += std::to_string(tile) + ",";
            }
        }
        return state;
    }

    void printBoard() const
    {
        std::cout << "Current Heuristic: " << heuristic << std::endl;
        for (auto row : board)
        {
            for (auto tile : row)
                std::cout << tile << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    bool isGoal()
    {
        int k = 1;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (i == N - 1 && j == N - 1) return board[i][j] == 0; // Last tile must be empty
                if (board[i][j] != k++) return false; // Check for proper placement
            }
        }
        return true;
    }

    // Swap function to move tiles
    void swapTiles(int r1, int c1, int r2, int c2)
    {
        std::swap(board[r1][c1], board[r2][c2]);
        emptyRow = r2;
        emptyCol = c2;
    }

    // Generate possible moves (up, down, left, right)
    std::vector<PuzzleState> generateMoves()
    {
        std::vector<PuzzleState> moves;
        std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (auto [dr, dc] : directions)
        {
            int newRow = emptyRow + dr;
            int newCol = emptyCol + dc;
            if (newRow >= 0 && newRow < N && newCol >= 0 && newCol < N)
            {
                PuzzleState newState = *this;
                newState.swapTiles(emptyRow, emptyCol, newRow, newCol);
                newState.calculateAndSetHeuristic();
                moves.push_back(newState);
            }
        }
        return moves;
    }
};

// Function to perform hill climbing with side stepping
PuzzleState hillClimbWithSideStepping(PuzzleState initialState)
{
    PuzzleState currentState = initialState;
    std::unordered_set<std::string> visitedStates;
    visitedStates.insert(currentState.toString());
    int sideSteps = 0;

    while (!currentState.isGoal())
    {
        currentState.printBoard();
        std::vector<PuzzleState> moves = currentState.generateMoves();

        // Remove already visited moves
        moves.erase(
            std::remove_if(moves.begin(), moves.end(), [&visitedStates](const PuzzleState& state)
                { return visitedStates.find(state.toString()) != visitedStates.end(); }),
            moves.end());

        // Sort moves by heuristic
        sort(moves.begin(), moves.end(), [](const PuzzleState& a, const PuzzleState& b)
            { return a.heuristic < b.heuristic; });

        if (moves.empty())
            break;

        // If a better move is found, take it
        if (moves[0].heuristic < currentState.heuristic)
        {
            currentState = moves[0];
            visitedStates.insert(currentState.toString());
            sideSteps = 0; // Reset side step counter when progress is made
        }
        else if (sideSteps < MAX_SIDE_STEPS)
        {
            // No better move found, allow random side stepping
            int randomIndex = rand() % moves.size();
            currentState = moves[randomIndex];
            visitedStates.insert(currentState.toString());
            sideSteps++;
        }
        else
        {
            // Can't progress further, side stepping limit reached
            break;
        }
    }

    return currentState;
}

int main()
{
    std::vector<std::vector<int>> board = {
        {1, 2, 3},
        {4, 0, 6},
        {7, 5, 8} };

    PuzzleState initialState(board, 1, 1);
    PuzzleState result = hillClimbWithSideStepping(initialState);

    if (result.isGoal())
    {
        std::cout << "Solved the puzzle!" << std::endl;
        result.printBoard();
    }
    else
    {
        std::cout << "Stuck. Could not solve the puzzle." << std::endl;
        result.printBoard();
    }

    return 0;
}