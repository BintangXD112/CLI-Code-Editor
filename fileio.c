#include "fileio.h"
#include "editor.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void editorAppendRow(char *s, size_t len) {
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    int at = E.numrows;
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';
    E.numrows++;
    E.dirty++;
}

void editorOpen(char *filename) {
    free(E.filename);
    E.filename = strdup(filename);
    FILE *fp = fopen(filename, "r");
    if (!fp) die("fopen");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorAppendRow(line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave() {
    if (E.filename == NULL) return;
    FILE *fp = fopen(E.filename, "w");
    if (!fp) {
        snprintf(E.statusmsg, sizeof(E.statusmsg), "Can't save! I/O error: %s", strerror(errno));
        return;
    }
    for (int j = 0; j < E.numrows; j++) {
        fwrite(E.row[j].chars, 1, E.row[j].size, fp);
        fputc('\n', fp);
    }
    fclose(fp);
    E.dirty = 0;
    snprintf(E.statusmsg, sizeof(E.statusmsg), "%d lines written to disk", E.numrows);
} 