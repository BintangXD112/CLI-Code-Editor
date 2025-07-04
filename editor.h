#ifndef EDITOR_H
#define EDITOR_H

#include <time.h>

typedef struct erow {
    int size;
    char *chars;
} erow;

struct editorConfig {
    int cx, cy;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    erow *row;
    int dirty;
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
};

extern struct editorConfig E;

void initEditor();

#endif // EDITOR_H 