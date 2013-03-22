# include <type.h>

/*
 * holds the priority and the particular item
 */

int priority;
mixed data;

void create(varargs int clone) {
  if(clone) {
  }
}

int get_priority() { return priority; }
mixed get_data() { return data; }

void set_data(mixed d) { data = d; }
void set_priority(int d) {
  if(d == priority) return;
  if(d < priority) {
    priority = d;
  }
  else {
    priority = d;
  }
}
