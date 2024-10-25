#include "main_Main.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For std::swap

//function's definitions
auto isWithinBounds(int, int) -> bool;
auto promoteToKing(int x, int y) -> void;
auto canCapture(int, int) -> bool;
auto checkEndgame() -> int;

// 8x8 board for the Checkers game
std::vector<std::vector<int>> board = {
        {0, 1, 0, 1, 0, 1, 0, 1},  // player 1's pieces
        {1, 0, 1, 0, 1, 0, 1, 0},  // player 1's
        {0, 1, 0, 1, 0, 1, 0, 1},  // player 1's
        {0, 0, 0, 0, 0, 0, 0, 0},  // empty row
        {0, 0, 0, 0, 0, 0, 0, 0},  // empty row
        {-1, 0, -1, 0, -1, 0, -1, 0},  // player 2's pieces
        {0, -1, 0, -1, 0, -1, 0, -1},  // player 2's
        {-1, 0, -1, 0, -1, 0, -1, 0}   // player 2's
};

int currentPlayer = 1;  // 1 for Player 1 (white), -1 for Player 2 (black)
int jumpingPieceX = -1;  // track the piece that is currently jumping
int jumpingPieceY = -1;

// helps to check if a position is within the bounds of the board
auto isWithinBounds(int x, int y) -> bool {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

// regular piece become king piece if it reaches the last row of the opposite side of the board
auto promoteToKing(int x, int y) -> void {
    if (board[x][y] == 1 && x == 7) board[x][y] = 2;    // player 1 to king
    if (board[x][y] == -1 && x == 0) board[x][y] = -2;  // player 2 to king
}

// checking if a piece
auto canCapture(int x, int y) -> bool {
    int piece = board[x][y];

    // we seperated cause normal pieces can only move forward diagonally

    if (piece == 1) {
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] < 0 && board[x + 2][y + 2] == 0) return true;
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] < 0 && board[x + 2][y - 2] == 0) return true;
    }

    if (piece == -1) {
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] > 0 && board[x - 2][y + 2] == 0) return true;
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] > 0 && board[x - 2][y - 2] == 0) return true;
    }

    // for both kings so abs() is capturing -2 and 2(both kings)
    if (abs(piece) == 2) {
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] != 0 && board[x + 2][y + 2] == 0) return true;
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] != 0 && board[x + 2][y - 2] == 0) return true;
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] != 0 && board[x - 2][y + 2] == 0) return true;
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] != 0 && board[x - 2][y - 2] == 0) return true;
    }

    return false;
}

auto checkEndgame() -> int {
    bool player1HasPieces = false;
    bool player2HasPieces = false;
    bool player1CanMoveOrCapture = false;
    bool player2CanMoveOrCapture = false;

    // helper lambda that checks if a move is a valid capture
    auto canCapture = [](int piece, int x, int y) -> bool {
        if (piece == 1 || piece == 2) {  // Player 1 or King (Player 1)
            return (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] < 0 && board[x + 2][y + 2] == 0) ||
                   (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] < 0 && board[x + 2][y - 2] == 0);
        } else if (piece == -1 || piece == -2) {  // Player 2 or King (Player 2)
            return (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] > 0 && board[x - 2][y + 2] == 0) ||
                   (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] > 0 && board[x - 2][y - 2] == 0);
        }
        return false;
    };

    // checks the all squares on the board to check for remaining pieces and available moves for both players
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int piece = board[i][j];

            // player 1's pieces cause 1 for a regular 2 for a king piece
            if (piece > 0) {
                player1HasPieces = true;

                // checking for valid moves (Player 1)
                if ((i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0) ||  // forward right
                    (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0) ||  // forward left
                    (piece == 2 && i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0) ||  // king backward right
                    (piece == 2 && i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0) ||  // king backward left
                    canCapture(piece, i, j)) {  // checks if piece can capture
                    player1CanMoveOrCapture = true;
                }
            } else if (piece < 0) {
                player2HasPieces = true;

                if ((i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0) ||  // forward right
                    (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0) ||  // forward left
                    (piece == -2 && i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0) ||  // king backward right
                    (piece == -2 && i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0) ||  // king backward left
                    canCapture(piece, i, j)) {
                    player2CanMoveOrCapture = true;
                }
            }
        }
    }

    //checking the game output
    if (!player1HasPieces || !player1CanMoveOrCapture) {
        if (player2HasPieces && player2CanMoveOrCapture) {
            return -1;  // Player 2 (black) wins
        }
    }

    if (!player2HasPieces || !player2CanMoveOrCapture) {
        if (player1HasPieces && player1CanMoveOrCapture) {
            return 1;
        }
    }

    // if neither player can move nor capture, it's a draw
    if (!player1CanMoveOrCapture && !player2CanMoveOrCapture) {
        return 2;  // Draw
    }

    return 0;  // game hasn't finished yet
}

// JNI PART
JNIEXPORT jboolean JNICALL Java_main_Main_movePiece(JNIEnv *env, jclass obj, jint startX, jint startY, jint endX, jint endY) {
    // prevent other pieces from moving during a multi-jump turn
    // only the currently jumping piece can move
    if (jumpingPieceX != -1 && (startX != jumpingPieceX || startY != jumpingPieceY)) {
        return JNI_FALSE;
    }

    if (!isWithinBounds(startX, startY) || !isWithinBounds(endX, endY) || board[endX][endY] != 0) {
        return JNI_FALSE;  // invalid move
    }

    int piece = board[startX][startY];
    if (piece == 0 || (piece != currentPlayer && piece != currentPlayer * 2)) {
        return JNI_FALSE;  // invalid move (wrong player)
    }

    // Regular piece's movements (1 step diagonal)
    if (abs(endX - startX) == 1 && abs(endY - startY) == 1) {
        // normal pieces can only move forward
        if ((piece == 1 && endX > startX) || (piece == -1 && endX < startX) || abs(piece) == 2) {
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
            currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;  // Switch turns
            jumpingPieceX = jumpingPieceY = -1;  // reset jump tracking
            return JNI_TRUE;
        }
    }

    // jumping over opponent
    if (abs(endX - startX) == 2 && abs(endY - startY) == 2) {
        int midX = (startX + endX) / 2;
        int midY = (startY + endY) / 2;
        int midPiece = board[midX][midY];

        // regular pieces should only capture forward
        if (piece == 1 && endX > startX && midPiece < 0) {
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else if (piece == -1 && endX < startX && midPiece > 0) {
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        }
        // kings capturing in all directions
        else if (abs(piece) == 2 && ((piece > 0 && midPiece < 0) || (piece < 0 && midPiece > 0))) {
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else {
            return JNI_FALSE;  // invalid capture
        }

        // check if more captures are possible
        if (canCapture(endX, endY)) {
            jumpingPieceX = endX;
            jumpingPieceY = endY;
            return JNI_TRUE;
        }

        // No more captures possible, switch turn to opponent
        currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;
        jumpingPieceX = jumpingPieceY = -1;  // reset jump tracking after final jump
        return JNI_TRUE;
    }

    return JNI_FALSE;
}


// JNI method to return the current board state to Java
JNIEXPORT jobjectArray JNICALL Java_main_Main_getBoardState(JNIEnv *env, jclass obj) {
    // Create 8x8 array
    jobjectArray boardState = env->NewObjectArray(8, env->FindClass("[I"), 0);

    for (int i = 0; i < 8; ++i) {
        jintArray row = env->NewIntArray(8);  // create int array for each row
        env->SetIntArrayRegion(row, 0, 8, &board[i][0]);  // copy C++ board row to Java array
        env->SetObjectArrayElement(boardState, i, row);  // add row to the Java array
        env->DeleteLocalRef(row);  // avoid memory leak
    }

    return boardState;
}


// JNI method to return the endgame status to Java
JNIEXPORT jint JNICALL Java_main_Main_checkEndgame(JNIEnv *env, jclass obj) {
    // Return the result of the endgame check
    return checkEndgame();
}

JNIEXPORT jint JNICALL Java_main_Main_getTileState
        (JNIEnv *, jclass, jint row, jint col){
    return board[row][col];
}
