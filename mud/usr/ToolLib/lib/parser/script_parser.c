# include <toollib.h>
# include <toollib/event_script.h>
# include <type.h>

inherit parser LIB_PARSER;

/*
 * This will parse the script and return a mapping of event handlers to
 * parse trees.
 */

void create(varargs int clone) {
}

mixed parse_script(string script) {
  string *bits;
  int i, n, j, m;

  bits = explode(script, "\n");
  for(i = 0, n = sizeof(bits); i < n; i++) {
    m = strlen(bits[i]);
    j = 0;
    while(j < m && bits[i][j] == ' ') j++;
    if(j < m && bits[i][j] == '#') bits[i] = nil;
  }

  return parser::parse(implode(bits - ({ nil }), "\n"));
}

/* The following are the various handlers called during parsing */

mixed _assoc_exp(mixed *args) {
  return ({ ({ args[0], args[2] }) });
}

mixed _event_arg(mixed *args) {
  return ({ ({ args[0], args[2] }) });
}

mixed _pass_first(mixed *args) {
  return args[0..0];
}

mixed _pass_second(mixed *args) {
  if(sizeof(args) > 1) return args[1..1];
  return ({ });
}

mixed _compound_stmt(mixed *args) {
  if(sizeof(args) == 3) return args[1..1];
  return ({ });
}

mixed _list(mixed *args) {
  if(!sizeof(args)) return ({ });
  if(sizeof(args) >= 3) 
    return ({ args[0] + args[2..] });
  return ({ args[0..0] });
}

/*
 * ({ "list", ... })
 */
mixed _list_exp(mixed *args) {
  if(!sizeof(args)) return ({ });
  if(sizeof(args) >= 3) 
    return ({ args[0] + args[2..] });
  return ({ ({ "list", args[0] }) });
}

mixed _stmt_list(mixed *args) {
  if(!sizeof(args)) return ({ });
  if(sizeof(args) >= 2)
    return ({ args[0] + args[1..] });
  return ({ args[0..0] });
}

mixed _char_const(mixed *args) {
  if(args[0][1] == '\\') {
    switch(args[0][2]) {
      case 'b': return ({ '\b' });
      case 'n': return ({ '\n' });
      case 't': return ({ '\t' });
      case '\\': return ({ '\\' });
      case '\'': return ({ '\'' });
      default: return ({ args[0][2] });
    }
  }
  return ({ args[0][1] });
}

mixed _int_const(mixed *args) {
  int t;
  sscanf(args[0], "%d", t);
  return ({ t });
}

mixed _nil_const(mixed *args) {
  return ({ "constant", nil });
}

mixed _true_const(mixed *args) {
  return ({ "constant", 1 });
}

mixed _false_const(mixed *args) {
  return ({ "constant", 0 });
}

mixed _string_const(mixed *args) {
  string str, front;

  if(sizeof(args) == 1) {
    front = "";
    str = args[0];
  }
  else {
    front = args[0];
    str = args[1];
  }

  str = str[1..strlen(str)-2];

  str = implode(explode(str, "\\n"), "\n");
  str = implode(explode(str, "\\t"), "\t");
  str = implode(explode(str, "\\b"), "\b");
  str = implode(explode(str, "\\\""), "\"");
  str = implode(explode(str, "\\\\"), "\\");

  return ({ front + str });
}

mixed _constant(mixed *args) {
  return ({ ({ "constant", args[0] }) });
}

mixed _assignment(mixed *args) {
  return ({ ({ "assignment", args[0], args[2] }) });
}

mixed _message_output(mixed *args) {
  return ({ ({ "message" }) + args[1..] });
}

mixed _function_call(mixed *args) {
  return ({ ({ "function", args[0], args[2] }) });
}

mixed _var_ref(mixed *args) {
  return ({ ({ "variable", args[0] }) });
}

mixed _array(mixed *args) {
  return ({ ({ "array" }) + args[2] });
}

mixed _map(mixed *args) {
  return ({ ({ "map" }) + args[2] });
}

mixed _noop(mixed *args) {
  return ({ });
}

mixed _catch(mixed *args) {
  return ({ ({ "catch" }) + args[1..] });
}

mixed _catch_expr(mixed *args) {
  return ({ ({ "catch_expr", args[2] }) });
}

mixed _array_decl(mixed *args) {
  return ({ ({ args[0], "*"+args[2] }) });
}

mixed _decl(mixed *args) {
  return ({ ({ args[0], args[1] }) });
}

mixed _array_decl_with_assignment(mixed *args) {
  return ({ ({ args[0], "*"+args[2], args[4] }) });
}

mixed _decl_with_assignment(mixed *args) {
  return ({ ({ args[0], args[1], args[3] }) });
}

mixed _event(mixed *args) {
  return ({ ({ "event", args[1], args[3] }) });
}

mixed _event_with_args(mixed *args) {
  if(sizeof(args) > 6)
    return ({ ({ "event", args[1], args[3] }) + args[5] });
  else
    return ({ ({ "event", args[1], args[3] }) });
}

mixed _if(mixed *args) {
  if(sizeof(args) > 4)
    return ({ ({ "if", args[2], args[4] }) });
  else
    return ({ ({ "if", args[2], ({ }) }) });
}

mixed _if_else(mixed *args) {
  return ({ args[0] + ({ args[2] }) });
}

mixed _do_while(mixed *args) {
  return ({ ({ "do_while", args[4], args[1] }) });
}

mixed _while_do(mixed *args) {
  return ({ ({ "while_do", args[2], args[4] }) });
}

mixed _for(mixed *args) {
  if(sizeof(args) >= 9)
    return ({ ({ "loop", args[2], args[4], args[6], args[8] }) });
  else
    return ({ ({ "loop", args[2], args[4], args[6] }) });
}

mixed _switch(mixed *args) {
  return ({ ({ "switch", args[2], args[4] }) });
}

mixed _case(mixed *args) {
  return ({ ({ "case", args[1], args[3] }) });
}

mixed _case_range(mixed *args) {
  return ({ ({ "case", ({ args[1], args[3] }), args[5] }) });
}

mixed _default(mixed *args) {
  return ({ ({ "case", nil, args[2] }) });
}

mixed _simple_command(mixed *args) {
  return ({ args[0..0] });
}

mixed _goto(mixed *args) {
  return ({ ({ "next_state", args[1] }) });
}

mixed _unaryop(mixed *args) {
  return ({ ({ "unary:" + args[0], args[1] }) });
}

mixed _binop(mixed *args) {
  return ({ ({ "binary:" + args[1], args[0], args[2] }) });
}

mixed _trinary(mixed *args) {
  return ({ ({ "trinary", args[0], args[2], args[4] }) });
}
