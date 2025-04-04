#include "queue.h"
#include "tile_game.h"

void *calloc(unsigned long nmemb, unsigned long size);
void free(void *ptr);

void enqueue(struct queue *q, struct game_state state) {
    insert_at_tail(&q->data, serialize(state));
}

struct game_state dequeue(struct queue *q) {
    return deserialize(remove_from_head(&q->data));
    }

int number_of_moves(struct game_state start) { 
    struct queue q = {0};
    enqueue(&q, start);
  
    const size_t SIZE = 1 << 24;
    bool *visited = calloc(SIZE, sizeof(bool));
    if (!visited) return -1;
  
    while (q.data.head) {
      struct game_state state = dequeue(&q);
      uint64_t s = serialize(state);
      size_t index = (s ^ (s >> 32)) % SIZE;
  
      if (visited[index]) continue;
      visited[index] = true;
  
      // debug counter
      static int step_counter = 0;
      step_counter++;
      if (step_counter % 100000 == 0) {
          fprintf(stderr, "Explored %d states so far...\n", step_counter);
      }
  
      // Check if solved
      bool solved = true;
      uint8_t expected = 1;
      for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
          if (i == 3 && j == 3) {
            if (state.tiles[i][j] != 0) solved = false;
          } else if (state.tiles[i][j] != expected++) {
            solved = false;
          }
        }
      }
      if (solved) {
        free(visited);
        free_list(q.data);
        return state.num_steps;
      }
  
    // Try all moves
    struct game_state next;
    if (state.empty_row < 3) {
        next = state;
        move_up(&next);
        uint64_t next_s = serialize(next);
        size_t next_index = (next_s ^ (next_s >> 32)) % SIZE;
        if (!visited[next_index]) {
            enqueue(&q, next);
        }
      }
      if (state.empty_row > 0) {
        next = state;
        move_down(&next);
        uint64_t next_s = serialize(next);
        size_t next_index = (next_s ^ (next_s >> 32)) % SIZE;
        if (!visited[next_index]) {
            enqueue(&q, next);
        }
      }
      if (state.empty_col < 3) {
        next = state;
        move_left(&next);
        uint64_t next_s = serialize(next);
        size_t next_index = (next_s ^ (next_s >> 32)) % SIZE;
        if (!visited[next_index]) {
            enqueue(&q, next);
        }
      }
      if (state.empty_col > 0) {
        next = state;
        move_right(&next);
        uint64_t next_s = serialize(next);
        size_t next_index = (next_s ^ (next_s >> 32)) % SIZE;
        if (!visited[next_index]) {
            enqueue(&q, next);
        }
      }
    }
  
    free(visited);
    free_list(q.data);
    return -1;
   }
  
