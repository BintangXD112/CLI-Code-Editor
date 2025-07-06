#include "input.h"
#include "editor.h"
#include "util.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        }
        return '\x1b';
    } else {
        return c;
    }
}

void editorMoveCursor(int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    switch (key) {
        case ARROW_LEFT:
            if (E.cx != 0) {
                E.cx--;
            } else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (row && E.cx < row->size) {
                E.cx++;
            } else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
            if (E.cy != 0) {
                E.cy--;
            }
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows) {
                E.cy++;
            }
            break;
    }
    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}

/***********************
 * PROMPT & UTILITIES  *
 ***********************/

char *editorPrompt(const char *prompt) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';
    while (1) {
        snprintf(E.statusmsg, sizeof(E.statusmsg), prompt, buf);
        editorRefreshScreen();
        int c = editorReadKey();
        if (c == '\r') {
            if (buflen != 0) {
                E.statusmsg[0] = '\0';
                return buf;
            }
        } else if (c == 127 || c == 8) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (!iscntrl(c) && c < 128 && buflen < bufsize - 1) {
            buf[buflen++] = c;
            buf[buflen] = '\0';
        } else if (c == 27) { // ESC
            E.statusmsg[0] = '\0';
            free(buf);
            return NULL;
        }
    }
}

/***********************
 * SEARCH & REPLACE    *
 ***********************/

void editorFind() {
    char *query = editorPrompt("Find: %s (ESC to cancel)");
    if (!query) return;
    for (int i = 0; i < E.numrows; i++) {
        char *match = strstr(E.row[i].chars, query);
        if (match) {
            E.cy = i;
            E.cx = match - E.row[i].chars;
            E.rowoff = E.numrows; // Scroll ke bawah agar hasil terlihat
            break;
        }
    }
    free(query);
}

void editorReplace() {
    char *find = editorPrompt("Replace: %s (ESC to cancel)");
    if (!find) return;
    char *replace = editorPrompt("Replace with: %s (ESC to cancel)");
    if (!replace) { free(find); return; }
    int replaced = 0;
    for (int i = 0; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *p = row->chars;
        while ((p = strstr(p, find))) {
            int pos = p - row->chars;
            int newsize = row->size - strlen(find) + strlen(replace);
            char *newchars = malloc(newsize + 1);
            memcpy(newchars, row->chars, pos);
            memcpy(newchars + pos, replace, strlen(replace));
            strcpy(newchars + pos + strlen(replace), row->chars + pos + strlen(find));
            free(row->chars);
            row->chars = newchars;
            row->size = newsize;
            p = row->chars + pos + strlen(replace);
            replaced++;
            E.dirty++;
        }
    }
    snprintf(E.statusmsg, sizeof(E.statusmsg), "Replaced %d occurrence(s)", replaced);
    free(find);
    free(replace);
}

/***********************
 * KEY PROCESSING      *
 ***********************/

void editorProcessKeypress() {
    int c = editorReadKey();
    switch (c) {
        case 26: // CTRL_KEY('z')
            editorUndo();
            break;
        case 25: // CTRL_KEY('y')
            editorRedo();
            break;
        case 17: // CTRL_KEY('q')
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        case 19: // CTRL_KEY('s')
            extern void editorSave();
            editorSave();
            break;
        case '\r': // Enter
            editorSaveSnapshot();
            extern void editorInsertNewline();
            editorInsertNewline();
            break;
        case 127: // Backspace
        case 8:
            editorSaveSnapshot();
            extern void editorDelChar();
            editorDelChar();
            break;
        case DEL_KEY:
            editorSaveSnapshot();
            editorMoveCursor(ARROW_RIGHT);
            extern void editorDelChar();
            editorDelChar();
            break;
        case 6: // CTRL_KEY('f')
            editorFind();
            break;
        case 18: // CTRL_KEY('r')
            editorSaveSnapshot();
            editorReplace();
            break;
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
        default:
            if (isprint(c)) {
                editorSaveSnapshot();
                extern void editorInsertChar(int c);
                editorInsertChar(c);
            }
            break;
    }
} 