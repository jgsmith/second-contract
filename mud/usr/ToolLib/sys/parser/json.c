# include <type.h>
# include <toollib.h>

inherit parser LIB_PARSER;

static void create(varargs int clone) {
  set_bnf_file(JSON_BNF);
}

/*
 * These functions are called by the parser based on matching patterns in the
 * grammar. The `args` array holds all of the data associated with the
 * right hand side of the matching pattern. The function returns the value
 * that should be used in place of the left hand side symbol.
 *
 * Returning '0' indicates that this parse alternative should fail.
 */

static mixed * _empty_object(mixed *args) { return ({ ([ ]) }); }

static mixed * _object(mixed *args) { return ({ args[1] }); }

static mixed * _add_pair(mixed *args) {
  args[0] += args[2];
  return ({ args[0] });
}

static mixed * _make_pair(mixed *args) {
  args[0] = args[0][1..strlen(args[0])-2];
  args[0] = implode(explode(args[0], "\\n"), "\n");
  args[0] = implode(explode(args[0], "\\\""), "\"");
  return ({ ([ args[0] : args[2] ]) });
}

static mixed *_empty_array(mixed *args) { return ({ ({ }) }); }

static mixed *_array(mixed *args) { return ({ args[1] }); }

static mixed *_add_value(mixed *args) {
  if(sizeof(args) == 1) return ({ ({ args[0] }) });
  args[0] += ({ args[2] });
  return ({ args[0] });
}

/*
 * LPC doesn't have a boolean type, so we translate true and false into
 * 1 and 0.
 */

static mixed * _true(mixed *args) { return ({ 1 }); }

static mixed * _false(mixed *args) { return ({ 0 }); }

static mixed * _null(mixed *args) { return ({ nil }); }

/*
 * LPC has two different numeric types: integer and float.
 */
static mixed * _parse_number(mixed *args) {
  int d;
  float f;
  if(sizeof(explode(args[0], ".")) == 1 &&
     sizeof(explode(args[0], "E")) == 1 &&
     sizeof(explode(args[0], "e")) == 1) {
    if(sscanf(args[0], "%d", d)) return ({ d });
  }
  if(sscanf(args[0], "%f", f)) return ({ f });
  return ({ nil });
}

static mixed * _parse_string(mixed *args) {
  args[0] = args[0][1..strlen(args[0])-2];
  args[0] = implode(explode(args[0], "\\n"), "\n");
  args[0] = implode(explode(args[0], "\\\""), "\"");
  return ({ args[0] });
}

mixed from_json(string json) {
  mixed *result;

  json = implode(explode(json, "\n"), " ");

  result = parser::parse(json);
  if(result) return result[0];
  return nil;
}

string to_json(mixed data) {
  string *keys;
  string *vals;
  string tmp;
  int i, n;

  switch(typeof(data)) {
    case T_NIL: return "null";
      /* LPC automatically converts numeric data to a string when concatenating
       * to a string. We don't have a boolean type, so converting a boolean
       * from JSON to LPC and back to JSON will result in integers in the JSON.
       */
    case T_INT: return ""+data;
    case T_FLOAT: return ""+data;
    case T_STRING:
      tmp = implode(explode(data, "\\"), "\\\\");
      tmp = implode(explode(tmp, "\n"), "\\n");
      tmp = implode(explode(tmp, "\t"), "\\t");
      tmp = implode(explode(tmp, "\""), "\\\"");
      return "\"" + tmp + "\"";
    case T_OBJECT: 
      tmp = data->to_json();
      if(tmp) return tmp;
      return "null";
    case T_ARRAY:
      vals = ({ });
      for(i = 0, n = sizeof(data); i < n; i++)
        vals += ({ to_json(data[i]) });
      return "[" + implode(vals, ",") + "]";
    case T_MAPPING:
      keys = map_indices(data);
      vals = ({ });
      for(i = 0, n = sizeof(keys); i < n; i++)
        vals += ({ to_json(keys[i]) + ":" + to_json(data[keys[i]]) });
      return "{" + implode(vals, ",") + "}";
    default: return "null";
  }
} 
