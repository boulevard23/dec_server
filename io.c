#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "io.h"
/*#include "graph.h"*/
/*#include "server.h"*/

void AddCmd(List* list, char* str) {
  Cmd* cmd = (Cmd *)malloc(sizeof(Cmd));
  memset(cmd, 0, sizeof(Cmd));
  strcpy(cmd->val, str);
  if (list->front == NULL) {
    list->front = cmd;
    list->rear = cmd;
  } else {
    list->rear->next = cmd;
    list->rear = cmd;
  }
}

int AnalyzeInput(List* cmdList[], char* input) {
  /*FILE* file = stdin;*/
  char line[128];
  char* word;
  char* tmp;
  char semicolonWord[12];

  bzero(line, sizeof(line));
  strcpy(line, input);

  /*fgets(line, 128, file);*/
  if (strlen(line) == 0) {
    return -2;
  }
  if (strcmp(line, "\n") == 0) {
    printf("please input the command.\n");
    strcat(error, "please input the command.\n");
    return -3;
  }

  /* The input command don't end with ";" */
  if (line[strlen(line) - 2] != ';') {
    printf("input should end with \";\" \n");
    strcat(error, "response from server: input should end with \";\" \n");
    return -1;
  }

  word = strtok(line, " \t\n");
  int i = 0;
  while (word != NULL) {
    if (cmdList[i] == NULL) {
      cmdList[i] = (List *)malloc(sizeof(List));
      memset(cmdList[i], 0, sizeof(List));
    }
    tmp = strchr(word, ';');
    if (tmp != NULL) {
      strncpy(semicolonWord, word, tmp-word);
      semicolonWord[tmp-word] = '\0';
      AddCmd(cmdList[i], semicolonWord);
      i++;
    } else {
      AddCmd(cmdList[i], word);
    }
    word = strtok(NULL, " \t\n");
  }
  /*fclose(file);*/

  return i;
}

int IsLegalInsertCmd(Cmd* cmd) {
  if (strlen(cmd->val) != 4) {
    return 0;
  }
  if (!isalpha(cmd->val[0]) || !isalpha(cmd->val[3])) {
    return 0;
  }
  if (cmd->val[1] != '-' || cmd->val[2] != '>') {
    return 0;
  }
  return 1;
}

int IsLegalQueryCmd(Cmd* cmd) {
  if (strlen(cmd->val) != 1) {
    return 0;
  }
  if (!isalpha(cmd->val[0])) {
    return 0;
  }
  return 1;
}


void InsertNode(List* list, NodeList* nodeList) {
  /* skip the fisrt command: insert */
  Cmd* cmd = list->front->next;
  char startChar[2];
  char endChar[2];
  bzero(startChar, sizeof(startChar));
  bzero(endChar, sizeof(endChar));

  /* check if there is any illegal input, if exists, terminate the whole command */
  for (; cmd != NULL; cmd = cmd->next) {
    if (!IsLegalInsertCmd(cmd)) {
      printf("insert command input error.\n");
      strcat(error, "response from server: insert command input error.\n");
    }
  }

  /* reset the cmd pointer to the list->front->next; */
  cmd = list->front->next;
  /*printf("print nodelist backup:\n");*/
  /*Traverse(nodeListBackup);*/
  for (; cmd != NULL; cmd = cmd->next) {
    strncpy(startChar, &cmd->val[0], 1);
    strncpy(endChar, &cmd->val[3], 1);

    if ((GetNode(nodeList, startChar) != NULL) && (GetNode(nodeList, endChar) != NULL)) {
      if (IsConflicted(nodeList, startChar, endChar)) {
        printf("CONFLICT DETECTED. INSERT FAILED.\n");
        printf("%s->%s and %s->%s cannot be true at the same time!\n", startChar, endChar, endChar, startChar);
        strcat(error, "response from server: CONFLICT DETECTED. INSERT FAILED.\n");
        strcat(error, startChar);
        strcat(error, "->");
        strcat(error, endChar);
        strcat(error, " and ");
        strcat(error, endChar);
        strcat(error, "->");
        strcat(error, startChar);
        strcat(error, " cannot be true at the same time!\n");
        return;
      }
    } else {
      if (GetNode(nodeList, startChar) == NULL) {
        AddNode(nodeList, startChar);
      }
      if (GetNode(nodeList, endChar) == NULL) {
        AddNode(nodeList, endChar);
      }
      if (!PathExists(nodeList, startChar, endChar)) {
        AddEdge(nodeList, startChar, endChar);
      }
    }
  }
  printf("INSERT DONE\n");
  strcat(response, "response from server: INSERT DONE\n");
}

void QueryNode(List* list, NodeList* nodeList) {
  /* skip the fisrt command: query */
  Cmd* cmd = list->front->next;
  char startChar[2];
  char endChar[2];
  bzero(startChar, sizeof(startChar));
  bzero(endChar, sizeof(endChar));
  for (; cmd != NULL; cmd = cmd->next) {
    if (IsLegalQueryCmd(cmd) == 0) {
      strcat(error, "response from server: query command input error.\n");
    }
  }
  cmd = list->front->next;
  Cmd* cmd2 = cmd->next;
  strncpy(startChar, &cmd->val[0], 1);
  strncpy(endChar, &cmd2->val[0], 1);
  if (GetNode(nodeList, startChar) == NULL) {
    printf("Event not found: %s.\n", startChar);
    strcat(error, "response from server: Event not found: ");
    strcat(error, startChar);
    strcat(error, ".\n");
  }
  if (GetNode(nodeList, endChar) == NULL) {
    printf("Event not found: %s.\n", endChar);
    strcat(error, "response from server: Event not found: ");
    strcat(error, endChar);
    strcat(error, ".\n");
    return;
  }
  if (PathExists(nodeList, startChar, endChar)) {
    printf("%s happened before %s.\n", startChar, endChar);
    strcat(response, "response from server: ");
    strcat(response, startChar);
    strcat(response, " happened before ");
    strcat(response, endChar);
    strcat(response, ".\n");
  } else if (PathExists(nodeList, endChar, startChar)) {
    printf("response from server: %s happened before %s.\n", endChar, startChar);
    strcat(response, "response from server: ");
    strcat(response, endChar);
    strcat(response, " happened before ");
    strcat(response, startChar);
    strcat(response, ".\n");
  } else {
    printf("response from server: %s concurrent to %s.\n", startChar, endChar);
    strcat(response, "response from server: ");
    strcat(response, startChar);
    strcat(response, " concurrent to ");
    strcat(response, endChar);
    strcat(response, ".\n");
  }
}

void ResetNodeList(NodeList* nodeList) {
  memset(nodeList, 0, sizeof(nodeList));
  printf("response from server: RESET DONE.\n");
  strcat(response, "RESET DONE.\n");
}

void ExecuteCmd(List* list, NodeList* nodeList, char* clientName) {
  printf("request received from %s: ", clientName);
  Cmd* cmd = list->front;
  for(; cmd != NULL; cmd = cmd->next) {
    printf("%s ", cmd->val);
  }
  printf("\n");

  cmd = list->front;
  if (strcmp(cmd->val, "insert") == 0) {
    InsertNode(list, nodeList);
  } else if (strcmp(cmd->val, "query") == 0) {
    QueryNode(list, nodeList);
  } else if (strcmp(cmd->val, "reset") == 0) {
    ResetNodeList(nodeList);
  } else {
    printf("input error!\n");
    strcat(error, "input error!\n");
  }
}
