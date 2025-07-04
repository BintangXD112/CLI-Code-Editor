#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

void editorOpen(char *filename);
void editorSave();
void editorAppendRow(char *s, size_t len);

#endif // FILEIO_H 