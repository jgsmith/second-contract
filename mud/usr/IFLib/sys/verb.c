# include <iflib.h>
# include <iflib/verb.h>
# include <worldlib/terrain.h>
# include <toollib.h>
# include <type.h>

mapping verb_data;
mapping verb_handlers;

static void create() {
  verb_data = ([ ]);
  verb_handlers = ([ ]);
}

int query_verb(string v) {
  if(verb_handlers[v]) return TRUE;
  return FALSE;
}

string *get_verb_ids() {
  return map_indices(verb_data);
}

string *get_verbs() { return map_indices(verb_handlers); }

static update_grammar() {
  GRAMMAR_P -> update_words();
}

/* returns the identifier of the verb data resource */
string add_verb(object VERB_DATA v, varargs string id) {
  string *names;
  int i, n;

  names = v -> get_verbs();
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(verb_handlers[names[i]]) verb_handlers[names[i]] |= ({ v });
    else verb_handlers[names[i]] = ({ v });
  }

  call_out("update_grammar", 0);

  if(!id) {
    do {
      id = "";
      for(i = 0; i < 5; i++) {
        n = random(20);
        id += "bcdfghjklmnpqrstvwxz"[n..n];
        n = random(6);
        id += "aeiouy"[n..n];
      }
    } while(verb_data[id]);
  }
  verb_data[id] = v;
  return id;
}

object get_verb(string id) {
  return verb_data[id];
}

void remove_verb(string id) {
  string *names;
  object v;
  int i, n;

  v = verb_data[id];

  names = v -> get_verbs();
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(verb_handlers[names[i]]) {
      verb_handlers[names[i]] -= ({ v });
      if(sizeof(verb_handlers[names[i]]) == 0) verb_handlers[names[i]] = nil;
    }
  }

  verb_data[id] = nil;
}

object *get_verb_handlers(string verb, int args) {
  object *list;
  int i, n;

  if(verb_handlers[verb]) list = verb_handlers[verb] + ({ });
  else list = ({ });

  for(i = 0, n = sizeof(list); i < n; i++) {
    if(list[i]->is_disabled()) list[i] = nil;
    if(!list[i]->uses_args(args)) list[i] = nil;
  }
  return list - ({ nil });
}

string get_verb_help(string verb) {
  object *list;
  string *ret;
  int i, n;

  if(verb_handlers[verb]) list = verb_handlers[verb];
  else list = ({ });

  ret = ({ });
  for(i = 0, n = sizeof(list); i < n; i++) {
    ret += ({ list[i]->get_brief(), "", list[i] -> get_help() }) - ({ nil });
    if(list[i]->get_see_also() && sizeof(list[i]->get_see_also())) {
      ret += ({ "See also: " + implode(list[i]->get_see_also(), ", ") });
    }
    ret += ({ "" });
  }

  return implode(ret - ({ nil }), "\n");
}

void replace_verb(string id, object VERB_DATA v) {
  remove_verb(id);
  add_verb(v, id);
}

