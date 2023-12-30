#pragma once
#include <stdbool.h>
#include "aesdsocket.h"

// Create a node
typedef struct Node {
  thread_data* data;
  struct Node* next;
} Node;

// Insert at the beginning
void insertAtBeginning(struct Node** head_ref, Node* new_data);
// Insert a node after a node
void insertAfter(struct Node* prev_node, Node* new_data);
// Insert at the end
void insertAtEnd(struct Node** head_ref, Node* new_data);
// Delete a node
void deleteNode(struct Node** head_ref, Node* key);
// Search a node
bool searchNode(struct Node** head_ref, Node* key);
// Sort the linked list
void sortLinkedList(struct Node** head_ref);
// Print the linked list
void printList(struct Node* node);

