#pragma once

#include "chess.hpp"
#include <vector>

class Pawn : public Piece {
public:
  using Piece::Piece;
  std::vector<int> enPassantTarget = {-1, -1};

  bool canCapture(int x, int y, Board &board) {
    if (!isValidTile(x, y))
      return false;
    if (board[y][x]->type != EMPTY && board[y][x]->color == color)
      return false;
    if (x == enPassantTarget[0] && y == enPassantTarget[1]) {
      int y2 = color == C_WHITE ? 4 : 3;
      if (board[y2][x]->color == color)
        return false;
    }
    return board[y][x]->type != EMPTY ||
           (x == enPassantTarget[0] && y == enPassantTarget[1]);
  }

  std::vector<std::vector<int>> getPseudoLegalMoves(int x, int y,
                                                    Board &board) override {
    std::vector<std::vector<int>> moves;

    int dir = color == C_WHITE ? -1 : 1;

    // Pawns move one tile forward
    if (isValidTile(x, y + dir) && board[y + dir][x]->type == EMPTY) {
      moves.push_back({x, y + dir});
      // Can move two tiles at start
      if (!hasMoved && board[y + 2 * dir][x]->type == EMPTY) {
        moves.push_back({x, y + 2 * dir});
      }
    }

    // Capturing pieces on sides
    if (canCapture(x - 1, y + dir, board)) {
      moves.push_back({x - 1, y + dir});
    }
    if (canCapture(x + 1, y + dir, board)) {
      moves.push_back({x + 1, y + dir});
    }

    return moves;
  }
};
