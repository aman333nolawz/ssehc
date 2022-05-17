from threading import Thread
import pygame
from board import Board
import chess.engine

pygame.init()
pygame.font.init()

W, H = 600, 600
SQ_SIZE = W // 8

win = pygame.display.set_mode((W + 100, H))
screen = pygame.Surface((W, H))

pygame.display.set_caption("Chess")


font = pygame.font.SysFont("Fira Code Medium", 30)
engine = chess.engine.SimpleEngine.popen_uci(r"./engines/stockfish_15_x64")

try:
    board = Board(screen, W, H, SQ_SIZE, engine, flipped=True)
    run = True
    while run:
        win.fill("#282A36")
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if board.turn == 1:
                    x, y = pygame.mouse.get_pos()
                    x, y = x // SQ_SIZE, y // SQ_SIZE
                    board.select_square([x, 7 - y])

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_z:
                    board.undo()
                elif event.key == pygame.K_r:
                    board = Board(screen, W, H, SQ_SIZE, engine, flipped=False)

        board.draw_board()
        board.draw_last_move()

        if board.is_check():
            board.draw_check()

        board.draw_pieces()

        if board.turn == 0:
            if not board.computer_played:
                Thread(target=board.computer_move).start()

        if board.pov_score:
            white_score = font.render(str(board.pov_score.white()), True, "#F8F8F2")
            black_score = font.render(str(board.pov_score.black()), True, "#F8F8F2")
            win.blit(black_score, (W + 20, 30))
            win.blit(white_score, (W + 20, H - 30))

        if board.selected_sq:
            board.draw_legal_moves(board.legal_moves_for_piece(board.selected_sq))

        win.blit(pygame.transform.flip(screen, False, True), (0, 0))
        pygame.display.flip()

    pygame.quit()
finally:
    engine.quit()
