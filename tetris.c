#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
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

int grid[10][24];
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
        if (x + i < 0 || y + j < 0 || x + i >= 10 || y + j >= 24 ||
            grid[x + i][y + j] != 0) {
          return 1;
        }
      }
      tIndex++;
    }
  }
  return 0;
}

int rotateCw(int r) { return (r + 1) % 4; }

int rotateAcw(int r) { return (r - 1 + 4) % 4; }

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
      if (x + i >= 0 && y + j >= 0 && x + i < 10 && y + j < 24 &&
          tetrominoes[tId][r][tIndex] != 0) {
        grid[x + i][y + j] = tetrominoes[tId][r][tIndex];
      }
      tIndex++;
    }
  }
}

int clearLines() {
  int lineIndex = 24 - 1;
  int linesCleared = 0;
  while (lineIndex >= 4) {
    int i = 0;
    for (i = 0; i < 10; i++) {
      if (grid[i][lineIndex] == 0)
        break;
    }
    if (i == 10) {
      linesCleared++;
      for (int m = lineIndex; m > 0; m--) {
        for (int n = 0; n < 10; n++) {
          grid[n][m] = grid[n][m - 1];
        }
      }
    }
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

  int tetromino = 0;
  int rotation = 0;
  int posx = 0;
  int posy = 0;

  long long lastFall = 0;
  long long fallInterval = 500;
  Action action = ACTION_NONE;

  // setting up window
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("anjuna's snake", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 200, 480, 0);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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
        lastFall = currentTime;
        continue;
      } else {
        pasteTetromino(posx, posy, rotation, tetromino);
        isTet = 0;
      }
    }

    action = ACTION_NONE;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          action = ACTION_ROTATE_CW;
          break;

        case SDLK_DOWN:
          action = ACTION_SOFT_DROP;
          break;

        case SDLK_LEFT:
          action = ACTION_MOVE_LEFT;
          break;

        case SDLK_RIGHT:
          action = ACTION_MOVE_RIGHT;
          break;

        case SDLK_q:
          action = ACTION_NONE;
          running = 0;
          break;
        }
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
        rotation = rotateCw(rotation);
      break;
    case ACTION_ROTATE_ACW:
      if (canRotateAcw(posx, posy, rotation, tetromino))
        rotation = rotateAcw(rotation);
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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    int tIndex = 0;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (posx + i >= 0 && posy + j >= 0 && posx + i < 10 && posy + j < 24 &&
            tetrominoes[tetromino][rotation][tIndex] != 0) {
          SDL_Rect rect = {(posx + i) * 20, (posy + j) * 20, 20, 20};
          SDL_RenderFillRect(renderer, &rect);
        }
        tIndex++;
      }
    }

    printf("\n\x1b[25A");
    for (int j = 4; j < 24; j++) {
      for (int i = 0; i < 10; i++) {
        if (grid[i][j] != 0) {
          printf("[]");
        } else {
          printf("**");
        }
      }
      printf("\n");
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 10; i++) {
      for (int j = 4; j < 24; j++) {
        if (grid[i][j] != 0) {
          SDL_Rect rect = {i * 20, j * 20, 20, 20};
          SDL_RenderFillRect(renderer, &rect);
        }
      }
    }
    SDL_RenderPresent(renderer);
  }
  // clearing things up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
