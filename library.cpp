#include "Main.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For std::swap

std::vector<std::vector<int>> board = {
        {0, 1, 0, 1, 0, 1, 0, 1},  // Player 1's pieces
        {1, 0, 1, 0, 1, 0, 1, 0},  // Player 1
        {0, 1, 0, 1, 0, 1, 0, 1},  // Player 1
        {0, 0, 0, 0, 0, 0, 0, 0},  // Empty
        {0, 0, 0, 0, 0, 0, 0, 0},  // Empty
        {-1, 0, -1, 0, -1, 0, -1, 0},  // Player 2's pieces
        {0, -1, 0, -1, 0, -1, 0, -1},  // Player 2
        {-1, 0, -1, 0, -1, 0, -1, 0}   // Player 2
};

int currentPlayer = 1;

// Helper to check if a position is within the bounds of the board
auto isWithinBounds = [](int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
};

// Promote to king if a piece reaches the opposite end
void promoteToKing(int x, int y) {
    if (board[x][y] == 1 && x == 7) {
        board[x][y] = 2;  // Promote Player 1 to king
    }
    if (board[x][y] == -1 && x == 0) {
        board[x][y] = -2;  // Promote Player 2 to king
    }
}

// Check if a capture is possible from a given position (for both normal and king pieces)
bool canCapture(int x, int y) {
    int piece = board[x][y];

    // For Player 1 regular piece (only forward captures)
    if (piece == 1) {
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] < 0 && board[x + 2][y + 2] == 0) return true;  // Forward-right
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] < 0 && board[x + 2][y - 2] == 0) return true;  // Forward-left
    }

    // For Player 2 regular piece (only forward captures)
    if (piece == -1) {
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] > 0 && board[x - 2][y + 2] == 0) return true;  // Forward-right
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] > 0 && board[x - 2][y - 2] == 0) return true;  // Forward-left
    }

    // For Player 1 king (can capture in all directions)
    if (piece == 2) {
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] < 0 && board[x + 2][y + 2] == 0) return true;  // Forward-right
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] < 0 && board[x + 2][y - 2] == 0) return true;  // Forward-left
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] < 0 && board[x - 2][y + 2] == 0) return true;  // Backward-right
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] < 0 && board[x - 2][y - 2] == 0) return true;  // Backward-left
    }

    // For Player 2 king (can capture in all directions)
    if (piece == -2) {
        if (isWithinBounds(x - 2, y + 2) && board[x - 1][y + 1] > 0 && board[x - 2][y + 2] == 0) return true;  // Forward-right
        if (isWithinBounds(x - 2, y - 2) && board[x - 1][y - 1] > 0 && board[x - 2][y - 2] == 0) return true;  // Forward-left
        if (isWithinBounds(x + 2, y + 2) && board[x + 1][y + 1] > 0 && board[x + 2][y + 2] == 0) return true;  // Backward-right
        if (isWithinBounds(x + 2, y - 2) && board[x + 1][y - 1] > 0 && board[x + 2][y - 2] == 0) return true;  // Backward-left
    }

    return false;
}

// Handle movement and capture logic for both regular pieces and kings (with multiple jumps)
JNIEXPORT jboolean JNICALL Java_Main_movePiece(JNIEnv, jclass, jint startX, jint startY, jint endX, jint endY) {
    if (!isWithinBounds(startX, startY) || !isWithinBounds(endX, endY) || board[endX][endY] != 0) {
        return JNI_FALSE;  // Invalid move
    }

    auto piece = board[startX][startY];
    if (piece == 0 || (piece != currentPlayer && piece != currentPlayer * 2)) {
        return JNI_FALSE;  // Invalid player move
    }

    // Regular movement logic for normal and king pieces
    if (abs(endX - startX) == 1 && abs(endY - startY) == 1) {
        // Restrict regular pieces to forward moves only, allow kings to move in any direction
        if ((piece == 1 && endX > startX) || (piece == -1 && endX < startX) || abs(piece) == 2) {
            std::swap(board[startX][startY], board[endX][endY]);
            promoteToKing(endX, endY);
            currentPlayer = (piece == 1 || piece == 2) ? -1 : 1;  // Switch turns after a regular move
            return JNI_TRUE;
        }
    }

    // Capture logic (jump over opponent)
    if (abs(endX - startX) == 2 && abs(endY - startY) == 2) {
        auto midX = (startX + endX) / 2;
        auto midY = (startY + endY) / 2;
        auto midPiece = board[midX][midY];

        // Regular piece or king piece for Player 1 (only forward captures for regular pieces)
        if ((piece == 1 && endX > startX) || piece == 2) {
            if ((midPiece == -1 || midPiece == -2)) {
                board[midX][midY] = 0;  // Remove the captured piece
                std::swap(board[startX][startY], board[endX][endY]);
                promoteToKing(endX, endY);

                // Check if more captures are possible after this capture
                if (canCapture(endX, endY)) {
                    return JNI_TRUE;  // Keep the turn, more captures available
                }

                currentPlayer = -1;  // Switch turn to Player 2 (black) after the capture
                return JNI_TRUE;
            }
        }

        // Regular piece or king piece for Player 2 (only forward captures for regular pieces)
        if ((piece == -1 && endX < startX) || piece == -2) {
            if ((midPiece == 1 || midPiece == 2)) {
                board[midX][midY] = 0;  // Remove the captured piece
                std::swap(board[startX][startY], board[endX][endY]);
                promoteToKing(endX, endY);

                // Check if more captures are possible after this capture
                if (canCapture(endX, endY)) {
                    return JNI_TRUE;  // Keep the turn, more captures available
                }

                currentPlayer = 1;  // Switch turn to Player 1 (white) after the capture
                return JNI_TRUE;
            }
        }
    }

    return JNI_FALSE;  // Invalid move
}

// JNI method to return the current board state to Java
JNIEXPORT jobjectArray JNICALL Java_Main_getBoardState(JNIEnv *env, jclass) {
    auto boardState = env->NewObjectArray(8, env->FindClass("[I"), nullptr);  // Create 8x8 Java int array

    for (auto i = 0; i < 8; ++i) {
        auto row = env->NewIntArray(8);  // Create new Java int array for each row
        env->SetIntArrayRegion(row, 0, 8, &board[i][0]);  // Copy C++ board row data to Java array
        env->SetObjectArrayElement(boardState, i, row);  // Insert row into Java array
        env->DeleteLocalRef(row);  // Avoid memory leaks
    }

    return boardState;  // Return the board state to Java
}
