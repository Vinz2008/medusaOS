typedef struct Pair {
  void* first;
  void* second;
} pair_t;

pair_t create_pair(void* first, void* second);
pair_t* create_pair_a(void* first, void* second);