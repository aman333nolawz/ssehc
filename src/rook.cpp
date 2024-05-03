#pragma once

#include "chess.h"
#include <vector>

class Rook : public Piece {
public:
  using Piece::Piece;

  std::vector<std::vector<int>> getPseudoLegalMoves(int x, int y,
                                                    Board &board) override {
    std::vector<std::vector<int>> moves;

    // Horizontal Right movement
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x - i, y))
        break;
      if (board[y][x - i]->type != EMPTY) {
        if (board[y][x - i]->color == color)
          break;
        else {
          moves.push_back({x - i, y});
          break;
        }
      }
      moves.push_back({x - i, y});
    }
    // Horizontal Left movement
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x + i, y))
        break;
      if (board[y][x + i]->type != EMPTY) {
        if (board[y][x + i]->color == color)
          break;
        else {
          moves.push_back({x + i, y});
          break;
        }
      }
      moves.push_back({x + i, y});
    }

    // Vertical up movement
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x, y - i))
        break;
      if (board[y - i][x]->type != EMPTY) {
        if (board[y - i][x]->color == color)
          break;
        else {
          moves.push_back({x, y - i});
          break;
        }
      }
      moves.push_back({x, y - i});
    }
    // Horizontal Left movement
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x, y + i))
        break;
      if (board[y + i][x]->type != EMPTY) {
        if (board[y + i][x]->color == color)
          break;
        else {
          moves.push_back({x, y + i});
          break;
        }
      }
      moves.push_back({x, y + i});
    }

    return moves;
  }
};
