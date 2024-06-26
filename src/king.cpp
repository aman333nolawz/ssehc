#pragma once

#include "chess.hpp"
#include <vector>

class King : public Piece {
public:
  using Piece::Piece;

  std::vector<std::vector<int>> getPseudoLegalMoves(int x, int y,
                                                    Board &board) override {
    std::vector<std::vector<int>> possibleMoves = {
        {x - 1, y - 1}, {x, y - 1},     {x + 1, y - 1}, {x - 1, y},
        {x + 1, y},     {x - 1, y + 1}, {x, y + 1},     {x + 1, y + 1}};
    std::vector<std::vector<int>> moves;

    for (auto move : possibleMoves) {
      int x = move[0];
      int y = move[1];
      if (!isValidTile(x, y)) {
        continue;
      }
      if (board[y][x]->color != color || board[y][x]->type == EMPTY) {
        moves.push_back(move);
      }
    }

    return moves;
  }
};
