import curses


def draw(canvas):
    row, col = canvas.getmaxyx()
    curs_x, curs_y = 0, 0
    canvas.addstr(row - 2, 1, "NORMAL", curses.A_BOLD)
    canvas.addstr(row - 2, col - 4, f"{curs_x}, {curs_y}")
    canvas.move(curs_x, curs_y)
    canvas.refresh()

    while True:
        char = canvas.getkey()
        if char == "h":
            curs_x -= 1
        elif char == "j":
            curs_y += 1
        elif char == "k":
            curs_y -= 1
        elif char == "l":
            curs_x += 1

        curs_x = max(curs_x, 0)
        curs_y = max(curs_y, 0)

        canvas.addstr(row - 2, col - 4, f"{curs_x}, {curs_y}")
        canvas.move(curs_y, curs_x)


if __name__ == "__main__":
    curses.update_lines_cols()
    curses.wrapper(draw)
