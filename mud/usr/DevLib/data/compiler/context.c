/*
 * provides some context for things like indentation and temporary variables
 */

mapping temps;
string indent;
int context_type;

void create(varargs int clone) {
  if(clone) {
    temps = ([ ]);
    indent = "";
    context_type = 0;
  }
}

int void_context() { return context_type == 0; }

void set_void_context() { context_type = 0; }
void reset_void_context() { context_type = 1; }

void increase_indent() { indent += "  "; }
void decrease_indent() {
  if(strlen(indent) > 0) indent = indent[2..];
}

string indent() { return indent; }

string get_tmp_var(string type) {
  string name;
  int i, n, j;

  if(type[strlen(type)-1] == '*') {
    type[strlen(type)-1] = '_';
  }
  if(!temps[type]) { temps[type] = ({ });}
  name = "_" + type;

  for(i = 0, n = sizeof(temps[type]); i < n; i++) {
    if(temps[type] == 0) {
      temps[type] = 1;
      return name+i;
    }
  }
  temps[type] += ({ 1 });
  return name + (sizeof(temps[type])-1);
}

void release_tmp_var(string var) {
  int i;
  string type;
  if(sscanf(var, "_%s%d", type, i) == 2) {
    if(sizeof(temps[type]) > i) temps[type][i] = 0;
  }
}

string emit(object ctx) {
  string *types;
  int i, j, m, n;
  string ret;

  ret = "";
  types = map_indices(temps);
  for(i = 0, n = sizeof(types); i < n; i++) {
    ret += indent;
    if(types[i][strlen(types[i])-1] == '_') {
      ret += types[i][0..strlen(types[i])-2] + " ";
      for(j = 0, m = sizeof(temps[types[i]]); j < m; j ++) {
        ret += "*_"+types[i]+j;
        if(j < m-1) ret += ", ";
      }
    }
    else {
     ret += types[i] + " ";
     for(j = 0, m = sizeof(temps[types[i]]); j < m; j ++) {
       ret += "_"+types[i]+j;
       if(j < m-1) ret += ", ";
     }
    }
    ret += ";\n";
  }
  return ret;
}
