#include <stdlib.h>
#include "blocks.h"
#include "search.h"

tState *localDomain(char *fileName) {
  tState *initial_state;
  
  initial_state = (tState *)malloc(sizeof(tState));
  initial_state->snapshot[0] = 2;
  initial_state->snapshot[1] = 3;
  initial_state->snapshot[2] = 4;
  initial_state->snapshot[3] = 5;
  initial_state->snapshot[4] = TABLE;
  initial_state->snapshot[5] = TABLE;
  
  numOperators=NUM_BLOCKS*NUM_BLOCKS;
  
  return initial_state;
};


void freeDomain() { }

int goalTest(tState *s) {
  boolean isGoal = false;
  
  return (s->snapshot[0] == 1 &&
	  s->snapshot[1] == 2 &&
	  s->snapshot[2] == 3 &&
	  s->snapshot[3] == 4 &&
	  s->snapshot[4] == 5 &&
	  s->snapshot[5] == TABLE &&);
}

void freeState(tState *s) {
  free(s);
}

int executable(unsigned op,tState *s) {
  return road[s->inCity][op]!=NO_ROAD;
}