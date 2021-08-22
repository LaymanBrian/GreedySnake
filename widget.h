#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "macro.h"
#include <QPaintEvent>
#include <QList>
#include <QPair>
#include <QSet>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void timerEvent(QTimerEvent*) override;

private:
    void drawBoard();
    void drawSnake();
    void drawFood();

    void moveSnake();
    void check();
    void init();
    void run();

private:
    unsigned int score = 0;
    BLOCK_COLOR board[ROW_COUNT][COLUMN_COUNT] = {};
    QList<QPair<int, int>> snake;
    QSet<QPair<int, int>> not_used;

    DIRECTION dir = DIRECTION::UP;
    DIRECTION dir_next = dir;
    GAME_STATE state = GAME_STATE::NOT_START;
    bool exist_food = false;
    QPair<int, int> food_pos = {-1, -1};

    int timer_id = -1;
};
#endif // WIDGET_H
