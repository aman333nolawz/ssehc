#include "chess.h"
#include <cctype>

Piece::Piece(char abbrev) {
  this->abbrev = abbrev;
  this->type = std::toupper(abbrev);
  this->color = std::isupper(abbrev) ? C_WHITE : C_BLACK;
}

bool Piece::isValidTile(int x, int y) {
  return (0 <= x && x < 8) && (0 <= y && y < 8);
}

std::vector<std::vector<int>> Piece::getPseudoLegalMoves(int x, int y,
                                                         Board &board) {
  std::vector<std::vector<int>> moves;
  return moves;
}
