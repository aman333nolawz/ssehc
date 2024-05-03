#pragma once

#include "chess.hpp"
#include <vector>

class Queen : public Piece {
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
