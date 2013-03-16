# include <type.h>
# include <toollib.h>

inherit parser LIB_PARSER;

static void create(varargs int clone) {
  set_bnf_file(MUDMODE_BNF);
}

atomic mixed from_mudmode(string mudmode) {
  mixed *result;

  result = parser::parse(mudmode);
  if(result && sizeof(result)) return result[0];
  return nil;
}

atomic string to_mudmode(mixed data) {
  string *keys;
  string *vals;
  string pref, post, result;
  int i, n;

  switch(typeof(data)) {
    case T_INT:
    case T_FLOAT: return ""+data;
    case T_STRING:
      if(strlen(data) == 0) return "";

      if(data[0..0] == "\\") {
        pref = "\\\\";
      }
      else if(data[0..0] == "\"") {
        pref = "\\\"";
      }
      else {
        pref = "";
      }

      if(data[strlen(data)-1] == '\\') {
        post = "\\\\";
      }
      else if(data[strlen(data)-1] == '"') {
        post = "\\\"";
      }
      else {
        post = "";
      }

      result = implode(explode(data, "\\"), "\\\\");
      result = implode(explode(result, "\""), "\\\"");
      return "\"" + pref + result + post + "\"";
      
    case T_OBJECT: return object_name(data);
    case T_ARRAY:
      vals = ({ });
      for(i = 0, n = sizeof(data); i < n; i++)
        vals += ({ to_mudmode(data[i]) });
      return "({" + implode(vals, ",") + "})";
    case T_MAPPING:
      keys = map_indices(data);
      vals = ({ });
      for(i = 0, n = sizeof(keys); i < n; i++)
        vals += ({ to_mudmode(keys[i]) + ":" + to_mudmode(data[keys[i]]) });
      return "([" + implode(vals, ",") + "])";
    default: return "";
  }
}

mixed _convert_int(mixed *args) {
  int r;
  sscanf(args[0], "%d", r);
  return ({ r });
}

mixed _convert_float(mixed *args) {
  float r;

  sscanf(args[0], "%f", r);
  return ({ r });
}

mixed _empty_string(mixed *args) {
  return ({ "" });
}

mixed _concat_string(mixed *args) {
  int i, n;
  string r;

  r = "";
  for(i = 0, n = sizeof(args); i < n; i++) {
    if(args[i] == "\\") i++;
    r += args[i];
  }

  return ({ r[1..strlen(r)-2] });
}

mixed _empty_array(mixed *args) {
  return ({ ({ }) });
}

mixed _array(mixed *args) {
  int i, n;
  mixed *stuff;

  stuff = ({ });
  for(i = 1, n = sizeof(args); i < n && args[i] != "})"; i+=2)
    stuff += args[i..i];
  return ({ stuff });
}

mixed _empty_map(mixed *args) {
  return ({ ([ ]) });
}

mixed _map(mixed *args) {
  int i, n;
  mapping stuff;

  stuff = ([ ]);
  for(i = 1, n = sizeof(args); i < n && args[i] != "])"; i += 4)
    stuff[args[i]] = args[i+2];

  return ({ stuff });
}
