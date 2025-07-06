#include "screen.h"
#include "editor.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define EDITOR_VERSION "0.0.1"

// Pewarnaan ANSI
#define HL_NORMAL 0
#define HL_KEYWORD 1
#define HL_STRING 2
#define HL_COMMENT 3
#define HL_NUMBER 4

const char *C_keywords[] = {
    "int", "char", "if", "else", "for", "while", "return", "void", "include", NULL
};

int is_c_keyword(const char *s, int len) {
    for (int i = 0; C_keywords[i]; i++) {
        if ((int)strlen(C_keywords[i]) == len && !strncmp(s, C_keywords[i], len))
            return 1;
    }
    return 0;
}

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
            // Line numbering
            char lnbuf[16];
            snprintf(lnbuf, sizeof(lnbuf), "%4d ", filerow + 1);
            write(STDOUT_FILENO, "\x1b[90m", 5); // abu-abu
            write(STDOUT_FILENO, lnbuf, strlen(lnbuf));
            write(STDOUT_FILENO, "\x1b[39m", 5);
            char *row = E.row[filerow].chars;
            int len = E.row[filerow].size - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols - 5) len = E.screencols - 5;
            int i = 0;
            while (i < len) {
                // Komentar C
                if (row[i] == '/' && row[i+1] == '/') {
                    write(STDOUT_FILENO, "\x1b[32m", 5); // hijau
                    write(STDOUT_FILENO, &row[i], len - i);
                    write(STDOUT_FILENO, "\x1b[39m", 5);
                    break;
                }
                // String
                if (row[i] == '"') {
                    write(STDOUT_FILENO, "\x1b[35m", 5); // magenta
                    write(STDOUT_FILENO, "\"", 1);
                    i++;
                    while (i < len && row[i] != '"') {
                        write(STDOUT_FILENO, &row[i], 1);
                        i++;
                    }
                    if (i < len) {
                        write(STDOUT_FILENO, "\"", 1);
                        i++;
                    }
                    write(STDOUT_FILENO, "\x1b[39m", 5);
                    continue;
                }
                // Keyword
                if (isalpha(row[i]) || row[i] == '_') {
                    int start = i;
                    while (i < len && (isalnum(row[i]) || row[i] == '_')) i++;
                    if (is_c_keyword(&row[start], i - start)) {
                        write(STDOUT_FILENO, "\x1b[34m", 5); // biru
                        write(STDOUT_FILENO, &row[start], i - start);
                        write(STDOUT_FILENO, "\x1b[39m", 5);
                        continue;
                    } else {
                        write(STDOUT_FILENO, &row[start], i - start);
                        continue;
                    }
                }
                // Angka
                if (isdigit(row[i])) {
                    write(STDOUT_FILENO, "\x1b[36m", 5); // cyan
                    int start = i;
                    while (i < len && isdigit(row[i])) i++;
                    write(STDOUT_FILENO, &row[start], i - start);
                    write(STDOUT_FILENO, "\x1b[39m", 5);
                    continue;
                }
                // Markdown heading
                if (row[i] == '#' && (i == 0 || row[i-1] == ' ')) {
                    write(STDOUT_FILENO, "\x1b[33m", 5); // kuning
                    write(STDOUT_FILENO, &row[i], 1);
                    write(STDOUT_FILENO, "\x1b[39m", 5);
                    i++;
                    continue;
                }
                write(STDOUT_FILENO, &row[i], 1);
                i++;
            }
        }
        write(STDOUT_FILENO, "\x1b[K", 3);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}

void editorDrawStatusBar() {
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), " %.20s - %d lines %s",
        E.filename ? E.filename : "[No Name]", E.numrows, E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), " %d/%d ", E.cy + 1, E.numrows);
    if (len > E.screencols) len = E.screencols;
    write(STDOUT_FILENO, "\x1b[7m", 4); // Inverse colors
    write(STDOUT_FILENO, status, len);
    while (len < E.screencols) {
        if (E.screencols - len == rlen) {
            write(STDOUT_FILENO, rstatus, rlen);
            len += rlen;
        } else {
            write(STDOUT_FILENO, " ", 1);
            len++;
        }
    }
    write(STDOUT_FILENO, "\x1b[m", 3);
    write(STDOUT_FILENO, "\r\n", 2);
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[?25l", 6);
    write(STDOUT_FILENO, "\x1b[H", 3);
    editorDrawRows();
    editorDrawStatusBar();
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.cx - E.coloff) + 1);
    write(STDOUT_FILENO, buf, strlen(buf));
    write(STDOUT_FILENO, "\x1b[?25h", 6);
} 