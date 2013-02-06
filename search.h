typedef struct tSearchNode_ {
  tState *state;
  struct tSearchNode_ *parent;
  unsigned action;
  int pathCost;
  int depth;
} tSearchNode;


tState *loadDomain(char *);
/* Variables to be fixed in loadDomain */
int numOperators;

void freeDomain();
int goalTest(tState *s);
int executable(unsigned op,tState *s);
tState *successorState(unsigned op,tState *s);
void freeState(tState *s);
int cost(unsigned op,tState *s);
void showOperator(unsigned op);
void showState(tState *s);

/* For handling the transposition table */
/* By default 
 *   stateHashKeySize=sizeof(tState);
 * and stateHashKey returns s. However, if information in the
 * state is referred by a pointer, these two values must be updated.
 */
unsigned int stateHashKeySize;
void *(*stateHashKey)(tState *s);