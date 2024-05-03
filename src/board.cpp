#include "bishop.cpp"
#include "chess.hpp"
#include "king.cpp"
#include "knight.cpp"
#include "pawn.cpp"
#include "queen.cpp"
#include "rook.cpp"
#include <cctype>
#include <memory>

Board::Board() {
  board.resize(8);
  char initialBoard[8][8] = {
      {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
      {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
      {'-', '-', '-', '-', '-', '-', '-', '-'},
      {'-', '-', '-', '-', '-', '-', '-', '-'},
      {'-', '-', '-', '-', '-', '-', '-', '-'},
      {'-', '-', '-', '-', '-', '-', '-', '-'},
      {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
      {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
  };
  for (int y = 0; y < 8; y++) {
    board[y].resize(8);
    for (int x = 0; x < 8; x++) {
      switch (std::tolower(initialBoard[y][x])) {
      case 'k':
        board[y][x] = std::make_shared<King>(initialBoard[y][x]);
        break;
      case 'q':
        board[y][x] = std::make_shared<Queen>(initialBoard[y][x]);
        break;
      case 'r':
        board[y][x] = std::make_shared<Rook>(initialBoard[y][x]);
        break;
      case 'n':
        board[y][x] = std::make_shared<Knight>(initialBoard[y][x]);
        break;
      case 'b':
        board[y][x] = std::make_shared<Bishop>(initialBoard[y][x]);
        break;
      case 'p':
        board[y][x] = std::make_shared<Pawn>(initialBoard[y][x]);
        break;
      default:
        board[y][x] = std::make_shared<Piece>(initialBoard[y][x]);
        break;
      }
    }
  }
}

Board::Board(const Board &rhs) {
  board.resize(8);
  for (int y = 0; y < 8; y++) {
    board[y].resize(8);
    std::copy(rhs.board[y].begin(), rhs.board[y].end(), board[y].begin());
  }
}

std::vector<std::shared_ptr<Piece>> &Board::operator[](int index) {
  return board[index];
}

Board::~Board() { board.clear(); }
