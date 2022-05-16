import chess
import chess.engine
import pygame
from easygui import buttonbox


def draw_circle_alpha(surface, color, center, radius):
    target_rect = pygame.Rect(center, (0, 0)).inflate((radius * 2, radius * 2))
    shape_surf = pygame.Surface(target_rect.size, pygame.SRCALPHA)
    pygame.draw.circle(shape_surf, color, (radius, radius), radius)
    surface.blit(shape_surf, target_rect)


class Board(chess.Board):
    def __init__(self, win=None, W=None, H=None, SQ_SIZE=None, *args, **kwargs):
        super().__init__(*args, **kwargs)
        if win == None:
            return
        self.win = win
        self.W = W
        self.H = H
        self.SQ_SIZE = SQ_SIZE
        self.colors = ["#EEEED2", "#769656"]
        self.pieces = ["p", "k", "n", "r", "b", "q"]
        self.pieces += [i.upper() for i in self.pieces]
        self.piece_images = [
            pygame.transform.scale(
                pygame.image.load(f"images/{piece}.png"), (self.SQ_SIZE, self.SQ_SIZE)
            )
            for piece in self.pieces
        ]
        self.selected_sq = None
        self.pov_score = None

    def draw_board(self):
        for y in range(8):
            for x in range(8):
                pygame.draw.rect(
                    self.win,
                    self.colors[(y + x) % 2],
                    (y * self.SQ_SIZE, x * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
                )

    def draw_pieces(self):
        self.board = str(self).replace(" ", "").split("\n")[::-1]
        for y, row in enumerate(self.board):
            for x, cell in enumerate(row):
                if cell == ".":
                    continue
                self.win.blit(
                    self.piece_images[self.pieces.index(cell)],
                    (x * self.SQ_SIZE, y * self.SQ_SIZE),
                )

    def draw_legal_moves(self, moves):
        for move in moves:
            x, y = chess.square_file(move.to_square), chess.square_rank(move.to_square)
            x, y = 7 - x, 7 - y
            draw_circle_alpha(
                self.win,
                (0, 0, 0, 127),
                (
                    x * self.SQ_SIZE + self.SQ_SIZE // 2,
                    y * self.SQ_SIZE + self.SQ_SIZE // 2,
                ),
                self.SQ_SIZE // 4,
            )

    def select_square(self, pos):
        if self.selected_sq:
            return self.human_move(pos)
        self.selected_sq = [axis // self.SQ_SIZE for axis in pos]
        self.selected_sq[0] = 7 - self.selected_sq[0]
        self.selected_sq[1] = 7 - self.selected_sq[1]

    def convert_to_pieces(self, piece):
        if type(piece) == str:
            try:
                return chess.Piece(
                    chess.PIECE_NAMES.index(piece.lower()), piece.islower()
                )
            except:
                return chess.Piece(
                    chess.PIECE_SYMBOLS.index(piece.lower()), piece.islower()
                )

        return piece

    def legal_moves_for_piece(self, piece):
        legal_moves = []
        piece = chess.square(*piece)

        for legal_move in self.legal_moves:
            if legal_move.from_square == piece:
                legal_moves.append(legal_move)
        return legal_moves

    def human_move(self, end_pos):
        if not self.selected_sq:
            return

        end_pos = [axis // self.SQ_SIZE for axis in end_pos]
        end_pos[0] = 7 - end_pos[0]
        end_pos[1] = 7 - end_pos[1]

        from_square = chess.square(*self.selected_sq)
        to_square = chess.square(*end_pos)
        moving_piece = self.piece_at(from_square)

        move = chess.Move(from_square, to_square)

        if moving_piece and moving_piece.piece_type == chess.PAWN:
            if self.is_legal(chess.Move(from_square, to_square, chess.QUEEN)):
                if (
                    moving_piece.color == chess.WHITE and to_square in range(56, 64)
                ) or (moving_piece.color == chess.BLACK and to_square in range(8)):
                    promotion_piece = buttonbox(
                        "What do you want to promote to?",
                        "Promotion",
                        ["queen", "rook", "knight", "bishop"],
                    )
                    move.promotion = self.convert_to_pieces(promotion_piece).piece_type
        if self.is_legal(move):
            self.push(move)
        self.selected_sq = None

    def computer_move(self):
        engine = None
        try:
            engine = chess.engine.SimpleEngine.popen_uci(r"./engines/stockfish_15_x64")
            result = engine.play(self, chess.engine.Limit(time=5))
            self.push(result.move)
            # self.pov_score = engine.analyse(self, chess.engine.Limit(time=5)).get("score")
        finally:
            if engine:
                engine.quit()

    def __str__(self):
        return super().__str__()[::-1]
