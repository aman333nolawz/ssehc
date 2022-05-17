import chess
import chess.engine
import pygame
from pygame import mixer
from easygui import buttonbox


pygame.init()


def draw_circle_alpha(surface, color, center, radius):
    target_rect = pygame.Rect(center, (0, 0)).inflate((radius * 2, radius * 2))
    shape_surf = pygame.Surface(target_rect.size, pygame.SRCALPHA)
    pygame.draw.circle(shape_surf, color, (radius, radius), radius)
    surface.blit(shape_surf, target_rect)


def draw_rect_alpha(surface, color, rect):
    shape_surf = pygame.Surface(pygame.Rect(rect).size, pygame.SRCALPHA)
    pygame.draw.rect(shape_surf, color, shape_surf.get_rect())
    surface.blit(shape_surf, rect)


class Board(chess.Board):
    move_sound = mixer.Sound("sounds/Move.ogg")
    capture_sound = mixer.Sound("sounds/Capture.ogg")
    check_sound = mixer.Sound("sounds/Check.ogg")
    start_sound = mixer.Sound("sounds/Start.ogg")

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
        self.computer_played = False
        self.start_sound.play()

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
        # print(self.board)
        # exit()
        for y, row in enumerate(self.board):
            row = row[::-1]
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
            x, y = self.convert_xy(x, y)
            draw_circle_alpha(
                self.win,
                (0, 0, 0, 127),
                (
                    x * self.SQ_SIZE + self.SQ_SIZE // 2,
                    y * self.SQ_SIZE + self.SQ_SIZE // 2,
                ),
                self.SQ_SIZE // 4,
            )

    def draw_last_move(self):
        if not self.move_stack:
            return
        last_move = self.move_stack[-1]
        x1, y1 = self.convert_xy(
            chess.square_file(last_move.from_square),
            chess.square_rank(last_move.from_square),
        )
        x2, y2 = self.convert_xy(
            chess.square_file(last_move.to_square),
            chess.square_rank(last_move.to_square),
        )
        draw_rect_alpha(
            self.win,
            "#F7F769aa",
            (x1 * self.SQ_SIZE, y1 * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
        )
        draw_rect_alpha(
            self.win,
            "#F7F769aa",
            (x2 * self.SQ_SIZE, y2 * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
        )

    def draw_check(self):
        # king_square = list(super().pieces(chess.KING, self.turn))[0]
        king_square = self.king(self.turn)
        x, y = chess.square_file(king_square), chess.square_rank(king_square)
        x, y = self.convert_xy(x, y)
        draw_rect_alpha(
            self.win,
            "#FF79C6cc",
            (x * self.SQ_SIZE, y * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
        )

    def convert_xy(self, x, y):
        return x, 7 - y

    def select_square(self, pos):
        if self.selected_sq:
            return self.human_move(pos)
        self.selected_sq = [axis // self.SQ_SIZE for axis in pos]
        self.selected_sq = self.convert_xy(*self.selected_sq)

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

    def undo(self):
        if not self.move_stack:
            return
        self.pop()

        if not self.move_stack:
            return
        self.pop()

    def push(self, move):
        sound = self.move_sound
        if self.is_capture(move):
            sound = self.capture_sound

        super().push(move)
        if self.is_check():
            print("check")
            sound = self.check_sound
        sound.play()

    def human_move(self, end_pos):
        if not self.selected_sq:
            return

        end_pos = [axis // self.SQ_SIZE for axis in end_pos]
        end_pos = self.convert_xy(*end_pos)

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
            # self.move_sound.play()
        self.selected_sq = None

    def computer_move(self, engine):
        self.computer_played = True
        result = engine.play(self, chess.engine.Limit(time=5))
        self.push(result.move)
        # self.move_sound.play()
        self.computer_played = False
        # self.pov_score = engine.analyse(self, chess.engine.Limit(time=5)).get("score")

    def __str__(self):
        builder = []

        mirror = self
        for square in chess.SQUARES_180:
            piece = mirror.piece_at(square)

            if piece:
                builder.append(piece.symbol())
            else:
                builder.append(".")

            if chess.BB_SQUARES[square] & chess.BB_FILE_H:
                if square != chess.H1:
                    builder.append("\n")
            else:
                builder.append(" ")

        return "".join(builder)[::-1]
