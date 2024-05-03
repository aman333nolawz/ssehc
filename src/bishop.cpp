#pragma once

#include "chess.hpp"
#include <vector>

class Bishop : public Piece {
public:
  using Piece::Piece;

  std::vector<std::vector<int>> getPseudoLegalMoves(int x, int y,
                                                    Board &board) override {
    std::vector<std::vector<int>> moves;

    // Top left diagonal
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x - i, y - i))
        break;
      if (board[y - i][x - i]->type != EMPTY) {
        if (board[y - i][x - i]->color == color)
          break;
        else {
          moves.push_back({x - i, y - i});
          break;
        }
      }
      moves.push_back({x - i, y - i});
    }
    // Top right diagonal
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x + i, y - i))
        break;
      if (board[y - i][x + i]->type != EMPTY) {
        if (board[y - i][x + i]->color == color)
          break;
        else {
          moves.push_back({x + i, y - i});
          break;
        }
      }
      moves.push_back({x + i, y - i});
    }

    // Bottom left diagonal
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x - i, y + i))
        break;
      if (board[y + i][x - i]->type != EMPTY) {
        if (board[y + i][x - i]->color == color)
          break;
        else {
          moves.push_back({x - i, y + i});
          break;
        }
      }
      moves.push_back({x - i, y + i});
    }
    // Bottom right diagonal
    for (int i = 1; i < 8; i++) {
      if (!isValidTile(x + i, y + i))
        break;
      if (board[y + i][x + i]->type != EMPTY) {
        if (board[y + i][x + i]->color == color)
          break;
        else {
          moves.push_back({x + i, y + i});
          break;
        }
      }
      moves.push_back({x + i, y + i});
    }

    return moves;
  }
};
