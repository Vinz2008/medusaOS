#include <kernel/stack.h>
#include <stdbool.h>
#include <kernel/kmalloc.h>

stack_t* create_stack_node(void* data){
    stack_t* stack_node = kmalloc(sizeof(stack_t));
    stack_node->data = data;
    stack_node->next = NULL;
    return stack_node;
}

bool stack_is_empty(stack_t* root){
    return !root;
}

void stack_push(stack_t** root, void* data){
    stack_t* stack_node = create_stack_node(data);
    stack_node->next = *root;
    *root = stack_node;
}

void* stack_pop(stack_t** root){
    if (stack_is_empty(*root)){
        return NULL;
    }
    stack_t* temp = *root;
    *root = (*root)->next;
    void* popped_data = temp->data;
    kfree(temp);
    return popped_data;
}

void* stack_peek(stack_t* root){
    if (!stack_is_empty(root)){
        return NULL;
    }
    return root->data;
}