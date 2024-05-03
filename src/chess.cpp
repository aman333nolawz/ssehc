#include "chess.hpp"
#include "raygui.h"
#include <cmath>
#include <memory>
#include <raylib.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "board.cpp"
#include "constants.cpp"
#include "engine.cpp"
#include "pawn.cpp"
#include "piece.cpp"
#include "queen.cpp"

std::string Chess::fen(Board &_board) {
  int blankCount = 0;
  std::string fenstring = "";
  int wKingX = -1;
  int wKingY = -1;

  int bKingX = -1;
  int bKingY = -1;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (_board[y][x]->type != EMPTY) {
        if (blankCount != 0) {
          fenstring += std::to_string(blankCount);
        }
        if (board[y][x]->type == KING) {
          if (board[y][x]->color == C_WHITE) {
            wKingX = x;
            wKingY = y;
          } else {
            bKingX = x;
            bKingY = y;
          }
        }
        blankCount = 0;
        fenstring += _board[y][x]->abbrev;
      } else {
        blankCount++;
      }
    }
    if (blankCount != 0) {
      fenstring += std::to_string(blankCount);
      blankCount = 0;
    }
    if (y < 7) {
      fenstring += '/';
    }
  }

  fenstring += turn == C_WHITE ? " w " : " b ";

  bool wQSideCastling = !_board[wKingY][wKingX]->hasMoved &&
                        _board[wKingY][0]->type == ROOK &&
                        !_board[wKingY][0]->hasMoved;
  bool wKSideCastling = !_board[wKingY][wKingX]->hasMoved &&
                        _board[wKingY][7]->type == ROOK &&
                        !_board[wKingY][7]->hasMoved;
  bool bQSideCastling = !_board[bKingY][bKingY]->hasMoved &&
                        _board[bKingY][0]->type == ROOK &&
                        !_board[bKingY][0]->hasMoved;
  bool bKSideCastling = !_board[bKingY][bKingY]->hasMoved &&
                        _board[bKingY][7]->type == ROOK &&
                        !_board[bKingY][7]->hasMoved;
  fenstring += wKSideCastling ? "K" : "-";
  fenstring += wQSideCastling ? "Q" : "-";
  fenstring += bKSideCastling ? "k" : "-";
  fenstring += bQSideCastling ? "q " : "- ";

  if (enPassantTarget[0] != -1) {
    fenstring += (char)enPassantTarget[0] + 97;
    if (turn == C_BLACK) // White moved 2 squares
      fenstring += std::to_string(8 - enPassantTarget[1]) + " ";
    else
      fenstring += std::to_string(8 - enPassantTarget[1]) + " ";
  } else {
    fenstring += "- ";
  }

  fenstring +=
      std::to_string(halfmoveClock) + ' ' + std::to_string(fullMoveCounter);
  return fenstring;
}

void Chess::computerMove() {
  std::vector<int> move = engine.bestMove(moves);

  moveTo(move[0], move[1], move[2], move[3]);
}

bool Chess::isInCheck(int color, Board &_board) {
  int kingPos[2] = {-1, -1};
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (_board[y][x]->type == KING && _board[y][x]->color == color) {
        kingPos[0] = x;
        kingPos[1] = y;
        break;
      }
    }
    if (kingPos[0] != -1)
      break;
  }

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (_board[y][x]->type != EMPTY && _board[y][x]->color != color) {
        for (auto move : _board[y][x]->getPseudoLegalMoves(x, y, _board)) {
          if (move[0] == kingPos[0] && move[1] == kingPos[1]) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

std::array<bool, 2> Chess::canCastle(int x, int y, Board &_board) {
  std::shared_ptr<Piece> piece = _board[y][x];
  if (piece->type != KING || piece->hasMoved)
    return {false, false};
  int color = piece->color;
  bool queenSideCastling =
      _board[y][0]->type == ROOK && !_board[y][0]->hasMoved;
  bool kingSideCastling = _board[y][7]->type == ROOK && !_board[y][7]->hasMoved;

  // Queen side castling
  if (queenSideCastling) {
    for (int i = 0; i <= 4; i++) {
      if (x - i < 0)
        break;
      std::shared_ptr<Piece> pieceBetween = _board[y][x - i];
      if (i != 0 && i != 4 && pieceBetween->type != EMPTY) {
        queenSideCastling = false;
        break;
      }
      // boardCopy.deletePieces();
      Board boardCopy = Board(_board);
      placeTo(x, y, x - i, y, boardCopy);
      if (isInCheck(color, boardCopy)) {
        queenSideCastling = false;
        break;
      }
    }
  }

  // King side castling
  if (kingSideCastling) {
    for (int i = 0; i <= 3; i++) {
      if (x + i > 7)
        break;
      std::shared_ptr<Piece> pieceBetween = _board[y][x + i];
      if (i != 0 && i != 3 && pieceBetween->type != EMPTY) {
        kingSideCastling = false;
        break;
      }
      // boardCopy.deletePieces();
      Board boardCopy = Board(_board);
      placeTo(x, y, x + i, y, boardCopy);
      if (isInCheck(color, boardCopy)) {
        kingSideCastling = false;
        break;
      }
    }
  }

  // boardCopy.deletePieces();
  return {queenSideCastling, kingSideCastling};
}

std::vector<std::vector<int>> Chess::legalMoves(int x, int y, int sideToMove,
                                                Board &_board) {
  std::vector<std::vector<int>> moves;
  std::shared_ptr<Piece> piece = _board[y][x];
  if (piece->color != sideToMove)
    return moves;
  if (piece->type == PAWN) {
    std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(_board[y][x]);
    pawn->enPassantTarget = enPassantTarget;
  }

  for (auto move : piece->getPseudoLegalMoves(x, y, _board)) {
    Board boardCopy = Board(_board);
    placeTo(x, y, move[0], move[1], boardCopy);
    if (!isInCheck(sideToMove, boardCopy)) {
      moves.push_back({move[0], move[1]});
    }
  }

  // Castling
  std::array<bool, 2> castling = canCastle(x, y, _board);
  if (castling[0] || castling[1]) {
    castlingMoves = {};
  }
  if (castling[0]) {
    if (0 <= x - 2) {
      moves.push_back({x - 2, y});
      castlingMoves.push_back({x - 2, y});
    }
  } else if (castling[1]) {
    if (x + 2 < 8) {
      moves.push_back({x + 2, y});
      castlingMoves.push_back({x + 2, y});
    }
  }

  return moves;
}

void Chess::placeTo(int x1, int y1, int x2, int y2, Board &_board,
                    bool playSound) {
  std::shared_ptr<Piece> from = _board[y1][x1];

  bool capturing = board[y1][x1]->type != EMPTY && board[y2][x2]->type != EMPTY;
  bool castling = false;
  bool promoting = false;

  _board[y1][x1] = std::make_shared<Piece>();
  if (x2 == enPassantTarget[0] && y2 == enPassantTarget[1]) {
    _board[y2][x2] = from;
    if (turn == C_WHITE) {
      y2 += 1;
    } else {
      y2 -= 1;
    }
    _board[y2][x2] = std::make_shared<Piece>();
  } else if ((from->abbrev == 'p' && y2 == 7) ||
             (from->abbrev == 'P' && y2 == 0)) { // Promotion of pawn
    _board[y2][x2] =
        std::make_shared<Queen>(from->color == C_WHITE ? 'Q' : 'q');

    promoting = true;
  } else {
    _board[y2][x2] = from;
    if (from->type == KING) {
      // Castling
      for (auto castlingMove : castlingMoves) {
        if (x2 == castlingMove[0] && y2 == castlingMove[1]) {
          if (x2 < x1) { // Queen side castling
            placeTo(0, y1, 3, y1, _board);
          } else { // King side castling
            placeTo(7, y1, 5, y1, _board);
          }
          castling = true;
        }
      }
    }
  }

  if (!playSound)
    return;

  if (isInCheck(!turn, board))
    PlaySound(checkSound);
  else if (promoting)
    PlaySound(promotionSound);
  else if (capturing)
    PlaySound(captureSound);
  else if (castling)
    PlaySound(castlingSound);
  else
    PlaySound(selfMoveSound);
}

bool Chess::moveTo(int x1, int y1, int x2, int y2) {
  std::shared_ptr<Piece> from = board[y1][x1];

  for (auto move : legalMoves(x1, y1, turn, board)) {
    if (move[0] == x2 and move[1] == y2) {

      if (board[y2][x2]->type != EMPTY || board[y1][x1]->type == PAWN) {
        halfmoveClock = 0; // reset halfmoveclock on capture or pawn move
      } else {
        halfmoveClock++;
      }

      placeTo(x1, y1, x2, y2, board, true);
      // Add en passant targets
      if (from->type == PAWN && std::abs(y2 - y1) == 2) {
        enPassantTarget = {x2, y1 + (y2 - y1) / 2};
        enPassantColor = turn;
      }

      if (turn == C_BLACK)
        fullMoveCounter++;
      if (turn != enPassantColor) {
        enPassantTarget = {-1, -1};
        enPassantColor = -1;
      }

      from->hasMoved = true;
      turn = !turn;

      lastMove[0][0] = x1;
      lastMove[0][1] = y1;
      lastMove[1][0] = x2;
      lastMove[1][1] = y2;

      moves += posToUci(x1, y1, x2, y2, board) + ' ';
      if (turn == computer) {
        computerMove();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        PlaySound(oppMoveSound);
      }
      return true;
    }
  }
  PlaySound(illegalSound);
  return false;
}
