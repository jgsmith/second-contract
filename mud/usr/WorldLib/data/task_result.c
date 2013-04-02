/*
 * Data associated with a task attempt
 */

int result;
int degree;
int raw;
int winner; /* for when we compare two participants */
int timing; /* how long do we need to delay? */

static void create(varargs int clone) {
}

void set_result(int r) { result = r; }
int get_result() { return result; }

void set_degree(int d) { degree = d; }
int get_degree() { return degree; }

void set_raw(int r) { raw = r; }
int get_raw() { return raw; }

void set_winner(int w) { winner = w; }
int get_winner() { return winner; }

void set_timing(int t) { timing = t; }
int get_timing() { return timing; }

mixed get_property(string *path) {
  if(sizeof(path) != 1) return nil;

  switch(path[0]) {
    case "successful": return result;
    case "degree": 
      switch(degree) {
        case TASK_MARGINAL: return "marginal";
        case TASK_CRITICAL: return "critical";
        case TASK_EXCEPTIONAL: return "exceptional";
        default : return "normal";
      }
    case "raw": return raw;
    case "winner": return winner < 0 ? "actor" : (winner > 0 ? "target" : "draw");
    case "timing": return timing;
  }
  return nil;
}

/* We're read-only in event scripts */
int set_property(string *path, mixed value) {
  return FALSE;
}
