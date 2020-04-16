/** Mancala AI
*   Uses Advanced Heuristic Minimax with Alpha Beta Pruning
*/

#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <bitset>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <climits>

using namespace std;

const int NUMBER_OF_HOLES = 6;
const int NUMBER_OF_PREDICTED_TURNS = 10;
const int TOTAL_NUMBER_OF_MARBLES = 48;

// Weights for heuristics
// Note: Since the evaluate() function returns an int, we keep the weights as int
const int SCORE_WEIGHT = 3;    // difference in score
const int PLAYER_MARBLES_WEIGHT = 1;    // maximize marbles in player's holes
const int OPPONENT_MARBLES_WEIGHT = -1;   // maximize marbles in opponent's holes

struct Board
{
    int maxPlayerMancala;
    int minPlayerMancala;
    int maxPlayerHoles[NUMBER_OF_HOLES];
    int minPlayerHoles[NUMBER_OF_HOLES];
};

/**
 * Updates the Board State based on the given move.
 *
 * @return updated board state.
 */
Board updateBoard(Board currentBoardState, bool maximizingPlayer, int holeIndex)
{
    Board newBoardState;

    // Initialize newBoardState to currentBoardState
    newBoardState.maxPlayerMancala = currentBoardState.maxPlayerMancala;
    newBoardState.minPlayerMancala = currentBoardState.minPlayerMancala;

    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        newBoardState.maxPlayerHoles[i] = currentBoardState.maxPlayerHoles[i];
        newBoardState.minPlayerHoles[i] = currentBoardState.minPlayerHoles[i];
    }

    // Update newBoardState based on current move
    int marbles = 0;

    if (maximizingPlayer)
    {
        // Pick up marble and empty chosen hole.
        marbles = newBoardState.maxPlayerHoles[holeIndex];
        newBoardState.maxPlayerHoles[holeIndex] = 0;
        holeIndex++;

        while (marbles > 0)
        {
            // Step 1. Fill own holes till it reaches mancala or no marbles remain.
            while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
            {
                // Check for stealing.
                // If last marble is dropped in empty hole on player's side, steal opponent's marbles in opposite hole.
                if (marbles == 1 && holeIndex < NUMBER_OF_HOLES && newBoardState.maxPlayerHoles[holeIndex] == 0)
                {
                    newBoardState.maxPlayerHoles[holeIndex] += newBoardState.minPlayerHoles[holeIndex];
                    newBoardState.minPlayerHoles[holeIndex] = 0;
                }

                newBoardState.maxPlayerHoles[holeIndex]++;
                holeIndex++;
                marbles--;
            }


            if (marbles <= 0)
            {
                break;
            }

            // Step 2. Fill own mancala
            newBoardState.maxPlayerMancala++;
            marbles--;

            if (marbles <= 0)
            {
                break;
            }

            // Step 3. Fill enemy holes till it reaches mancala
            holeIndex = 0;

            while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
            {
                newBoardState.minPlayerHoles[holeIndex]++;
                holeIndex++;
                marbles--;
            }
        }
    }
    else
    {
        // Pick up marble and empty chosen hole.
        marbles = newBoardState.minPlayerHoles[holeIndex];
        newBoardState.minPlayerHoles[holeIndex] = 0;
        holeIndex++;

        while (marbles > 0)
        {
            // 1. Fill own holes till it reaches mancala or no marbles remain.
            while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
            {
                // Check for stealing.
                // If last marble is dropped in empty hole on player's side, steal opponent's marbles in opposite hole.
                if (marbles == 1 && holeIndex < NUMBER_OF_HOLES && newBoardState.minPlayerHoles[holeIndex] == 0)
                {
                    newBoardState.minPlayerHoles[holeIndex] = newBoardState.maxPlayerHoles[holeIndex];
                    newBoardState.maxPlayerHoles[holeIndex] = 0;
                }

                newBoardState.minPlayerHoles[holeIndex]++;
                holeIndex++;
                marbles--;
            }


            if (marbles <= 0)
            {
                break;
            }

            // 2. Fill own mancala
            newBoardState.minPlayerMancala++;
            marbles--;

            if (marbles <= 0)
            {
                break;
            }

            // 3. Fill enemy holes till it reaches mancala
            holeIndex = 0;

            while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
            {
                newBoardState.maxPlayerHoles[holeIndex]++;
                holeIndex++;
                marbles--;
            }
        }
    }

    return newBoardState;
}

/**
 * Checks to see if game is over based on given board state.
 *
 * @return boolean indicating if game is over.
 */
bool isGameOver(Board boardState)
{
    bool isMaximizingPlayerHolesEmpty = true;
    bool isMinimizingPlayerHolesEmpty = true;
    bool isGameOver = false;

    // Check if max player holes are empty
    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        if (boardState.maxPlayerHoles[i] != 0)
        {
            isMaximizingPlayerHolesEmpty = false;
            break;
        }
    }

    // Check if min player holes are empty
    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        if (boardState.minPlayerHoles[i] != 0)
        {
            isMinimizingPlayerHolesEmpty = false;
            break;
        }
    }

    // Game ends when either player holes are empty
    isGameOver = (isMaximizingPlayerHolesEmpty || isMinimizingPlayerHolesEmpty);

    if (isGameOver)
    {
        // Update board for game over state
        // Move marbles in remaining holes to player mancala
        for (int i = 0; i < NUMBER_OF_HOLES; i++)
        {
            if (boardState.maxPlayerHoles[i] > 0)
            {
                boardState.maxPlayerMancala += boardState.maxPlayerHoles[i];
                boardState.maxPlayerHoles[i] = 0;
            }

            if (boardState.minPlayerHoles[i] > 0)
            {
                boardState.minPlayerMancala += boardState.minPlayerHoles[i];
                boardState.minPlayerHoles[i] = 0;
            }
        }
    }

    return isGameOver;
}

/**
 * Checks to see if moves result in another turn.
 */
bool isFreeTurn(int holeIndex, int marbles)
{
    holeIndex++;

    while (marbles > 0)
    {
        // Step 1. Fill holes until we hit player's mancala.
        while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
        {
            holeIndex++;
            marbles--;
        }

        if (marbles <= 0)
        {
            break;
        }

        // Step 2. Fill player's mancala. If this is the last marble, then it is a free turn.
        if (marbles == 1)
        {
            return true;
        }

        marbles--;
        holeIndex = 0;

        // Step 3. Fill opponent's holes until we hit player's mancala.
        while (holeIndex < NUMBER_OF_HOLES && marbles > 0)
        {
            holeIndex++;
            marbles--;
        }

        holeIndex = 0;
    }

    return false;
}

/**
 * Gets the total number of marbles in the entire game (including holes and mancalas).
 *
 * @return total number of marbles in the game.
 */
int getTotalNumberOfMarbles(Board boardState)
{
    int marbles = 0;

    // Get marbles in holes
    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        marbles += boardState.maxPlayerHoles[i];
        marbles += boardState.minPlayerHoles[i];
    }

    // Get marbles in mancala
    marbles += boardState.maxPlayerMancala;
    marbles += boardState.minPlayerMancala;

    return marbles;
}

/**
 * Gives an evaluation on how "good" the move is based on several heuristics.
 *
 * Heuristic 1: Favour a larger difference in the score.
 * Heuristic 2: Maximize number of marbles in player's holes.
 * Heuristic 3: Minimize number of marbles in opponent's holes.
 *
 * @return evaluation/rating of the move.
 */
int evaluate(Board boardState, bool maximizingPlayer, int depth)
{
    // Heuristic 1: Favour a larger difference in the score.
    int score = boardState.maxPlayerMancala - boardState.minPlayerMancala;

    // Heuristic 2: Maximize number of marbles in player's holes.
    // Heuristic 3: Minimize number of marbles in opponent's holes.
    int playerMarbles = 0;
    int opponentMarbles = 0;

    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        if (maximizingPlayer)
        {
            playerMarbles += boardState.maxPlayerHoles[i];
            opponentMarbles += boardState.minPlayerHoles[i];
        }
        else
        {
            playerMarbles += boardState.minPlayerHoles[i];
            opponentMarbles += boardState.maxPlayerHoles[i];
        }
    }

    // Return the evaluation based on the heuristics
    if ((playerMarbles > opponentMarbles&& isGameOver(boardState)) ||
        boardState.maxPlayerMancala > (TOTAL_NUMBER_OF_MARBLES / 2))
    {
        return (1000 * depth);
    }

    if ((opponentMarbles < playerMarbles && isGameOver(boardState)) ||
        boardState.minPlayerMancala > (TOTAL_NUMBER_OF_MARBLES / 2))
    {
        return (-1000 * -depth);
    }

    if (!maximizingPlayer)
    {
        playerMarbles = -playerMarbles;
        opponentMarbles = -opponentMarbles;
    }

    return ((score * SCORE_WEIGHT) +
        (playerMarbles * PLAYER_MARBLES_WEIGHT) +
        (opponentMarbles * OPPONENT_MARBLES_WEIGHT));
}

/**
 * Uses Minimax Algorithm with Alpha Beta Pruning.
 * This algorithm is used to determine the best possible next move that can be performed.
 *
 * If this is the root node:
 * @return The index of the hole determined to be the best next move.
 *
 * If this is not the root node:
 * @return The evaluation of the move based on how "good" the move is.
 */
int minimax(Board currentBoardState, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (isGameOver(currentBoardState) || depth == 0)
    {
        return evaluate(currentBoardState, maximizingPlayer, depth);
    }

    if (maximizingPlayer)
    {
        // MaxEval gives us a measurement of the optimal choice
        int maxEval = INT_MIN;
        int maxEvalIndex = -1;

        // Loops through all of the holes (possible moves)
        for (int i = 0; i < NUMBER_OF_HOLES; i++)
        {
            // Only checks valid moves (hole has marbles in it)
            if (currentBoardState.maxPlayerHoles[i] > 0)
            {
                Board newBoardState = updateBoard(currentBoardState, maximizingPlayer, i);
                bool freeTurn = isFreeTurn(i, currentBoardState.maxPlayerHoles[i]);

                int eval = minimax(newBoardState, depth - 1, alpha, beta, freeTurn);

                // Gets maximum evaluation
                if (eval > maxEval)
                {
                    maxEval = eval;
                    maxEvalIndex = i;
                }

                alpha = max(alpha, eval);

                if (beta <= alpha)
                {
                    break;
                }
            }
        }

        // If this is the root node, return the hole index instead
        if (depth == NUMBER_OF_PREDICTED_TURNS)
        {
            return maxEvalIndex;
        }

        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        int minEvalIndex = -1;

        for (int i = 0; i < NUMBER_OF_HOLES; i++)
        {
            if (currentBoardState.minPlayerHoles[i] > 0)
            {
                Board newBoardState = updateBoard(currentBoardState, maximizingPlayer, i);
                bool freeTurn = isFreeTurn(i, currentBoardState.minPlayerHoles[i]);

                int eval = minimax(newBoardState, depth - 1, alpha, beta, !freeTurn);

                if (eval < minEval)
                {
                    minEval = eval;
                    minEvalIndex = i;
                }

                beta = min(beta, eval);

                if (beta <= alpha)
                {
                    break;
                }
            }
        }

        if (depth == NUMBER_OF_PREDICTED_TURNS)
        {
            return minEvalIndex;
        }

        return minEval;
    }
}

void printNextMove(int player, int player1Mancala, vector <int> player1Marbles, int player2Mancala, vector <int> player2Marbles)
{
    bool maximizingPlayer = (player == 1);    // Always assume player 1 is maximizingPlayer.

    // Set up Board State.
    Board boardState;
    boardState.maxPlayerMancala = player1Mancala;
    boardState.minPlayerMancala = player2Mancala;

    for (int i = 0; i < NUMBER_OF_HOLES; i++)
    {
        boardState.maxPlayerHoles[i] = player1Marbles.at(i);
        boardState.minPlayerHoles[i] = player2Marbles.at(i);
    }

    // Increment by 1 because output expects range starting at 1 (minimax works with index starting at 0).
    cout << (minimax(boardState, NUMBER_OF_PREDICTED_TURNS, INT_MIN, INT_MAX, maximizingPlayer) + 1);
}

int main(void)
{
    int _player;
    cin >> _player;

    int _player1Mancala;
    cin >> _player1Mancala;

    int _player1Marbles_size = 6;
    vector<int> _player1Marbles;
    int _player1Marbles_item;

    for (int _player1Marbles_i = 0; _player1Marbles_i < _player1Marbles_size; _player1Marbles_i++) {
        cin >> _player1Marbles_item;
        _player1Marbles.push_back(_player1Marbles_item);
    }

    int _player2Mancala;
    cin >> _player2Mancala;

    int _player2Marbles_size = 6;
    vector<int> _player2Marbles;
    int _player2Marbles_item;

    for (int _player2Marbles_i = 0; _player2Marbles_i < _player2Marbles_size; _player2Marbles_i++) {
        cin >> _player2Marbles_item;
        _player2Marbles.push_back(_player2Marbles_item);
    }

    printNextMove(_player, _player1Mancala, _player1Marbles, _player2Mancala, _player2Marbles);

    return 0;
}