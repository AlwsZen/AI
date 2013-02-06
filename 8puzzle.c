#include <stdlib.h>
#include <stdio.h>
#include "8puzzle.h"
#include "search.h"

tState *loadDomain(char *a) {
  
  tState *initial_state;
  
  numOperators = 4;
  initial_state = (tState *)malloc(sizeof(tState));
  
  initial_state->tablero[0][0] = 4;
  initial_state->tablero[0][1] = 1;
  initial_state->tablero[0][2] = 3;
  initial_state->tablero[1][0] = 7;
  initial_state->tablero[1][1] = 2;
  initial_state->tablero[1][2] = 5;
  initial_state->tablero[2][0] = 0;
  initial_state->tablero[2][1] = 8;
  initial_state->tablero[2][2] = 6;
  
  initial_state->holePos.x = 2;
  initial_state->holePos.y = 0;

  return initial_state;
}

void freeDomain() {}

int goalTest(tState *s) {
  
  int positions[3][3];
  positions[0][0] = 1;
  positions[0][1] = 2;
  positions[0][2] = 3;
  positions[1][0] = 4;
  positions[1][1] = 5;
  positions[1][2] = 6;
  positions[2][0] = 7;
  positions[2][1] = 8;
  positions[2][2] = 0;
  
  struct {
    int x;
    int y;
  } hole;
  
  hole.x = 2;
  hole.y = 2;
  
  return (compareArrays(3, s->tablero, positions) && hole.x == s->holePos.x && hole.y == s->holePos.y);
}

void freeState(tState *s) {
  free(s);
}

int main(){}
