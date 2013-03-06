# include <toollib.h>
# include <toollib/template.h>
# include <type.h>

string name;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_variable(string s) { name = s; }

string render(mapping data, object pov) {
  switch(typeof(data[name])) {
    case T_STRING: return data[name];
    default : return "";
  }
}

mixed value(mapping data) { return data[name]; }
