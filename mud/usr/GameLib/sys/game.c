# include <gamelib.h>
# include <kernel/kernel.h>

string name;

static void create(varargs int clone) {
  name = "Second Contract Dev";
}

string get_name() { return name; }

void set_name(string s) {
  if(SYSTEM()) name = s;
}
