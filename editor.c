#include "editor.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#define UNDO_STACK_SIZE 32

typedef struct editorSnapshot {
    int numrows;
    erow *row;
    int cx, cy;
} editorSnapshot;

static editorSnapshot undoStack[UNDO_STACK_SIZE];
static int undoTop = 0, redoTop = 0;

void freeSnapshot(editorSnapshot *snap) {
    for (int i = 0; i < snap->numrows; i++) {
        free(snap->row[i].chars);
    }
    free(snap->row);
}

editorSnapshot makeSnapshot() {
    editorSnapshot snap;
    snap.numrows = E.numrows;
    snap.row = malloc(sizeof(erow) * E.numrows);
    for (int i = 0; i < E.numrows; i++) {
        snap.row[i].size = E.row[i].size;
        snap.row[i].chars = strdup(E.row[i].chars);
    }
    snap.cx = E.cx;
    snap.cy = E.cy;
    return snap;
}

void loadSnapshot(editorSnapshot *snap) {
    for (int i = 0; i < E.numrows; i++) free(E.row[i].chars);
    free(E.row);
    E.numrows = snap->numrows;
    E.row = malloc(sizeof(erow) * E.numrows);
    for (int i = 0; i < E.numrows; i++) {
        E.row[i].size = snap->row[i].size;
        E.row[i].chars = strdup(snap->row[i].chars);
    }
    E.cx = snap->cx;
    E.cy = snap->cy;
}

void editorSaveSnapshot() {
    if (undoTop == UNDO_STACK_SIZE) {
        freeSnapshot(&undoStack[0]);
        memmove(&undoStack[0], &undoStack[1], sizeof(editorSnapshot) * (UNDO_STACK_SIZE - 1));
        undoTop--;
    }
    undoStack[undoTop++] = makeSnapshot();
    redoTop = undoTop;
}

void editorUndo() {
    if (undoTop <= 1) return;
    redoTop = undoTop;
    undoTop--;
    loadSnapshot(&undoStack[undoTop - 1]);
}

void editorRedo() {
    if (redoTop >= UNDO_STACK_SIZE || redoTop <= undoTop) return;
    loadSnapshot(&undoStack[redoTop]);
    undoTop = ++redoTop;
}

struct editorConfig E;

int getWindowSize(int *rows, int *cols);

void initEditor() {
    E.cx = 0;
    E.cy = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL;
    E.dirty = 0;
    E.filename = NULL;
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

void editorInsertChar(int c) {
    if (E.cy == E.numrows) {
        // Tambah baris baru jika kursor di bawah file
        editorAppendRow("", 0);
    }
    erow *row = &E.row[E.cy];
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[E.cx + 1], &row->chars[E.cx], row->size - E.cx + 1);
    row->size++;
    row->chars[E.cx] = c;
    E.cx++;
    E.dirty++;
}

void editorDelChar() {
    if (E.cy == E.numrows) return;
    if (E.cx == 0 && E.cy == 0) return;
    erow *row = &E.row[E.cy];
    if (E.cx > 0) {
        memmove(&row->chars[E.cx - 1], &row->chars[E.cx], row->size - E.cx + 1);
        row->size--;
        E.cx--;
        E.dirty++;
    } else {
        // Gabungkan baris ini ke baris sebelumnya
        int prev_size = E.row[E.cy - 1].size;
        E.row[E.cy - 1].chars = realloc(E.row[E.cy - 1].chars, prev_size + row->size + 1);
        memcpy(&E.row[E.cy - 1].chars[prev_size], row->chars, row->size + 1);
        E.row[E.cy - 1].size += row->size;
        free(row->chars);
        memmove(&E.row[E.cy], &E.row[E.cy + 1], sizeof(erow) * (E.numrows - E.cy - 1));
        E.numrows--;
        E.cy--;
        E.cx = prev_size;
        E.dirty++;
    }
}

void editorInsertNewline() {
    if (E.cx == 0) {
        editorAppendRow("", 0);
        for (int j = E.numrows - 1; j > E.cy; j--) {
            E.row[j] = E.row[j - 1];
        }
        E.row[E.cy + 1].chars = strdup("");
        E.row[E.cy + 1].size = 0;
        E.cy++;
        E.cx = 0;
    } else {
        erow *row = &E.row[E.cy];
        editorAppendRow(&row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        for (int j = E.numrows - 1; j > E.cy + 1; j--) {
            E.row[j] = E.row[j - 1];
        }
        E.cy++;
        E.cx = 0;
    }
    E.dirty++;
} 