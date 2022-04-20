#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "conio.h"
#include "kbhit.h"

#define FIELD_HEIGHT 8
#define FIELD_WIDTH 8

#define BLOCK_TYPE_MAX 7

enum
{
  CELL_TYPE_NONE,
  CELL_TYPE_BLOCK_0,
  CELL_TYPE_BLOCK_1,
  CELL_TYPE_BLOCK_2,
  CELL_TYPE_BLOCK_3,
  CELL_TYPE_BLOCK_4,
  CELL_TYPE_BLOCK_5,
  CELL_TYPE_BLOCK_6,
  CELL_TYPE_MAX,
};

char cellAA[][8] = {
    "・",     // CELL_TYPE_NONE
    "🟠",   // CELL_TYPE_BLOCK_0,
    "🟤",   // CELL_TYPE_BLOCK_1,
    "🔵",      // CELL_TYPE_BLOCK_2,
    "⭕️", // CELL_TYPE_BLOCK_3,
    "🔴",      // CELL_TYPE_BLOCK_4,
    "🟣",   // CELL_TYPE_BLOCK_5,
    "🟢",   // CELL_TYPE_BLOCK_6,
};

int cells[FIELD_HEIGHT][FIELD_WIDTH];
int checked[FIELD_HEIGHT][FIELD_WIDTH];

int cursorX, cursorY;
int selectedX = -1, selectedY = -1;

bool locked = true;

int getConnectedBlockCount(int _x, int _y, int _cellType, int _count)
{
  if (
      (_x < 0) || (_x >= FIELD_WIDTH) || (_y < 0) || (_y >= FIELD_HEIGHT) || checked[_y][_x] || (cells[_y][_x] == CELL_TYPE_NONE) || (cells[_y][_x] != _cellType))
  {
    return _count;
  }
  else
  {
    _count++;
    checked[_y][_x] = true;

    _count = getConnectedBlockCount(_x, _y - 1, _cellType, _count);
    _count = getConnectedBlockCount(_x - 1, _y, _cellType, _count);
    _count = getConnectedBlockCount(_x, _y + 1, _cellType, _count);
    _count = getConnectedBlockCount(_x + 1, _y, _cellType, _count);
    return _count;
  }
};

void eraseConnectedBlocks(int _x, int _y, int _cellType)
{
  if (
      (_x < 0) || (_x >= FIELD_WIDTH) || (_y < 0) || (_y >= FIELD_HEIGHT) || (cells[_y][_x] == CELL_TYPE_NONE) || (cells[_y][_x] != _cellType))
    return;

  cells[_y][_x] = CELL_TYPE_NONE;
  eraseConnectedBlocks(_x, _y - 1, _cellType);
  eraseConnectedBlocks(_x - 1, _y, _cellType);
  eraseConnectedBlocks(_x, _y + 1, _cellType);
  eraseConnectedBlocks(_x + 1, _y, _cellType);
};
void eraseConnectedBlocksAll()
{
  memset(checked, 0, sizeof checked);
  for (int y = 0; y < FIELD_HEIGHT; y++)
  {
    for (int x = 0; x < FIELD_WIDTH; x++)
    {
      int n = getConnectedBlockCount(x, y, cells[y][x], 0);

      if (n >= 3)
      {
        eraseConnectedBlocks(x, y, cells[y][x]);
        locked = true;
      }
    }
  }
}
void display()
{
  system("clear");
  for (int y = 0; y < FIELD_HEIGHT; y++)
  {
    for (int x = 0; x < FIELD_WIDTH; x++)
    {
      if ((x == cursorX) && (y == cursorY) && (!locked))
        printf("⚫️");
      else
        printf("%s", cellAA[cells[y][x]]);
    }
    if (y == selectedY)
      printf("⬅️");
    printf("\n");
  }

  for (int x = 0; x < FIELD_WIDTH; x++)
  {
    printf("%s", (x == selectedX) ? "⬆️" : "　");
  }
}

int main(void)
{
  srand((unsigned int)time(NULL));
  for (int y = 0; y < FIELD_HEIGHT; y++)
  {
    for (int x = 0; x < FIELD_WIDTH; x++)
    {
      cells[y][x] = CELL_TYPE_BLOCK_0 + rand() % BLOCK_TYPE_MAX;
    }
  }

  time_t t = time(NULL);
  while (1)
  {
    if (t < time(NULL))
    {
      t = time(NULL);

      if (locked)
      {
        locked = false;
        for (int y = FIELD_HEIGHT - 2; y >= 0; y--)
        {
          for (int x = 0; x < FIELD_WIDTH; x++)
          {
            if ((cells[y][x] != CELL_TYPE_NONE) && (cells[y + 1][x] == CELL_TYPE_NONE))
            {
              cells[y + 1][x] = cells[y][x];
              cells[y][x] = CELL_TYPE_NONE;
              locked = true;
            }
          }
        }
        for (int x = 0; x < FIELD_WIDTH; x++)
        {
          if (cells[0][x] == CELL_TYPE_NONE)
          {
            cells[0][x] = CELL_TYPE_BLOCK_0 + rand() % BLOCK_TYPE_MAX;
            locked = true;
          }
        }
        if (!locked)
        {
          eraseConnectedBlocksAll();
        }
      }
      display();
    }

    if (kbhit())
    {
      if (locked)
      {
        getch();
      }
      else
      {

        switch (getch())
        {
        case 'w':
          cursorY--;
          break;
        case 's':
          cursorY++;
          break;
        case 'a':
          cursorX--;
          break;
        case 'd':
          cursorX++;
          break;
        default:
          if (selectedX < 0)
          {
            selectedX = cursorX;
            selectedY = cursorY;
          }
          else
          {
            int distance = abs(selectedX - cursorX) + abs(selectedY - cursorY);
            if (distance == 0)
            {
              selectedX = -1;
              selectedY = -1;
            }
            else if (distance == 1)
            {
              int temp = cells[cursorY][cursorX];
              cells[cursorY][cursorX] = cells[selectedY][selectedX];
              cells[selectedY][selectedX] = temp;

              eraseConnectedBlocksAll();

              selectedX = -1;
              selectedY = -1;
            }
            else
            {
              printf("\a");
            }
          }
          break;
        }
      }
      display();
    }
  }

  return 0;
}
