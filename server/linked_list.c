// Linked list operations in C

#include <stdio.h>
#include <stdlib.h>
// #include "aesdsocket.h"
#include "linked_list.h"

// Create a node
// struct Node {
//   int data;
//   struct Node* next;
// };
// Implementation from https://www.programiz.com/dsa/linked-list-operations
// Insert at the beginning
void insertAtBeginning(struct Node** head_ref,  Node* new_data) {
  // Allocate memory to a node
  struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

  // insert the data
  new_node = new_data;

  new_node->next = (*head_ref);

  // Move head to new node
  (*head_ref) = new_node;
}

// Insert a node after a node
void insertAfter(struct Node* prev_node, Node* new_data) {
  if (prev_node == NULL) {
  printf("the given previous node cannot be NULL");
  return;
  }

  struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
  new_node = new_data;
  new_node->next = prev_node->next;
  prev_node->next = new_node;
}

// Insert the the end
void insertAtEnd(struct Node** head_ref, Node* new_data) {
  struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
  struct Node* last = *head_ref; /* used in step 5*/

  new_node = new_data;
  new_node->next = NULL;

  if (*head_ref == NULL) {
  *head_ref = new_node;
  return;
  }

  while (last->next != NULL) last = last->next;

  last->next = new_node;
  return;
}

// Delete a node
void deleteNode(struct Node** head_ref, Node* key) {
  struct Node *temp = *head_ref, *prev;

  if (temp != NULL && temp->data->client_fd == key->data->client_fd) {
  *head_ref = temp->next;
  free(temp);
  return;
  }
  // Find the key to be deleted
  while (temp != NULL && temp->data->client_fd != key->data->client_fd) {
  prev = temp;
  temp = temp->next;
  }

  // If the key is not present
  if (temp == NULL) return;

  // Remove the node
  prev->next = temp->next;

  free(temp);
}

// Search a node
bool searchNode(struct Node** head_ref, Node* key) {
  struct Node* current = *head_ref;

  while (current != NULL) {
  if (current->data->client_fd == key->data->client_fd) return 1;
  current = current->next;
  }
  return 0;
}

// Sort the linked list
void sortLinkedList(struct Node** head_ref) {
  struct Node *current = *head_ref, *index = NULL;
  int temp;

  if (head_ref == NULL) {
  return;
  } else {
  while (current != NULL) {
    // index points to the node next to current
    index = current->next;

    while (index != NULL) {
    if (current->data->client_fd > index->data->client_fd) {
      temp = current->data->client_fd;
      current->data->client_fd = index->data->client_fd;
      index->data->client_fd = temp;
    }
    index = index->next;
    }
    current = current->next;
  }
  }
}

// Print the linked list
void printList(struct Node* node) {
  while (node != NULL) {
    syslog(LOG_INFO," client fd: %d ", node->data->client_fd);
    // printf(" client fd: %d ", node->data->client_fd);
    node = node->next;
  }
}

// Driver program
// int main() {
//   struct Node* head = NULL;

//   insertAtEnd(&head, 1);
//   insertAtBeginning(&head, 2);
//   insertAtBeginning(&head, 3);
//   insertAtEnd(&head, 4);
//   insertAfter(head->next, 5);

//   printf("Linked list: ");
//   printList(head);

//   printf("\nAfter deleting an element: ");
//   deleteNode(&head, 3);
//   printList(head);

//   int item_to_find = 3;
//   if (searchNode(&head, item_to_find)) {
//   printf("\n%d is found", item_to_find);
//   } else {
//   printf("\n%d is not found", item_to_find);
//   }

//   sortLinkedList(&head);
//   printf("\nSorted List: ");
//   printList(head);
// }
