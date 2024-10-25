#include "main_Main.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For std::swap

// function declarations
auto isWithinBounds(int, int) -> bool;
auto promoteToKing(int x, int y) -> void;
auto canCapture(int, int) -> bool;
auto checkEndgame() -> int;

// 8x8 board for the Checkers game
std::vector<std::vector<int>> board = {
        {0, 1, 0, 1, 0, 1, 0, 1},  // Player 1's pieces
        {1, 0, 1, 0, 1, 0, 1, 0},  // Player 1
        {0, 1, 0, 1, 0, 1, 0, 1},  // Player 1
        {0, 0, 0, 0, 0, 0, 0, 0},  // Empty row
        {0, 0, 0, 0, 0, 0, 0, 0},  // Empty row
        {-1, 0, -1, 0, -1, 0, -1, 0},  // Player 2's pieces
        {0, -1, 0, -1, 0, -1, 0, -1},  // Player 2
        {-1, 0, -1, 0, -1, 0, -1, 0}   // Player 2
};

int currentPlayer = 1;  // 1 for Player 1 (white), -1 for Player 2 (black)
int jumpingPieceX = -1;  // Track the piece that is currently jumping
int jumpingPieceY = -1;

// Helper function to check if a position is within the bounds of the board
auto isWithinBounds(int x, int y) -> bool {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

// Promote to king if a piece reaches the opposite side of the board
auto promoteToKing(int x, int y) -> void {
    if (board[x][y] == 1 && x == 7) board[x][y] = 2;    // Player 1 to king
    if (board[x][y] == -1 && x == 0) board[x][y] = -2;  // Player 2 to king
}

// Check if a capture is possible for a specific piece
auto canCapture(int x, int y) -> bool {
    int piece = board[x][y];

    if (piece == 1) {  // Player 1 normal piece (forward captures only)
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] < 0 && board[x + 2][y + 2] == 0) return true;
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] < 0 && board[x + 2][y - 2] == 0) return true;
    }

    if (piece == -1) {  // Player 2 normal piece (forward captures only)
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] > 0 && board[x - 2][y + 2] == 0) return true;
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] > 0 && board[x - 2][y - 2] == 0) return true;
    }

    // King pieces can capture in all directions
    if (abs(piece) == 2) {
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] != 0 && board[x + 2][y + 2] == 0) return true;
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] != 0 && board[x + 2][y - 2] == 0) return true;
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] != 0 && board[x - 2][y + 2] == 0) return true;
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] != 0 && board[x - 2][y - 2] == 0) return true;
    }

    return false;
}

// Check for the endgame conditions
auto checkEndgame() -> int {
    bool player1HasPieces = false;
    bool player2HasPieces = false;
    bool player1CanMoveOrCapture = false;
    bool player2CanMoveOrCapture = false;

    // Check all squares on the board to determine remaining pieces and available moves
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int piece = board[i][j];

            if (piece > 0) {  // Player 1's pieces
                player1HasPieces = true;

                // Check for valid moves or captures for Player 1
                if ((i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0) ||  // Forward right
                    (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0) ||  // Forward left
                    (piece == 2 && i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0) ||  // King backward right
                    (piece == 2 && i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0) ||  // King backward left
                    canCapture(i, j)) {  // Check if piece can capture
                    player1CanMoveOrCapture = true;
                }
            } else if (piece < 0) {  // Player 2's pieces
                player2HasPieces = true;

                // Check for valid moves or captures for Player 2
                if ((i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0) ||  // Forward right
                    (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0) ||  // Forward left
                    (piece == -2 && i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0) ||  // King backward right
                    (piece == -2 && i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0) ||  // King backward left
                    canCapture(i, j)) {
                    player2CanMoveOrCapture = true;
                }
            }
        }
    }

    // Determine the game result based on remaining pieces and available moves
    if (!player1HasPieces || !player1CanMoveOrCapture) {
        if (player2HasPieces && player2CanMoveOrCapture) {
            return -1;  // Player 2 wins
        }
    }

    if (!player2HasPieces || !player2CanMoveOrCapture) {
        if (player1HasPieces && player1CanMoveOrCapture) {
            return 1;  // Player 1 wins
        }
    }

    if (!player1CanMoveOrCapture && !player2CanMoveOrCapture) {
        return 2;  // Draw
    }

    return 0;  // Game is still ongoing
}

// JNI part
JNIEXPORT jboolean JNICALL Java_main_Main_movePiece(JNIEnv *env, jclass obj, jint startX, jint startY, jint endX, jint endY) {
    // Prevent other pieces from moving during a multi-jump turn
    if (jumpingPieceX != -1 && (startX != jumpingPieceX || startY != jumpingPieceY)) {
        return JNI_FALSE;  // Only the currently jumping piece can move
    }

    if (!isWithinBounds(startX, startY) || !isWithinBounds(endX, endY) || board[endX][endY] != 0) {
        return JNI_FALSE;  // Invalid move
    }

    int piece = board[startX][startY];
    if (piece == 0 || (piece != currentPlayer && piece != currentPlayer * 2)) {
        return JNI_FALSE;  // Invalid move (wrong player)
    }

    // Normal movement (1 step diagonal)
    if (abs(endX - startX) == 1 && abs(endY - startY) == 1) {
        // Normal pieces can only move forward; kings can move in any direction
        if ((piece == 1 && endX > startX) || (piece == -1 && endX < startX) || abs(piece) == 2) {
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
            currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;  // Switch turns after a normal move
            jumpingPieceX = jumpingPieceY = -1;  // Reset jump tracking
            return JNI_TRUE;
        }
    }

    // Capture move (jump over opponent)
    if (abs(endX - startX) == 2 && abs(endY - startY) == 2) {
        int midX = (startX + endX) / 2;
        int midY = (startY + endY) / 2;
        int midPiece = board[midX][midY];

        // Normal pieces should only capture forward
        if (piece == 1 && endX > startX && midPiece < 0) {
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else if (piece == -1 && endX < startX && midPiece > 0) {
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else if (abs(piece) == 2 && ((piece > 0 && midPiece < 0) || (piece < 0 && midPiece > 0))) {  // Kings can capture in all directions
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else {
            return JNI_FALSE;  // Invalid capture
        }

        // Check if more captures are possible (multi-jump rule)
        if (canCapture(endX, endY)) {
            jumpingPieceX = endX;
            jumpingPieceY = endY;
            return JNI_TRUE;  // Allow further jumps if possible
        }

        // No more captures possible, switch turn to opponent
        currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;
        jumpingPieceX = jumpingPieceY = -1;  // Reset jump tracking after final jump
        return JNI_TRUE;
    }

    return JNI_FALSE;  // Invalid move
}


// JNI method to get tile state
JNIEXPORT jint JNICALL Java_main_Main_getTileState(JNIEnv *env, jclass obj, jint row, jint col) {
    return board[row][col];
}

// JNI method to check endgame status
JNIEXPORT jint JNICALL Java_main_Main_checkEndgame(JNIEnv *env, jclass obj) {
    return checkEndgame();
}
