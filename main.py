import pygame
from board import Board

pygame.init()

W, H = 600, 600
SQ_SIZE = W // 8

screen = pygame.display.set_mode((W, H))
pygame.display.set_caption("Chess")

board = Board(screen, W, H, SQ_SIZE)

while True:
    screen.fill("#282828")
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

        elif event.type == pygame.MOUSEBUTTONDOWN:
            board.select_square(pygame.mouse.get_pos())

    board.draw_board()
    board.draw_pieces()

    if board.selected_sq:
        board.draw_legal_moves(board.legal_moves_for_piece(board.selected_sq))

    pygame.display.flip()
