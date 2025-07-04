// basic_editor.c
// Simple CLI text editor in C (Linux/Unix)
// Compile: gcc -o basic_editor basic_editor.c

#include "util.h"
#include "editor.h"
#include "input.h"
#include "screen.h"
#include "fileio.h"

#define CTRL_KEY(k) ((k) & 0x1f)

int main(int argc, char *argv[]) {
    enableRawMode();
    initEditor();
    if (argc >= 2) {
        editorOpen(argv[1]);
    }
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
} 