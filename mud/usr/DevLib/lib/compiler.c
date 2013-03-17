# include <devlib.h>
# include <devlib/compiler.h>
# include <type.h>
# include <kernel/kernel.h>

void create(varargs int clone) {
  if(clone) {
  }
}

/* Takes the first item in the array and uses that to build the object */
string compile_parse(object ctx, mixed *args) {
  string *results;
  object ob;
  string handler;
  string *bits;
  int i, n;

  if(!ctx) {
    ctx = new_object(COMPILER_CONTEXT_DATA);
  }

  if(!sizeof(args)) return nil;

  switch(typeof(args[0])) {
    case T_STRING:
      bits = explode(args[0], ":");
      if(sizeof(bits) > 1) {
        return call_other(this_object(), "compile_" + bits[0], ctx, bits[1..] + args[1..]);
      }
      return call_other(this_object(), "compile_" + args[0], ctx, args[1..]);
    case T_ARRAY:
      if(sizeof(args) == 1) return compile_parse(ctx, args[0]);
      results = allocate(n = sizeof(args));
      for(i = 0; i < n; i++) results[i] = compile_parse(ctx, args[i]);
      return implode(results - ({ "", nil }), "\n");
    default :
      return "";
  }
}

string compile_list(object ctx, mixed *args) {
  int i, n;
  string *exps;

  if(sizeof(args) == 1) return compile_parse(ctx, args[0]);
  exps = allocate(n = sizeof(args));
  for(i = 0; i < n; i++) exps[i] = compile_parse(ctx, args[i]);
  return implode(exps - ({ nil }), ", ");
}

string compile_assignment(object ctx, mixed *args) {
  return compile_parse(ctx, args[0]) + " = " + compile_parse(ctx, args[1]);
}

string compile_constant(object ctx, mixed *args) {
  return ""+args[0];
}

string compile_binary(object ctx, mixed *args) {
  int i, n;
  string *operands;

  operands = allocate((n = sizeof(args))-1);
  for(i = 1; i < n; i++) operands[i-1] = compile_parse(ctx, args[i]);
  return implode(operands - ({ "", nil }), " " + args[0] + " ");
}

/*
 * Messages are compiled as calls to an inherited function
 */
string compile_message(object ctx, mixed *args) {
  if(ctx -> void_context()) {
    return "_C_message_async(" + compile_parse(ctx, args[0]) + ")";
  }
  else {
    return "_C_message(" + compile_parse(ctx, args[0]) + ")";
  }
}

string compile_function(object ctx, mixed *args) {
  string *fargs;
  int i, n;
  string real_name;

  real_name = "__" + args[0];
  fargs = allocate(n = sizeof(args)-1);
  for(i = 0; i < n; i++) fargs[i] = compile_parse(ctx, args[i+1]);
  return "this_object() -> " + real_name + "(" + implode(fargs, ", ") + ")";
}

string compile_variable(object ctx, mixed *args) {
  return args[0];
}

string compile_array(object ctx, mixed *args) {
  string *items;
  int i, n;

  items = allocate(n = sizeof(args));
  for(i = 0; i < n; i++) items[i] = compile_parse(ctx, args[i]);
  return "({ " + implode(items, ", ") + " })";
}

string compile_map(object ctx, mixed *args) {
  string *items;
  int i, n;
  items = allocate(n = sizeof(args));
  for(i = 0; i < n; i++) {
    ctx -> reset_void_context();
    items[i] = compile_parse(ctx, args[i][0]) + ":";
    ctx -> reset_void_context();
    items[i] += compile_parse(ctx, args[i][1]);
  }
  return "([ " + implode(items, ", ") + " ])";
}

string compile_catch(object ctx, mixed *args) {
  string *bits;

  bits = ({ compile_parse(ctx, args[0]), "" });
  if(sizeof(args) > 1)
    bits[1] = compile_parse(ctx, args[1]);
  return "catch { " + implode(bits, "}:{") + " }";
}

string compile_catch_expr(object ctx, mixed *args) {
  ctx -> reset_void_context();
  return "catch(" + compile_parse(ctx, args[0]) + ")";
}

string event_default_args() { return ""; }

string compile_event(object ctx, mixed *args) {
  string *ev_args;
  int i, n;
  
  if((n = sizeof(args)) > 2) {
    ev_args = allocate(n-2);
    for(i = 2; i < n; i++) 
      ev_args[i-2] = "\"" + args[i][0] + "\":" + compile_parse(ctx, args[i][1]);
  }
  else {
    ev_args = ({});
  }
  if(ctx -> void_context()) {
    return "_C_event_async(" + compile_parse(ctx, args[0]) + ", \"" + args[1] + "\", " + event_default_args() + " ([" + implode(ev_args, ", ") + "]))";
  } 
  else {
    return "_C_event(" + compile_parse(ctx, args[0]) + ", \"" + args[1] + "\", " + event_default_args() + " ([" + implode(ev_args, ", ") + "]))";
  }
}

string compile_if(object ctx, mixed *args) {
  string ret;
  string tmp;

  ctx -> reset_void_context();
  ret = "if(" + compile_parse(ctx, args[0]) + ")";
  if(tmp = compile_parse(ctx, args[1])) ret += tmp;
  else ret += "{}";
  if(sizeof(args) > 2) {
    ret += " else ";
    if(tmp = compile_parse(ctx, args[2])) ret += tmp;
    else tmp += "{}";
  }
  return ret;
}

string compile_do_while(object ctx, mixed *args) {

  return "do { " + compile_parse(ctx, args[0]) + " } while(" + compile_parse(ctx, args[1]) + ");";
}

string compile_while_do(object ctx, mixed *args) {
  object ob;

  return "while(" + compile_parse(ctx, args[0]) + ") { " + compile_parse(ctx, args[1]) + " }";
}

string compile_loop(object ctx, mixed *args) {
  string ret;

  ret = "for(" + compile_parse(ctx, args[0]) + "; "
               + compile_parse(ctx, args[1]) + "; "
               + compile_parse(ctx, args[2]) + ") ";
  if(sizeof(args) > 3) {
    ret += compile_parse(ctx, args[3]);
  }
  else ret += ";";
}

string compile_switch(object ctx, mixed *args) {
  return "switch(" + compile_parse(ctx, args[0]) + ") {"
       + compile_parse(ctx, args[1]) + "}";
}

string compile_case(object ctx, mixed *args) {
  string ret;

  ret = "case ";
  /* if array, then it's a range test
   * if nil, then it's the default case
   */
  if(typeof(args[0][0]) == T_ARRAY) {
    ret += compile_parse(ctx, args[0][0]) + " .. "
         + compile_parse(ctx, args[0][1]);
  }
  else {
    ret += compile_parse(ctx, args[0]);
  }
  ret += ": " + compile_parse(ctx, args[1]);

  return ret;
}

string compile_break(object ctx, mixed *args) {
  return "break;";
}

string compile_continue(object ctx, mixed *args) {
  return "continue;";
}

string compile_next_state(object ctx, mixed *args) {
  return "return \"" + args[0] + "\";";
}

string compile_unary(object ctx, mixed *args) {
  return "(" + args[0] + compile_parse(ctx, args[1]) + ")";
}

string compile_trinary(object ctx, mixed *args) {

  return "(" + compile_parse(ctx, args[0]) + " ? "
             + compile_parse(ctx, args[1]) + " : "
             + compile_parse(ctx, args[2]) + ")";
}
