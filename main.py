#!/usr/bin/env python
from threading import Thread

import chess.engine
import pygame
import pygame_gui

from board import Board

pygame.init()
pygame.font.init()

W, H = 600, 600
SQ_SIZE = W // 8

win = pygame.display.set_mode((W + 200, H))
screen = pygame.Surface((W, H))
manager = pygame_gui.UIManager(win.get_size(), "theme.json")

screen_x = 0
screen_y = 0
clock = pygame.time.Clock()

pygame.display.set_caption("Chess")

font = pygame.font.SysFont(None, 30)

reset_button = pygame_gui.elements.UIButton(
    pygame.Rect(W + 30 + screen_x, 60 + screen_y, 75, 30), text="Reset", manager=manager
)
flip_button = pygame_gui.elements.UIButton(
    pygame.Rect(W + 30 + screen_x, 100 + screen_y, 75, 30), text="Flip", manager=manager
)
undo_button = pygame_gui.elements.UIButton(
    pygame.Rect(W + 30 + screen_x, 140 + screen_y, 75, 30), text="Undo", manager=manager
)
h_vs_h = pygame_gui.elements.UIDropDownMenu(["Human", "Computer"], "Computer", pygame.Rect(W + 30 + screen_x, 180 + screen_y, 100, 30), manager)

flip = True
opponent, player = chess.BLACK, chess.WHITE
play_with_human = False

def reset_board():
    global flip, board
    board = Board(screen, W, H, SQ_SIZE, engine, manager, flipped=True)
    flip = True


engine = chess.engine.SimpleEngine.popen_uci(r"./engines/stockfish_15_x64")

try:
    reset_board()
    run = True
    while run:
        time_delta = clock.tick(60) / 1000.0
        win.fill("#282A36")
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                x, y = pygame.mouse.get_pos()
                if (
                    (board.turn == player or play_with_human)
                    and x >= screen_x
                    and x <= W
                    and y >= screen_y
                    and y <= H
                ):
                    x, y = x - screen_x, y - screen_y
                    x, y = x // SQ_SIZE, y // SQ_SIZE
                    if flip:
                        x, y = [x, 7 - y]
                    else:
                        x, y = [7 - x, y]
                    board.select_square([x, y])

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_z:
                    board.undo()
                elif event.key == pygame.K_r:
                    reset_board()
                elif event.key == pygame.K_f:
                    flip = not flip
                    board.flipped = not board.flipped
                    board.load_images()
                elif event.key == pygame.K_b:
                    opponent, player = 0, 1
                elif event.key == pygame.K_w:
                    opponent, player = 1, 0

            elif event.type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == flip_button:
                    flip = not flip
                    board.flipped = not board.flipped
                    board.load_images()
                if event.ui_element == undo_button:
                    board.undo()
                if event.ui_element == reset_button:
                    reset_board()

            elif event.type == pygame_gui.UI_SELECTION_LIST_NEW_SELECTION:
                if event.text == "Human":
                    play_with_human = True
                else:
                    play_with_human = False

            manager.process_events(event)

        manager.update(time_delta)

        board.draw_board()
        board.draw_last_move()

        if board.is_check():
            board.draw_check()

        board.draw_pieces()

        if board.turn == opponent and not play_with_human:
            if not board.computer_played:
                Thread(target=board.computer_move).start()

        if board.selected_sq:
            board.draw_legal_moves(board.legal_moves_for_piece(board.selected_sq))

        win.blit(pygame.transform.flip(screen, not flip, flip), (screen_x, screen_y))
        manager.draw_ui(win)
        pygame.display.flip()

    pygame.quit()
finally:
    engine.quit()
