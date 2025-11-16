#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
  ACTION_NONE,
  ACTION_MOVE_LEFT,
  ACTION_MOVE_RIGHT,
  ACTION_ROTATE_CW,
  ACTION_ROTATE_ACW,
  ACTION_SOFT_DROP,
  ACTION_HARD_DROP,
} Action;

int grid[24][10];
int tetrominoes[7][4][16] = {
    // ======== T PIECE ========
    {
        {0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    },
    // ======== O PIECE ========
    {
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    // ======== I PIECE ========
    {
        {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    },
    // ======== S PIECE ========
    {
        {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    },
    // ======== Z PIECE ========
    {
        {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    },
    // ======== J PIECE ========
    {
        {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    },
    // ======== L PIECE ========
    {
        {0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    }};

int isCollide(int x, int y, int r, int tId) {
  int tIndex = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (tetrominoes[tId][r][tIndex] != 0) {
        if (x < 0 || y < 0 || grid[x][y] != 0) {
          return 1;
        }
      }
      tIndex++;
    }
  }
  return 0;
}

int rotateCw(int r) { return (r + 1) % 4; }

int rotateAcw(int r) { return (r - 1) % 4; }

int canRotateCw(int x, int y, int r, int tId) {
  return !isCollide(x, y, rotateCw(r), tId);
}

int canRotateAcw(int x, int y, int r, int tId) {
  return !isCollide(x, y, rotateAcw(r), tId);
}

int canMoveRight(int x, int y, int r, int tId) {
  return !isCollide(x + 1, y, r, tId);
}

int canMoveLeft(int x, int y, int r, int tId) {
  return !isCollide(x - 1, y, r, tId);
}

int canMoveDown(int x, int y, int r, int tId) {
  return !isCollide(x, y + 1, r, tId);
}

void pasteTetromino(int x, int y, int r, int tId) {
  int tIndex = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i > 0 && j > 3 && i < 20 && j < 24) {
        grid[i][j] = tetrominoes[tId][r][tIndex];
        tIndex++;
      }
    }
  }
}

int clearLines() {
  int lineIndex = 24 - 1;
  int linesCleared = 0;
  while (lineIndex >= 4) {
    int i;
    for (i = 0; i < 10; i++) {
      if (grid[lineIndex][i] == 0)
        break;
    }
    if (i == 10)
      linesCleared++;
    lineIndex--;
  }
  return linesCleared;
}

int getNewTetromino() { return rand() % 7; }

long long now_ms() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int main() {
  int running = 1;
  int timeGap = 100;
  int isTet = 0;

  int tetromino;
  int rotation;
  int posx = 0;
  int posy = 0;

  long long lastFall;
  long long fallInterval = 500;
  Action action;

  while (running) {
    if (!isTet) {
      tetromino = getNewTetromino();
      isTet = 1;
      posx = posy = 0;
      if (isCollide(posx, posy, rotation, tetromino)) {
        running = 0;
        continue;
      }
    }

    long long currentTime = now_ms();
    if (currentTime - lastFall >= fallInterval) {
      if (canMoveDown(posx, posy, rotation, tetromino)) {
        posy++;
        continue;
      } else {
        pasteTetromino(posx, posy, rotation, tetromino);
      }
    }

    switch (action) {
    case ACTION_NONE:
      break;
    case ACTION_MOVE_LEFT:
      if (canMoveLeft(posx, posy, rotation, tetromino))
        posx--;
      break;
    case ACTION_MOVE_RIGHT:
      if (canMoveRight(posx, posy, rotation, tetromino))
        posx++;
      break;
    case ACTION_ROTATE_CW:
      if (canRotateCw(posx, posy, rotation, tetromino))
        rotateCw(rotation);
      break;
    case ACTION_ROTATE_ACW:
      if (canRotateAcw(posx, posy, rotation, tetromino))
        rotateAcw(rotation);
      break;
    case ACTION_SOFT_DROP:
      if (canMoveDown(posx, posy, rotation, tetromino))
        posy++;
      break;
    case ACTION_HARD_DROP:
      while (canMoveDown(posx, posy, rotation, tetromino))
        posy++;
      pasteTetromino(posx, posy, rotation, tetromino);
      break;
    }

    clearLines();
  }
}
