# include <system.h>
# include <system/http.h>
# include <toollib.h>
# include <iflib.h>
# include <iflib/verb.h>
# include <type.h>
# include <status.h>
# include <kernel/kernel.h>

inherit resource HTTP_RESOURCE_LIB;

static void create(varargs int clone) {
  resource::create(clone);

  set_authentication_flags(HTTP_ALLOWS_PUBLIC_READ);

  if(clone) {
  }
}

/* this isn't a collection of resources */
int resource_exists() {
  return typeof(get_resource_id()) == T_NIL ||
         typeof(VERB_D -> get_verb(get_resource_id())) != T_NIL;
}

mixed *content_types_provided() {
  return ({ ({ "application/json", "to_json" }) });
}

mixed *content_types_accepted() {
  return ({ ({ "application/json", "from_json" }) });
}

mapping data_for_verb(string verb) {
  object verb_data;

  verb_data = VERB_D -> get_verb(verb);
  if(verb_data) return verb_data -> get_properties();
  else return ([ ]);
}

mixed to_json(mapping metadata) {
  string *list;
  string *verbs;
  int i, n;

  if(get_resource_id()) {
    return JSON_P -> to_json(data_for_verb(get_resource_id()) + ([
      "id": get_resource_id(),
    ]));
  }
  else {
    /* we want to list all of the available verbs */
    list = ({ "[" });
    verbs = VERB_D -> get_verb_ids();
    for(i = 0, n = sizeof(verbs); i < n; i++) {
      list += ({ 
        (i ? "," : "") +
        JSON_P -> to_json(data_for_verb(verbs[i]) + ([
          "id": verbs[i],
        ]))
      });
    }
    list += ({ "]" });
    return list;
  }
}

mixed from_json(mapping metadata) {
  mixed info;
  string json;
  string id;
  object VERB_DATA verb;
  int args_used;

  json = implode(get_request() -> get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;
  /* now feed data back to verb object */
  id = get_resource_id();

  if(id) {
    /* we're updating */
    verb = VERB_D -> get_verb(id);
  }
  else {
    /* we're creating a new one */
    verb = new_object(VERB_DATA);
  }

  if(info["verbs"] && typeof(info["verbs"]) == T_ARRAY)
    verb -> set_verbs(info["verbs"]);
  if(info["action"] && typeof(info["action"]) == T_ARRAY)
    verb -> set_action(info["action"] - ({ "" }));
  if(info["brief"]) verb -> set_brief(info["brief"]);
  if(info["help"]) verb -> set_help(info["help"]);
  if(typeof(info["disabled"]) == T_INT) verb->set_disabled(info["disabled"]);
  if(info["see_also"] && typeof(info["see_also"]) == T_ARRAY)
    verb -> set_see_also(info["see_also"]);

  args_used = verb -> get_args_used();
  if(typeof(info["arguments"]) == T_MAPPING) {
    if(typeof(info["arguments"]["direct"]) == T_MAPPING) {
      if(typeof(info["arguments"]["direct"]["used"]) == T_INT) {
        if(info["arguments"]["direct"]["used"])
          args_used |= ARG_DIRECT_OBJ;
        else
          args_used &= ~ARG_DIRECT_OBJ;
      }
    }
    if(typeof(info["arguments"]["indirect"]) == T_MAPPING) {
      if(typeof(info["arguments"]["indirect"]["used"]) == T_INT) {
        if(info["arguments"]["indirect"]["used"])
          args_used |= ARG_INDIRECT_OBJ;
        else
          args_used &= ~ARG_INDIRECT_OBJ;
      }
    }
    if(typeof(info["arguments"]["instrumental"]) == T_MAPPING) {
      if(typeof(info["arguments"]["instrumental"]["used"]) == T_INT) {
        if(info["arguments"]["instrumental"]["used"])
          args_used |= ARG_IMPLEMENT_OBJ;
        else
          args_used &= ~ARG_IMPLEMENT_OBJ;
      }
    }
    if(typeof(info["arguments"]["communication"]) == T_INT) {
      if(info["arguments"]["communication"])
        args_used |= ARG_COMMUNICATION;
      else
        args_used &= ~ARG_COMMUNICATION;
    }
  }
  verb -> set_args_used(args_used);

  if(id) {
    VERB_D -> add_verb(verb, id);
    return 200;
  }
  else {
    id = VERB_D -> add_verb(verb);
    set_resource_id(id);
    return 200;
  }
}

int post_is_create() { return 1; }

int create_path_after_handler() { return 1; }

string create_path() { return get_resource_id(); }

