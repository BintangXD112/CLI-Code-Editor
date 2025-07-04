#include "editor.h"
#include "util.h"
#include <stdlib.h>

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