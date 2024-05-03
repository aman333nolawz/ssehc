#include "chess.hpp"
#include "subprocess.hpp"
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace sp = subprocess;

sp::popen process("stockfish", {});

Engine::Engine(Board &board) {
  this->board = board;
  process.in() << "setoption name Skill Level value 0 " << std::endl;
  process.in() << "ucinewgame" << std::endl;
}

void Engine::newGame() {}

std::vector<int> Engine::bestMove(std::string moves) {
  std::string move;

  process.in() << "position startpos moves " << moves << std::endl;
  process.in() << "go depth 10" << std::endl;
  while (std::getline(process.out(), move)) {
    if (move.rfind("bestmove", 0) == 0) {
      break;
    }
  }
  std::istringstream ss(move);
  ss >> move;
  ss >> move;

  auto out = Chess::UciToPos(move);
  return out;
};
