#include "simulated_process.h"

/*
    * Enqueue a process to the end of the queue
    *
    * @param head The head of the queue
    * @param process The process to be enqueued
    */
void enqueue(Node **head, Process *process) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->process = process;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

/*
    * Dequeue a process from the front of the queue
    *
    * @param head The head of the queue
    * @return The process that is dequeued
    */
Process *dequeue(Node **head) {
    if (!head || !*head || !(*head)->process) {
        return NULL;
    }

    Node *temp = *head;
    Process *process = temp->process;
    *head = temp->next;
    free(temp);

    return process;
}

/*
    * Check if the queue is empty
    *
    * @param head The head of the queue
    * @return True if the queue is empty, false otherwise
    */
bool is_empty(Node *head) {
    return head == NULL;
}

/*
    * Get the number of processes in the queue
    *
    * @param head The head of the queue
    * @return The number of processes in the queue
    */
int count_process(Node *head) {
    int count = 0;
    Node *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}