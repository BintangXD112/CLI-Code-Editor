#include "screen.h"
#include "editor.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EDITOR_VERSION "0.0.1"

void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "Basic Editor -- version %s", EDITOR_VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;
                int padding = (E.screencols - welcomelen) / 2;
                if (padding) {
                    write(STDOUT_FILENO, "~", 1);
                    padding--;
                }
                while (padding--) write(STDOUT_FILENO, " ", 1);
                write(STDOUT_FILENO, welcome, welcomelen);
            } else {
                write(STDOUT_FILENO, "~", 1);
            }
        } else {
            int len = E.row[filerow].size - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            write(STDOUT_FILENO, &E.row[filerow].chars[E.coloff], len);
        }
        write(STDOUT_FILENO, "\x1b[K", 3);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[?25l", 6);
    write(STDOUT_FILENO, "\x1b[H", 3);
    editorDrawRows();
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.cx - E.coloff) + 1);
    write(STDOUT_FILENO, buf, strlen(buf));
    write(STDOUT_FILENO, "\x1b[?25h", 6);
} 