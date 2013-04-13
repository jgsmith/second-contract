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

  if(clone) { }
}

/* this isn't a collection of resources */
int resource_exists() {
  return typeof(get_resource_id()) == T_NIL ||
         typeof(ADVERB_D -> get_adverb(get_resource_id())) != T_NIL;
}

mapping data_for_adverb(string verb) {
  object adverb_data;

  adverb_data = ADVERB_D -> get_adverb(verb);
  if(adverb_data) return adverb_data -> get_properties();
  else return ([ ]);
}

mixed to_json(mapping metadata) {
  string *list;
  string *adverbs;
  int i, n;

  if(get_resource_id()) {
    return JSON_P -> to_json(data_for_adverb(get_resource_id()) + ([
      "id": get_resource_id(),
    ]));
  }
  else {
    /* we want to list all of the available verbs */
    list = ({ "[" });
    adverbs = ADVERB_D -> get_adverbs();
    for(i = 0, n = sizeof(adverbs); i < n; i++) {
      list += ({ 
        (i ? "," : "") +
        JSON_P -> to_json(data_for_adverb(adverbs[i]) + ([
          "id": adverbs[i],
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
  object ADVERB_DATA adverb;
  int args_used;

  json = implode(get_request() -> get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;
  /* now feed data back to verb object */
  id = get_resource_id();
  if(!id && info["adverb"]) {
    id = info["adverb"];
  }

  adverb = ADVERB_D -> get_adverb(id);
  if(!adverb) adverb = new_object(ADVERB_DATA);

  adverb -> set_adverb(info["adverb"]);
  adverb -> set_brief(info["brief"]);
  adverb -> set_help(info["help"]);

  ADVERB_D -> add_adverb(adverb);
  set_resource_id(info["adverb"]);
  return 200;
}

int post_is_create() { return 1; }

int create_path_after_handler() { return 1; }

string create_path() { return get_resource_id(); }

