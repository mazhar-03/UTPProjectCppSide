#include "main_Main.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For std::swap

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

// Helper to check if a position is within the bounds of the board
bool isWithinBounds(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

// Promote to king if a piece reaches the opposite side of the board
void promoteToKing(int x, int y) {
    if (board[x][y] == 1 && x == 7) board[x][y] = 2;    // Player 1 to king
    if (board[x][y] == -1 && x == 0) board[x][y] = -2;  // Player 2 to king
}

// Check if a capture is possible for a specific piece
bool canCapture(int x, int y) {
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

// Handle movement and capture logic for both normal and king pieces
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
        // Normal pieces can only move forward
        if ((piece == 1 && endX > startX) || (piece == -1 && endX < startX) || abs(piece) == 2) {  // Forward for normal pieces
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
            currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;  // Switch turns
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
        if (piece == 1 && endX > startX && midPiece < 0) {  // Player 1 capturing
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else if (piece == -1 && endX < startX && midPiece > 0) {  // Player 2 capturing
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else if (abs(piece) == 2 && ((piece > 0 && midPiece < 0) || (piece < 0 && midPiece > 0))) {  // Kings capturing in all directions
            board[midX][midY] = 0;
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
        } else {
            return JNI_FALSE;  // Invalid capture
        }

        // Allow the same piece to continue jumping if possible
        if (canCapture(endX, endY)) {
            jumpingPieceX = endX;
            jumpingPieceY = endY;
            return JNI_TRUE;
        }

        currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;  // Switch turns
        jumpingPieceX = jumpingPieceY = -1;  // Reset jump tracking after final jump
        return JNI_TRUE;
    }

    return JNI_FALSE;  // Invalid move
}


// JNI method to return the current board state to Java
JNIEXPORT jobjectArray JNICALL Java_main_Main_getBoardState(JNIEnv *env, jclass obj) {
    jobjectArray boardState = env->NewObjectArray(8, env->FindClass("[I"), nullptr);  // Create 8x8 array

    for (int i = 0; i < 8; ++i) {
        jintArray row = env->NewIntArray(8);  // Create int array for each row
        env->SetIntArrayRegion(row, 0, 8, &board[i][0]);  // Copy C++ board row to Java array
        env->SetObjectArrayElement(boardState, i, row);  // Add row to the Java array
        env->DeleteLocalRef(row);  // Avoid memory leak
    }

    return boardState;
}

int checkEndgame() {
    bool player1HasPieces = false;
    bool player2HasPieces = false;
    bool player1CanMoveOrCapture = false;
    bool player2CanMoveOrCapture = false;

    // Helper function to check if a move is a valid capture
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

    // Iterate over the board to check for remaining pieces and available moves for both players
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
                    canCapture(piece, i, j)) {  // Check if piece can capture
                    player1CanMoveOrCapture = true;
                }
            } else if (piece < 0) {  // Player 2's pieces
                player2HasPieces = true;

                // Check for valid moves or captures for Player 2
                if ((i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0) ||  // Forward right
                    (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0) ||  // Forward left
                    (piece == -2 && i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0) ||  // King backward right
                    (piece == -2 && i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0) ||  // King backward left
                    canCapture(piece, i, j)) {  // Check if piece can capture
                    player2CanMoveOrCapture = true;
                }
            }
        }
    }

    // Determine the game outcome
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

    // If neither player can move or capture, it's a draw
    if (!player1CanMoveOrCapture && !player2CanMoveOrCapture) {
        return 2;  // Draw
    }

    return 0;  // Game is still ongoing
}


// JNI method to return the endgame status to Java
JNIEXPORT jint JNICALL Java_main_Main_checkEndgame(JNIEnv *env, jclass obj) {
    return checkEndgame();  // Return the result of the endgame check
}