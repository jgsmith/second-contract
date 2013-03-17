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
