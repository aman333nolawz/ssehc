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
board = Board(screen, W, H, SQ_SIZE)

while True:
    win.fill("#282A36")
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

        elif event.type == pygame.MOUSEBUTTONDOWN:
            if board.turn == 1:
                board.select_square(pygame.mouse.get_pos())

    board.draw_board()
    board.draw_pieces()

    if board.turn == 0:
        board.computer_move()

    # if board.pov_score:
    # white_score = font.render(str(board.pov_score.white()), True, "#F8F8F2")
    # black_score = rm -rf __font.render(str(board.pov_score.black()), True, "#F8F8F2")
    # win.blit(black_score, (W + 20, 30))
    # win.blit(white_score, (W + 20, H - 30))
    # print("WHITE:", board.pov_score.white())
    # print("BLACK:", board.pov_score.black())

    if board.selected_sq:
        board.draw_legal_moves(board.legal_moves_for_piece(board.selected_sq))

    win.blit(screen, (0, 0))
    pygame.display.flip()
