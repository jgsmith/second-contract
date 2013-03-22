/*
 * Manages data for ending chronic sensations
 */

object env;
int    id;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_object(object o) { env = o; }
void set_id(int i) { id = i; }

void trigger() {
  if(env) env -> remove_sensation(id);
}
