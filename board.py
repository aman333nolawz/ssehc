import math

import chess
import chess.engine
import pygame
from easygui import buttonbox
from pygame import mixer

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


def draw_polygon_alpha(surface, color, points):
    lx, ly = zip(*points)
    min_x, min_y, max_x, max_y = min(lx), min(ly), max(lx), max(ly)
    target_rect = pygame.Rect(min_x, min_y, max_x - min_x, max_y - min_y)
    shape_surf = pygame.Surface(target_rect.size, pygame.SRCALPHA)
    pygame.draw.polygon(shape_surf, color, [(x - min_x, y - min_y) for x, y in points])
    surface.blit(shape_surf, target_rect)


def draw_arrow(screen, lcolor, tricolor, start, end, trirad, thickness=2):
    rad = math.pi / 180
    pygame.draw.line(screen, lcolor, start, end, thickness)
    x1, y1 = start
    x2, y2 = end
    rotation = (math.atan2(start[1] - end[1], end[0] - start[0])) + math.pi / 2
    pygame.draw.polygon(
        screen,
        tricolor,
        (
            (
                end[0] + trirad * math.sin(rotation),
                end[1] + trirad * math.cos(rotation),
            ),
            (
                end[0] + trirad * math.sin(rotation - 120 * rad),
                end[1] + trirad * math.cos(rotation - 120 * rad),
            ),
            (
                end[0] + trirad * math.sin(rotation + 120 * rad),
                end[1] + trirad * math.cos(rotation + 120 * rad),
            ),
        ),
    )


class Board(chess.Board):
    move_sound = mixer.Sound("sounds/Move.ogg")
    capture_sound = mixer.Sound("sounds/Capture.ogg")
    check_sound = mixer.Sound("sounds/Check.ogg")
    start_sound = mixer.Sound("sounds/Start.ogg")

    def __init__(
        self,
        win=None,
        W=None,
        H=None,
        SQ_SIZE=None,
        engine=None,
        manager=None,
        flipped=False,
        analyse=False,
        *args,
        **kwargs,
    ):
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
        self.engine = engine
        self.selected_sq = None
        self.move_analysis = None
        self.computer_played = False
        self.start_sound.play()
        self.flipped = flipped
        self.ponder_move = None
        self.move_last_x = 0
        self.move_last_y = 0
        self.move_color = {
            "brilliant": "#00CCCCAA",
            "best": "#00FF00AA",
            "ok": "#006600AA",
            "inaccuracy": "#FFAA00AA",
            "blunder": "#FF0000AA",
        }
        self.analyse = analyse
        self.pov_score = chess.engine.PovScore(chess.engine.Cp(0), chess.WHITE)
        self.old_pov = chess.engine.PovScore(chess.engine.Cp(0), chess.WHITE)
        self.load_images()

    def load_images(self):
        self.piece_images = [
            pygame.transform.flip(
                pygame.transform.scale(
                    pygame.image.load(f"images/{piece}.png"),
                    (self.SQ_SIZE, self.SQ_SIZE),
                ),
                not self.flipped,
                self.flipped,
            )
            for piece in self.pieces
        ]

    def draw_board(self):
        colors = self.colors[::-1] if self.flipped else self.colors[::-1]
        for y in range(8):
            for x in range(8):
                pygame.draw.rect(
                    self.win,
                    colors[(y + x) % 2],
                    (y * self.SQ_SIZE, x * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
                )

    def draw_pieces(self):
        for y in range(8):
            for x in range(8):
                square = chess.square(x, y)
                piece = self.piece_at(square)
                if piece is None:
                    continue
                self.win.blit(
                    self.piece_images[self.pieces.index(piece.symbol())],
                    (x * self.SQ_SIZE, y * self.SQ_SIZE),
                )
        if not self.ponder_move:
            return
        x1, y1 = chess.square_file(self.ponder_move.from_square), chess.square_rank(
            self.ponder_move.from_square
        )
        x2, y2 = chess.square_file(self.ponder_move.to_square), chess.square_rank(
            self.ponder_move.to_square
        )
        draw_arrow(
            self.win,
            "#F9C24C",
            "#F9C24C",
            (
                x1 * self.SQ_SIZE + self.SQ_SIZE // 2,
                y1 * self.SQ_SIZE + self.SQ_SIZE // 2,
            ),
            (
                x2 * self.SQ_SIZE + self.SQ_SIZE // 2,
                y2 * self.SQ_SIZE + self.SQ_SIZE // 2,
            ),
            self.SQ_SIZE // 4,
            self.SQ_SIZE // 4,
        )

    def draw_legal_moves(self, moves):
        for move in moves:
            x, y = chess.square_file(move.to_square), chess.square_rank(move.to_square)
            draw_circle_alpha(
                self.win,
                (0, 0, 0, 60),
                (
                    x * self.SQ_SIZE + self.SQ_SIZE // 2,
                    y * self.SQ_SIZE + self.SQ_SIZE // 2,
                ),
                self.SQ_SIZE // 6,
            )

    def draw_last_move(self):
        if not self.move_stack:
            return
        last_move = self.move_stack[-1]
        x1, y1 = (
            chess.square_file(last_move.from_square),
            chess.square_rank(last_move.from_square),
        )
        x2, y2 = (
            chess.square_file(last_move.to_square),
            chess.square_rank(last_move.to_square),
        )
        draw_rect_alpha(
            self.win,
            "#F7F769aa",
            (x1 * self.SQ_SIZE, y1 * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
        )
        if self.move_analysis:
            draw_rect_alpha(
                self.win,
                self.move_color[self.move_analysis],
                (x2 * self.SQ_SIZE, y2 * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
            )
        else:
            draw_rect_alpha(
                self.win,
                "#F7F769aa",
                (x2 * self.SQ_SIZE, y2 * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
            )

    def draw_check(self):
        # king_square = list(super().pieces(chess.KING, self.turn))[0]
        king_square = self.king(self.turn)
        x, y = chess.square_file(king_square), chess.square_rank(king_square)
        draw_rect_alpha(
            self.win,
            "#FF79C6cc",
            (x * self.SQ_SIZE, y * self.SQ_SIZE, self.SQ_SIZE, self.SQ_SIZE),
        )

    def select_square(self, pos):
        if self.selected_sq:
            return self.human_move(pos)
        self.selected_sq = pos

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

    def undo(self, n=2):
        for _ in range(n):
            if not self.move_stack:
                break
            self.pop()

        if self.analyse:
            analysis = self.engine.analyse(self, chess.engine.Limit(time=0.2))
            self.ponder_move = analysis.get("pv", [None])[0]
            self.pov_score = chess.engine.PovScore(chess.engine.Cp(0), chess.WHITE)
            self.old_pov = chess.engine.PovScore(chess.engine.Cp(0), chess.WHITE)

    def _analyse(self):
        analysis = self.engine.analyse(self, chess.engine.Limit(time=0.2))
        self.ponder_move = analysis.get("pv", [None])[0]
        self.pov_score = analysis.get("score")

        if self.turn == chess.BLACK:
            diff = self.pov_score.white().score(
                mate_score=100000
            ) - self.old_pov.white().score(mate_score=100000)
        else:
            diff = self.pov_score.black().score(
                mate_score=100000
            ) - self.old_pov.black().score(mate_score=100000)
        if diff > 100:
            self.move_analysis = "brilliant"
        elif diff > -50:
            self.move_analysis = "best"
        elif diff > -100:
            self.move_analysis = "ok"
        elif diff > -200:
            self.move_analysis = "inaccuracy"
        elif diff <= -200:
            self.move_analysis = "blunder"

    def push(self, move, play_sound=False, ponder_move=False):
        if not play_sound:
            return super().push(move)
        sound = self.move_sound
        if self.is_capture(move):
            sound = self.capture_sound

        self.old_pov = self.pov_score
        super().push(move)
        sound.play()

        self.move_last_x += 1
        if self.move_last_x > 1:
            self.move_last_x = 0
            self.move_last_y += 1

        if not self.analyse:
            return
        self._analyse()

    def human_move(self, end_pos, ponder_move=True):
        if not self.selected_sq:
            return

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
                    if promotion_piece == "queen":
                        move.promotion = chess.QUEEN
                    elif promotion_piece == "rook":
                        move.promotion = chess.ROOK
                    elif promotion_piece == "knight":
                        move.promotion = chess.KNIGHT
                    elif promotion_piece == "bishop":
                        move.promotion = chess.BISHOP
        if self.is_legal(move):
            self.push(move, True, ponder_move)
        self.selected_sq = None
        self.computer_played = False

    def computer_move(self, ponder_move=True):
        self.computer_played = True
        result = self.engine.play(self, chess.engine.Limit(depth=15))
        self.push(result.move, True, ponder_move)
        self.computer_played = False

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
