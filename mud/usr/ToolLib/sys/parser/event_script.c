# include <toollib.h>
# include <toollib/event_script.h>
# include <type.h>

inherit parser LIB_SCRIPT_PARSER;

void create(varargs int clone) {
  parser::create(clone);
  set_bnf_file("event_script.bnf");
}

mixed _return(mixed *args) {
  return ({ ({ "return", args[1] }) });
}

mixed _msg_intensity(mixed *args) {
  int t;
  sscanf(args[1], "%d", t);
  if(args[0] == "+") return ({  t });
  else               return ({ -t });
}

mixed _message_output(mixed *args) {
  int t;
  if(sizeof(args) == 1) return ({ ({ "env", 0, args[0] }) });
  if(sizeof(args) == 3) return ({ ({ args[2], 0, args[0] }) });
  if(sizeof(args) == 4) return ({ ({ args[2], args[3], args[0] }) });
}

mixed _acute_sensation(mixed *args) {
  if(args[0] == ":")
    return ({ ({ "acute_sensation", "light", args[1] }) });
  else
    return ({ ({ "acute_sensation", args[0], args[2] }) });
}

mixed _chronic_sensation(mixed *args) {
  if(args[0] == ":")
    return ({ ({ "chronic_sensation", "light", args[2], args[5] }) });
  else
    return ({ ({ "chronic_sensation", args[0], args[2], args[5] }) });
}

mixed _acute_sensation_set(mixed *args) {
  return ({ ({ "sensation_set", args[2] }) });
}

mixed _chronic_sensation_set(mixed *args) {
  return ({ ({ "sensation_set", args[2], args[6] }) });
}

mixed _sensation_step(mixed *args) {
  return ({ ({ args[0], args[2] }) });
}

mixed _sensation_chain(mixed *args) {
  return ({ ({ "sensation_chain", args[0] }) });
}
