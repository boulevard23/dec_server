#include "graph.h"

extern char error[128];
extern char response[128];

typedef struct listStruct {
  struct cmdStruct* front;
  struct cmdStruct* rear;
} List;

typedef struct cmdStruct {
  char val[12];
  struct cmdStruct* next;
} Cmd;

void AddCmd(List *, char *);

int AnalyzeInput(List* [], char *);

int IsLegalInsertCmd(Cmd *);

int IsLegalQueryCmd(Cmd *);

void InsertNode(List *, NodeList *);

void QueryNode(List *, NodeList *);

void ResetNodeList(NodeList *);

void ExecuteCmde(List *, NodeList *, char *);
