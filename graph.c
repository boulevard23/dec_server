#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

Node* InitNode() {
  Node* node = (Node *)malloc(sizeof(Node));
  memset(node, 0, sizeof(Node));
  return node;
}

NodeList* InitNodeList() {
  NodeList* nodeList = (NodeList *)malloc(sizeof(NodeList));
  memset(nodeList, 0, sizeof(NodeList));
  return nodeList;
}

void AddNode(NodeList* nodeList, char* c) {
  Node* node = InitNode();
  strcpy(node->event, c);
  /*node->event = c;*/
  if (nodeList->nodeFront == NULL) {
    nodeList->nodeFront = nodeList->nodeRear = node;
  } else {
    nodeList->nodeRear->nodeNext = node;
    nodeList->nodeRear = node;
  }
}

/* add edge to edgeList, two vertexes of the edge is edgeNode1 and edgeNode2*/
void AddEdge(NodeList* nodeList, char* startChar, char* endChar) {
  Edge* edge = (Edge *)malloc(sizeof(Edge));
  Node* edgeNode1 = GetNode(nodeList, startChar);
  Node* edgeNode2 = GetNode(nodeList, endChar);
  if (edgeNode1->edges == NULL) {
    edgeNode1->edges = (Edge *)malloc(sizeof(Edge));
    edgeNode1->edges->connectsTo = edgeNode2;
  } else {
    edge = edgeNode1->edges;
    while (edge->edgeNext != NULL) {
      edge = edge->edgeNext;
    }
    edge->edgeNext = (Edge *)malloc(sizeof(Edge));
    edge->edgeNext->connectsTo = edgeNode2;
  }
}

Node* GetNode(NodeList* nodeList, char* c) {
  Node* node = nodeList->nodeFront;
  for (; node != NULL; node = node->nodeNext) {
    if (strcasecmp(node->event, c) == 0) {
      return node;
    }
  }
  return NULL;
}

int EdgeExists(NodeList* nodeList, char* startChar, char* endChar) {
  Node* edgeStartNode = GetNode(nodeList, startChar);
  Node* edgeEndNode = GetNode(nodeList, endChar);
  Edge* edge = edgeStartNode->edges;
  for(; edge != NULL; edge = edge->edgeNext) {
    if (edge->connectsTo == edgeEndNode) {
      return 1;
    }
  }
  return 0;
}

int SubPathExists(Node* startNode, Node* endNode) {
  if (startNode->visited == 1) {
    return 0;
  } else if (startNode == endNode) {
    return 1;
  } else {
    startNode->visited = 1;
  }

  Edge* edge = startNode->edges;
  for(; edge != NULL; edge = edge->edgeNext) {
    if (SubPathExists(edge->connectsTo, endNode)) {
      return 1;
    }
  }
  return 0;
}

int PathExists(NodeList* nodeList, char* startChar, char* endChar) {
  Node* node = nodeList->nodeFront;
  Node* startNode = GetNode(nodeList, startChar);
  Node* endNode = GetNode(nodeList, endChar);
  for(; node != NULL; node = node->nodeNext) {
    node->visited = 0;
  }
  return SubPathExists(startNode, endNode);
}

int IsConflicted(NodeList* nodeList, char* startChar, char* endChar) {
  if (PathExists(nodeList, endChar, startChar) == 1) {
    return 1;
  }
  return 0;
}

void Traverse(NodeList* nodeList) {
  Node* node = nodeList->nodeFront;
  printf("step1\n");
  Edge* edge;
  while (1) {
    if (node != NULL) {
      printf("%s: ", node->event);
      edge = node->edges;
      while (1) {
        if (edge != NULL) {
          printf("%s, ", edge->connectsTo->event);
          edge = edge->edgeNext;
        } else {
          printf("\n");
          break;
        }
      }
      node = node->nodeNext;
    } else {
      break;
    }
  }
}
