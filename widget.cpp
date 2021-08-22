#include "widget.h"
#include <QPainter>
#include <QDebug>
#include <QPalette>
#include <QMessageBox>
#include <chrono>

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(BLOCK_WIDTH * COLUMN_COUNT, BLOCK_HEIGHT * ROW_COUNT);
    QPalette p;
    p.setBrush(QPalette::Window, QBrush(QColor(200, 200, 200)));
    this->setPalette(p);
    init();
}

Widget::~Widget() {

}

void Widget::run() {
    state = GAME_STATE::RUNNING;
    if(timer_id != -1) {
        this->killTimer(timer_id);
        timer_id = -1;
    }
    timer_id = this->startTimer(SLEEP_TIME);
}

void Widget::timerEvent(QTimerEvent*) {
    if(state == GAME_STATE::RUNNING) {
        moveSnake();
        check();
        update();
    }
    if(state == GAME_STATE::RUNNING) return;
    if(state == GAME_STATE::GAME_OVER)
        QMessageBox::information(this, "Game Over", QString("You lost this game with scores:%1").arg(score));
    else if(state == GAME_STATE::WIN)
        QMessageBox::information(this, "Win", QString("You won this game with scores:%1").arg(score));
    if(timer_id != -1) {
        killTimer(timer_id);
        timer_id = -1;
    }
    init();
}

void Widget::init() {
    srand(time(0));
    exist_food = false;
    food_pos = {-1, -1};
    timer_id = -1;
    score = 0;
    not_used.clear();
    snake.clear();
    dir = DIRECTION::UP;
    dir_next = dir;
    state = GAME_STATE::NOT_START;

    for(int i = 0; i < ROW_COUNT; ++i) {
        for(int j = 0; j < COLUMN_COUNT; ++j) {
            not_used.insert(qMakePair(i, j));
            board[i][j] = BLOCK_COLOR::NONE;
        }
    }
    //初始蛇的身体
    snake.emplaceBack(ROW_COUNT / 2, COLUMN_COUNT / 2);
    board[ROW_COUNT / 2][COLUMN_COUNT / 2] = BLOCK_COLOR::YELLO;
    not_used.remove(qMakePair(ROW_COUNT / 2, COLUMN_COUNT / 2));

    snake.emplaceBack(ROW_COUNT / 2 + 1, COLUMN_COUNT / 2);
    board[ROW_COUNT / 2 + 1][COLUMN_COUNT / 2] = BLOCK_COLOR::YELLO;
    not_used.remove(qMakePair(ROW_COUNT / 2 + 1, COLUMN_COUNT / 2));

    snake.emplaceBack(ROW_COUNT / 2 + 2, COLUMN_COUNT / 2);
    board[ROW_COUNT / 2 + 2][COLUMN_COUNT / 2] = BLOCK_COLOR::YELLO;
    not_used.remove(qMakePair(ROW_COUNT / 2 + 2, COLUMN_COUNT / 2));

    update();
}

void Widget::paintEvent(QPaintEvent *) {
    drawSnake();
    drawFood();
    drawBoard();
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if(state != GAME_STATE::RUNNING) {
        run();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up: {
        if(dir == DIRECTION::DOWN) break;
        dir_next = DIRECTION::UP;
        break;
    }
    case Qt::Key_Down: {
        if(dir == DIRECTION::UP) break;
        dir_next = DIRECTION::DOWN;
        break;
    }
    case Qt::Key_Left: {
        if(dir == DIRECTION::RIGHT) break;
        dir_next = DIRECTION::LEFT;
        break;
    }
    case Qt::Key_Right: {
        if(dir == DIRECTION::LEFT) break;
        dir_next = DIRECTION::RIGHT;
        break;
    }
    }

}

void Widget::drawBoard() {
    QPainter painter(this);
    QPen pen;
    pen.setWidth(LINE_WIDTH);
    for(int i = 1; i < ROW_COUNT; ++i) {
        painter.drawLine(0, BLOCK_HEIGHT * i, BLOCK_WIDTH * ROW_COUNT, BLOCK_HEIGHT * i);
        painter.drawLine(BLOCK_WIDTH * i, 0, BLOCK_WIDTH * i, BLOCK_HEIGHT * COLUMN_COUNT);
    }
}

void Widget::drawSnake() {
    QPainter painter(this);
    QBrush brush(Qt::yellow);
    painter.setBrush(brush);
    for(auto node : snake) {
        painter.drawRect(node.second * BLOCK_WIDTH,
                         node.first * BLOCK_HEIGHT,
                         BLOCK_WIDTH,
                         BLOCK_HEIGHT);
    }
}

void Widget::drawFood() {
    if(state != GAME_STATE::RUNNING) return;
    QPainter painter(this);
    QBrush brush(Qt::red);
    painter.setBrush(brush);
    if(exist_food) {
        painter.drawRect(food_pos.second * BLOCK_WIDTH,
                         food_pos.first * BLOCK_HEIGHT,
                         BLOCK_WIDTH,
                         BLOCK_HEIGHT);
        return;
    }
    if(not_used.empty()) {
        state = GAME_STATE::WIN;
        return;
    }
    int n = not_used.size();
    int food_num = rand() % n;
    auto begin = not_used.begin();
    while(food_num--) ++begin;
    QPair<int, int> food = *begin;
    board[food.first][food.second] = BLOCK_COLOR::RED;
    not_used.erase(begin);

    exist_food = true;
    food_pos = food;
    painter.drawRect(food.second * BLOCK_WIDTH,
                     food.first * BLOCK_HEIGHT,
                     BLOCK_WIDTH,
                     BLOCK_HEIGHT);
}

void Widget::moveSnake() {
    QPair<int, int> head = snake.front();
    QPair<int, int> next;
    dir = dir_next;
    switch (dir) {
    case DIRECTION::UP: {
        next = {head.first - 1, head.second};
        break;
    }
    case DIRECTION::RIGHT: {
        next = {head.first, head.second + 1};
        break;
    }
    case DIRECTION::DOWN: {
        next = {head.first + 1, head.second};
        break;
    }
    case DIRECTION::LEFT: {
        next = {head.first, head.second - 1};
        break;
    }
    }
    if(next.first < 0 || next.second < 0 || next.first >= ROW_COUNT || next.second >= COLUMN_COUNT) {
        snake.emplaceFront(next);
        snake.pop_back();
        return;
    }
    if(board[next.first][next.second] == BLOCK_COLOR::YELLO) {
        state = GAME_STATE::GAME_OVER;
        return;
    }
    bool remove_tail = true;
    if(board[next.first][next.second] == BLOCK_COLOR::RED) {
        ++score;
        exist_food = false;
        remove_tail = false;
    }
    board[next.first][next.second] = BLOCK_COLOR::YELLO; snake.emplaceFront(next); not_used.remove(next);
    if(remove_tail) {
        QPair<int, int> tail = snake.back();
        not_used.insert(tail);
        board[tail.first][tail.second] = BLOCK_COLOR::NONE;
        snake.pop_back();
    }
}

void Widget::check() {
    QPair<int, int> head = snake.front();
    if(head.first < 0 || head.second < 0 || head.first >= ROW_COUNT || head.second >= COLUMN_COUNT) {
        //snake.pop_front();
        state = GAME_STATE::GAME_OVER;
        //qDebug() << head.first << ' ' << head.second;
        return;
    } else if(not_used.empty())
        state = GAME_STATE::WIN;
}
