#ifndef INPUT_H
#define INPUT_H

int editorReadKey();
void editorProcessKeypress();
void editorMoveCursor(int key);
char *editorPrompt(const char *prompt);
void editorFind();
void editorReplace();

enum editorKey {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

#endif // INPUT_H 