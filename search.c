#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "queue.h"
#include "hash.h"
#include "domain.h"
#include "search.h"
#include <sys/time.h>
#include <sys/resource.h>

struct {
  int (*algorithm)();
  void (*fringeInsert)(QUEUE *,QUEUE);
  void (*successorsInsert)(QUEUE *,tSearchNode *);
  char *fileName;
  char verbose;
} options;

struct {
  unsigned int expandedNodes;
  struct rusage resources;
} statistics;

tState *initialState;

/*--- Different types of node insertion in fringe  ---*/

void fringeInsertLast(QUEUE *fringe,QUEUE successors) {
  *fringe=qAppend(*fringe,successors);
}

void fringeInsertFirst(QUEUE *fringe,QUEUE successors) {
  *fringe=qAppend(successors,*fringe);
}

/*--- Different types of node insertion in successors  ---*/

void succInsertFirst(QUEUE *successors,tSearchNode *n) {
  qInsertFirst(successors,&n,sizeof(tSearchNode *));
}

/*--- Expanding a node -------------------------------------*/

QUEUE expand(tSearchNode *node) {
  QUEUE successors=qEMPTY;
  unsigned op;
  for (op=0;op<numOperators;op++)
    if (executable(op,node->state)) {
      tSearchNode *new;
      tState *s=successorState(op,node->state);
      
      new=(tSearchNode *)malloc(sizeof(tSearchNode));
      new->state=s;
      new->parent=node;
      new->action=op;
      new->pathCost=node->pathCost + cost(op,node->state);
      new->depth=node->depth+1;
      options.successorsInsert(&successors,new);
    }
  statistics.expandedNodes++;
  return successors;
}

/*--- Printing the solution -------------------------------------*/

void printReversePath(tSearchNode *node) {
  if (node->parent==NULL) return;

  printReversePath(node->parent);
  showOperator(node->action);
  if (options.verbose) {
    printf("\n");
    showState(node->state);
    printf("\n");
  }
  else printf("; ");
}

void printSolution(tSearchNode *node) {
  printf("Solution: \n");
  printReversePath(node);
  printf("\nDepth=%d\n",node->depth);
  printf("Cost=%d\n",node->pathCost);
}

/*--- General search algorithms (tree-based, graph-based) -----------*/

tSearchNode *rootNode() {
  /* Create the root node */
  tSearchNode *root=(tSearchNode *)malloc(sizeof(tSearchNode));
  root->state=initialState;
  root->parent=NULL;
  root->pathCost=0;
  root->depth=0;
  return root;
}

int treeSearch() {
  tSearchNode *node,*n,*root=rootNode();
  QUEUE fringe=qEMPTY,successors;

  qInsertFirst(&fringe,&root,sizeof(tSearchNode *));
  
  while (1) {  
    if (qIsEmpty(fringe)) return 0;
    qPOP(&fringe,&node,sizeof(tSearchNode *));

    if (goalTest(node->state)) {printSolution(node); return 1;}
    
    successors = expand(node);    
    options.fringeInsert(&fringe,successors);
  }
  
}

int graphSearch() {
  tSearchNode *node,*n,*root=rootNode();
  QUEUE fringe=qEMPTY,successors;
  tHashTable *closed=hashCreate(10007,stateHashKeySize);
  tHashEntry e;
 
  qInsertFirst(&fringe,&root,sizeof(tSearchNode *));

  while (1) {
    if (qIsEmpty(fringe)) return 0;
    qPOP(&fringe,&node,sizeof(tSearchNode *));

    if (goalTest(node->state)) {printSolution(node); return 1;}

    e.key=stateHashKey(node->state);
    if (hashFind(closed,&e)) {  /* already visited state */
      freeState(node->state);
      free(node);
      continue;
    }

    hashInsert(closed,stateHashKey(node->state),NULL);
    
    successors = expand(node);
    options.fringeInsert(&fringe,successors);
  }
  hashFree(closed);
}

/*--- Default hash key = whole tState struct ---*/
void *defaultHashKey(tState *s) {
  return s;
}

/*--- Main: checking command line options and printing statistics ---*/

extern char *optarg;
extern int optind;

int main (int argc,char *argv[]) {
  /* Processing options */
  int c;
  
  options.algorithm=treeSearch;
  options.fringeInsert=fringeInsertLast;
  options.successorsInsert=succInsertFirst;
  options.verbose=0;
  options.fileName=NULL;
  statistics.expandedNodes=0;
  
  /* Default settings for hash table */
  stateHashKeySize=sizeof(tState);
  stateHashKey=defaultHashKey;

  while ((c = getopt(argc, argv, "a:i:f:v")) != EOF) {
    switch (c) {
      case 'a':
	  if (!strcmp(optarg,"tree"))
	    options.algorithm=treeSearch;
	  else {
	    if (!strcmp(optarg,"graph"))
	      options.algorithm=graphSearch;
	    else {
	      printf("Unrecognized algorithm %s.\n",optarg); exit(1);
	    }
	  }
	  break;
      case 'i':
	  if (!strcmp(optarg,"breadth"))
	    options.fringeInsert=fringeInsertLast;
	  else if (!strcmp(optarg,"depth"))
	    options.fringeInsert=fringeInsertFirst;
	  else {
	    printf("Unrecognized type of fringe insertion %s.\n",optarg);
	    exit(1);
	  }
	  break;
      case 'f':
        options.fileName=optarg;
        break;
      case 'v': options.verbose=1;break;
      case 'h':
      case '?': 
      printf("search -a <algorith_mname> -i <fringe_insertion> -f <file_name>");
    }
  }
  
  initialState=loadDomain(options.fileName);

  if (!(options.algorithm()))
    printf("No solution.\n");
  printf("%d expanded nodes.\n",statistics.expandedNodes);
  getrusage(RUSAGE_SELF,&statistics.resources);
  printf("%ld.%03ld seconds.\n",
    statistics.resources.ru_utime.tv_sec,
    statistics.resources.ru_utime.tv_usec/1000);
  
  freeDomain();
  
  return 0;
}
