typedef struct linkedList {
    void* data;
    struct linkedList* next;
} linked_list_t;

linked_list_t* linked_list_init();
void linked_list_insert_beginning(void* data, linked_list_t** node);
void append_linked_list(void* data, linked_list_t* node);