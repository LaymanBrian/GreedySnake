#ifndef MACRO_H
#define MACRO_H

#define ROW_COUNT 31
#define COLUMN_COUNT 31
#define BLOCK_WIDTH 20
#define BLOCK_HEIGHT 20
#define LINE_WIDTH 2
#define SLEEP_TIME 200ms

enum BLOCK_COLOR {
    NONE,//nothing
    YELLO,//snake
    RED//food
};

enum DIRECTION {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum GAME_STATE {
    NOT_START,
    RUNNING,
    GAME_OVER,
    WIN
};

#endif // MACRO_H
