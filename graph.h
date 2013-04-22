typedef struct vertexStruct {
  char event[12];
  struct edgeStruct* edges;
  struct vertexStruct* nodeNext;
  int visited;
} Node;

typedef struct edgeStruct {
  struct vertexStruct* connectsTo;
  struct edgeStruct* edgeNext;
} Edge;

typedef struct nodeListStruct {
  struct vertexStruct* nodeFront;
  struct vertexStruct* nodeRear;
} NodeList;

typedef struct edgeListStruct {
  struct edgeStruct* edgeFront;
  struct edgeStruct* edgeRear;
} EdgeList;

Node* InitNode();

NodeList* InitNodeList();

void AddNode(NodeList*, char*);

void AddEdge(NodeList*, char*, char*);

Node* GetNode(NodeList*, char*);

int EdgeExists(NodeList* nodeList, char*, char*);

int SubPathExists(Node*, Node*);

int PathExists(NodeList*, char*, char*);

int IsConflicted(NodeList*, char*, char*);

void Traverse(NodeList*);
