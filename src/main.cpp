#include "chess.cpp"
#include "constants.cpp"
#include <algorithm>
#include <memory>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include <map>

const int screenW = W + 170;
const int screenH = H;
const Color colors[2] = {Color({0xf3, 0xf3, 0xf4, 0xff}),
                         Color({0x6a, 0x9b, 0x41, 0xff})};

bool flipped = false;
Chess chess;
int selected[2] = {-1, -1};
int hoveringTile[2] = {4, 0};
std::map<char, Texture2D> pieceTextures;
bool active = true;

void drawMoves(int x, int y) {
  if (x < 0)
    return;

  for (std::vector<int> move : chess.legalMoves(
           flipped ? 7 - x : x, flipped ? 7 - y : y, chess.turn, chess.board)) {
    DrawCircle((flipped ? 7 - move[0] : move[0]) * SQ + SQ / 2,
               (flipped ? 7 - move[1] : move[1]) * SQ + SQ / 2, SQ / 4,
               ColorAlpha(BLACK, 0.3));
  }
}

void drawPiece(int x, int y) {
  std::shared_ptr<Piece> piece;
  if (flipped) {
    piece = chess.board[7 - y][7 - x];
  } else {
    piece = chess.board[y][x];
  }

  if ((chess.lastMove[0][0] == (flipped ? 7 - x : x) &&
       chess.lastMove[0][1] == (flipped ? 7 - y : y)) ||
      (chess.lastMove[1][0] == (flipped ? 7 - x : x) &&
       chess.lastMove[1][1] == (flipped ? 7 - y : y))) {
    DrawRectangle(x * SQ, y * SQ, SQ, SQ, Color({0xfa, 0xfa, 0x94, 0xaa}));
  }

  if (piece->type == EMPTY)
    return;

  Texture2D texture = pieceTextures[piece->abbrev];
  Rectangle srcRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
  Rectangle dstRec = {(float)x * SQ, (float)y * SQ, SQ, SQ};
  DrawTexturePro(texture, srcRec, dstRec, {0, 0}, 0, WHITE);
}

void UpdateDrawFrame(void) {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      DrawRectangle(x * SQ, y * SQ, SQ, SQ, colors[(x + y) % 2]);
      drawPiece(x, y);
    }
  }
  DrawRectangleLinesEx(
      {(float)hoveringTile[0] * SQ, (float)hoveringTile[1] * SQ, SQ, SQ}, 5.0f,
      BLUE);
  DrawRectangleLinesEx(
      {(float)selected[0] * SQ, (float)selected[1] * SQ, SQ, SQ}, 5, RED);
  drawMoves(selected[0], selected[1]);

  if (GuiButton({W + 10, 10, 150, 55}, "Restart")) {
    chess = Chess();
  }

  if (GuiButton({W + 10, 70, 150, 55}, "Flip")) {
    flipped = !flipped;
  }

  EndDrawing();
}

int main() {
  InitWindow(screenW, screenH, "Chess");
  InitAudioDevice();
  SetTargetFPS(15);

  chess = Chess();

  pieceTextures['p'] = LoadTexture("images/p.png");
  pieceTextures['n'] = LoadTexture("images/n.png");
  pieceTextures['b'] = LoadTexture("images/b.png");
  pieceTextures['r'] = LoadTexture("images/r.png");
  pieceTextures['q'] = LoadTexture("images/q.png");
  pieceTextures['k'] = LoadTexture("images/k.png");

  pieceTextures['P'] = LoadTexture("images/P.png");
  pieceTextures['N'] = LoadTexture("images/N.png");
  pieceTextures['B'] = LoadTexture("images/B.png");
  pieceTextures['R'] = LoadTexture("images/R.png");
  pieceTextures['Q'] = LoadTexture("images/Q.png");
  pieceTextures['K'] = LoadTexture("images/K.png");

  GuiSetStyle(DEFAULT, TEXT_SIZE, 27);

  while (!WindowShouldClose()) {
    UpdateDrawFrame();

    hoveringTile[0] = std::min(GetMouseX() / SQ, 7);
    hoveringTile[1] = std::min(GetMouseY() / SQ, 7);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      int x = GetMouseX() / SQ;
      int y = GetMouseY() / SQ;
      if (x < 8 && y < 8) {

        if (selected[0] == -1) {
          if (chess.board[flipped ? 7 - y : y][flipped ? 7 - x : x]->type !=
                  EMPTY &&
              chess.board[flipped ? 7 - y : y][flipped ? 7 - x : x]->color ==
                  chess.turn) {
            selected[0] = x;
            selected[1] = y;
          }
        } else {
          chess.moveTo(flipped ? 7 - selected[0] : selected[0],
                       flipped ? 7 - selected[1] : selected[1],
                       flipped ? 7 - x : x, flipped ? 7 - y : y);
          selected[0] = -1;
          selected[1] = -1;
        }
      }
    }
  }

  for (auto iter = pieceTextures.begin(); iter != pieceTextures.end(); ++iter) {
    UnloadTexture(iter->second);
  }

  CloseWindow();
}
