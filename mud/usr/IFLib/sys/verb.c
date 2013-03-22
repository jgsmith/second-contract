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

  if(id) {
    if(!verb_data[id]) return nil;
    names = verb_data[id] -> get_verbs() - v -> get_verbs();
    for(i = 0, n = sizeof(names); i < n; i++)
      if(verb_handlers[names[i]]) verb_handlers[names[i]] -= ({ id });
  }
  else {
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
  names = v -> get_verbs();
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(verb_handlers[names[i]]) verb_handlers[names[i]] |= ({ id });
    else verb_handlers[names[i]] = ({ id });
  }

  call_out("update_grammar", 0);

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
      verb_handlers[names[i]] -= ({ id });
      if(sizeof(verb_handlers[names[i]]) == 0) verb_handlers[names[i]] = nil;
    }
  }

  verb_data[id] = nil;
}

object *get_verb_handlers(string verb, int args) {
  object *list;
  string *ids;
  int i, n;

  if(verb_handlers[verb]) ids = verb_handlers[verb] + ({ });
  else list = ({ });

  list = allocate(sizeof(ids));
  for(i = 0, n = sizeof(ids); i < n; i++) {
    list[i] = verb_data[ids[i]];
    if(list[i]->is_disabled()) list[i] = nil;
    if(!list[i]->uses_args(args)) list[i] = nil;
  }
  return list - ({ nil });
}

string get_verb_help(string verb) {
  string *list;
  string *ret;
  object ob;
  int i, n;

  if(verb_handlers[verb]) list = verb_handlers[verb];
  else list = ({ });

  ret = ({ });
  for(i = 0, n = sizeof(list); i < n; i++) {
    ob = verb_data[list[i]];
    if(ob) {
      ret += ({ ob->get_brief(), "", ob -> get_help() }) - ({ nil });
      if(ob->get_see_also() && sizeof(ob->get_see_also())) {
        ret += ({ "See also: " + implode(ob->get_see_also(), ", ") });
      }
    }
    ret += ({ "" });
  }

  return implode(ret - ({ nil }), "\n");
}
