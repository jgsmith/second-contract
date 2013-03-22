/*
 * data for an event
 */

object target;
string event_type;
mapping args;

void create(varargs int clone) {
  if(clone) {
    args = ([ ]);
  }
}

string get_event_type() { return event_type; }
void set_event_type(string s) {
  if(s) event_type = s;
}

object get_object() { return target; }
void set_object(object ob) {
  if(ob) target = ob;
}

mapping get_args() { return args ? args + ([ ]) : ([ ]); }
void set_args(mapping a) {
  if(a) args += a;
}
