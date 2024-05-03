#pragma once

#include "constants.cpp"
#include "raylib.h"
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct Move {
  int from[2];
  int to[2];

  Move(int x1, int y1, int x2, int y2) {
    from[0] = x1;
    from[1] = y1;
    to[0] = x2;
    to[1] = y2;
  }
};

class Piece;

class Board {
public:
  std::vector<std::vector<std::shared_ptr<Piece>>> board;

  Board();
  Board(const Board &rhs);
  std::vector<std::shared_ptr<Piece>> &operator[](int index);
  ~Board();
};

const std::map<char, int> pieceValues = {{KING, 200}, {QUEEN, 9},  {ROOK, 5},
                                         {BISHOP, 3}, {KNIGHT, 3}, {PAWN, 1}};

class Piece {
public:
  bool hasMoved = false;
  char type;
  char abbrev;
  int color = -1;

  Piece(char abbrev = EMPTY);
  bool isValidTile(int x, int y);

  virtual std::vector<std::vector<int>> getPseudoLegalMoves(int x, int y,
                                                            Board &board);
};

class Engine {
public:
  Board board;

  Engine(Board &board);
  void newGame();
  std::vector<int> bestMove(std::string moves);
};

class Chess {
public:
  Board board;
  Engine engine = Engine(board);

  Sound selfMoveSound;
  Sound captureSound;
  Sound castlingSound;
  Sound startSound;
  Sound oppMoveSound;
  Sound checkSound;
  Sound illegalSound;
  Sound promotionSound;

  int turn = C_WHITE;
  int ogTurn = turn;
  int player = C_WHITE;
  int computer = C_BLACK;
  int halfmoveClock = 0;
  int fullMoveCounter = 1;
  std::vector<std::vector<int>> castlingMoves;
  std::vector<int> enPassantTarget = {-1, -1};
  int enPassantColor = -1;
  std::string moves = "";
  int lastMove[2][2] = {{-1, -1}, {-1, -1}};

  inline Chess() {
    startSound = LoadSound("audio/game-start.ogg");
    selfMoveSound = LoadSound("audio/move-self.ogg");
    oppMoveSound = LoadSound("audio/move-opponent.ogg");
    captureSound = LoadSound("audio/capture.ogg");
    castlingSound = LoadSound("audio/castle.ogg");
    checkSound = LoadSound("audio/move-check.ogg");
    illegalSound = LoadSound("audio/illegal.ogg");
    promotionSound = LoadSound("audio/promotion.ogg");
    PlaySound(startSound);
  }

  static inline std::string posToUci(int x1, int y1, int x2, int y2,
                                     Board board) {
    std::string move = "";
    move += (char)(x1 + 97) + std::to_string(8 - y1);
    move += (char)(x2 + 97) + std::to_string(8 - y2);

    if (board[y1][x1]->type == PAWN) {
      if (board[y1][x1]->color == C_WHITE && y1 == 1 && y2 == 0) {
        move += 'q';
      } else if (board[y1][x1]->color == C_BLACK && y1 == 7 && y2 == 8) {
        move += 'q';
      }
    }

    return move;
  }
  static inline std::vector<int> UciToPos(std::string move) {
    std::vector<int> moveVector;
    moveVector.push_back(move[0] - 97);
    moveVector.push_back(8 - (move[1] - '0'));
    moveVector.push_back(move[2] - 97);
    moveVector.push_back(8 - (move[3] - '0'));
    return moveVector;
  }
  std::string fen(Board &_board);
  void computerMove();
  bool isInCheck(int color, Board &_board);
  std::array<bool, 2> canCastle(int x, int y, Board &_board);
  std::vector<std::vector<int>> legalMoves(int x, int y, int sideToMove,
                                           Board &_board);
  void placeTo(int x1, int y1, int x2, int y2, Board &_board,
               bool playSound = false);
  bool moveTo(int x1, int y1, int x2, int y2);
};
