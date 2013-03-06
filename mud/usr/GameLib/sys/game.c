# include <gamelib.h>
# include <kernel/kernel.h>

string name;

void create(varargs int clone) {
  name = "Second Contract";
}

string get_name() { return name; }

void set_name(string s) {
  if(SYSTEM()) name = s;
}
