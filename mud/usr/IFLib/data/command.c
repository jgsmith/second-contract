# include <iflib.h>
# include <iflib/binder.h>
# include <worldlib/proximity.h>
# include <type.h>
# include <toollib.h>
# include <devlib.h>
# include <data.h>

/*
 * All the data associated with the command as well as some logic to run it
 */

string verb;
string *adverbs;
mixed *direct;
mixed *indirect;
mixed *instrument;
mixed *evocation;
mixed *topic;

void default_error(object actor);

void create(varargs int clone) {
  if(clone) {
    adverbs = ({ });
    direct = ({ });
    indirect = ({ });
    instrument = ({ });
    evocation = ({ });
    topic = ({ });
  }
}

void set_verb(string s) { verb = s; }
void add_adverb(string s) { adverbs |= ({ s }); }

void add_direct(mixed *bits) { direct += bits; }
void add_indirect(string prep, mixed *bits) { indirect += ({ ({ prep, bits }) }); }
void add_instrument(string prep, mixed *bits) { instrument += ({ ({ prep, bits }) }); }

void add_evocation(string e) { evocation += ({ e }); }

void add_topic(string t) { topic += ({ t }); }

/* ({ words... }), prep, ({ words... }) ... */
string unparse_noun_phrase(mixed *phrase, varargs string holding) {
  int i, n;
  string *ret;

  if(!holding) holding = "holding";
  ret = ({ });
  for(i = 0, n = sizeof(phrase); i < n; i++) {
    if(typeof(phrase[i]) == T_ARRAY && typeof(phrase[i][0]) == T_STRING) ret += phrase[i];
    else if(typeof(phrase[i]) == T_INT) {
      switch(phrase[i]) {
        case PROX_INSIDE: ret += ({ "in" }); break;
        case PROX_ON: ret += ({ "on" }); break;
        case PROX_AGAINST: ret += ({ "against" }); break;
        case PROX_CLOSE: ret += ({ "close to" }); break;
        case PROX_UNDER: ret += ({ "under" }); break;
        case PROX_NEAR: ret += ({ "near" }); break;
        case PROX_OVER: ret += ({ "over" }); break;
        case PROX_BEHIND: ret += ({ "behind" }); break;
        case PROX_BEFORE: ret += ({ "before" }); break;
        case PROX_BESIDE: ret += ({ "beside" }); break;
        case PROX_CONTAINS: ret += ({ holding }); break;
        case PROX_GUARD: ret += ({ "guarding" }); break;
        case PROX_BLOCK: ret += ({ "blocking" }); break;
      }
    }
  }
  return implode(ret, " ");
}

int execute(object actor) {
  /* We see if we can find a proper verb of the right type */
  int num_args;
  object *verb_objs;
  object verb_obj;
  string *bits;
  string *actions;
  string tmp;
  int i, n, j, m;
  object BINDER_MATCH direct_obs;
  object BINDER_MATCH indirect_obs;
  object BINDER_MATCH instrument_obs;
  object EVENT_DATA e;
  object EVENT_SET e_set, estmp;
  object ADVERB_DATA effective_adverb;
  object ADVERB_DATA adverb;

  if(sizeof(adverbs)) {
    effective_adverb = ADVERB_D -> get_effective_adverb(adverbs);
    if(!effective_adverb) {
      default_error(actor);
      return FALSE;
    }
  }

  num_args = 0;
  if(sizeof(direct) > 0) num_args |= 1;
  if(sizeof(indirect) > 0) num_args |= 2;
  if(sizeof(instrument) > 0) num_args |= 4;

  verb_objs = VERB_D -> get_verb_handlers(verb, num_args);
  if(verb_objs) {
    verb_objs -= ({ nil });
  }
  /* now go through and see which verb handlers work with the current
   * set of objects - use the first one that matches
   */
  for(j = 0, m = sizeof(verb_objs); j < m; j++) {
    verb_obj = verb_objs[j];
    if(direct && sizeof(direct)) {
      /* if direct_obs are expected to be in indirect_obs, then we need to
       * look for indirect_obs first
       */
      direct_obs = BINDER_D -> bind_direct(actor, BINDER_MATCH_SINGULAR, direct);
    }

    if(instrument && sizeof(instrument))
      instrument_obs = BINDER_D -> bind_instrument(actor, instrument);

    if(!indirect_obs && indirect && sizeof(indirect)) {
      indirect_obs = BINDER_D -> bind_indirect(actor, indirect, direct_obs);
    }

    /* we need to construct an event chain that will be executed with this
     * set of objects.
     */

    /* build out the event set for the first verb handler */
    actions = verb_obj->get_action();
    adverb = verb_obj->get_modified_adverb(effective_adverb);

    for(i = 0, n = sizeof(actions); i < n; i++) {
      estmp = new_object(EVENT_SET);
      e = new_object(EVENT_DATA);
      e -> set_event_type("pre-" + actions[i]);
      e -> set_object(actor);
      e -> set_args(([
        "actor": actor,
        "direct": (direct_obs ? direct_obs -> get_objects() : ({ })),
        "indirect": (indirect_obs ? indirect_obs -> get_objects() : ({ })),
        "instrument": (instrument_obs ? instrument_obs -> get_objects() : ({ })),
        "topic": topic,
        "evocation": evocation,
        "verb": verb,
        "adverb": adverbs,
        "mods": adverb,
      ]));
      estmp -> add_guard(e);
      e = new_object(e);
      e -> set_event_type(actions[i]);
      estmp -> add_consequent(e);
      e = new_object(e);
      e -> set_event_type("post-"+actions[i]);
      estmp -> add_reaction(e);
      if(e_set) e_set -> add_next(estmp);
      else e_set = estmp;
    }
    /* if successful, then return - otherwise, remove the handler and
     * try again
     */
    if(EVENTS_D -> run_event_set(e_set)) return TRUE;
  }
  
  /* if we get here, we weren't able to act on the command */
  default_error(actor);
  return FALSE;
}

void default_error(object actor) {
  string *bits;
  int i, n;
  string tmp;

  bits = ({ "You", "can't" });
  if(adverbs && sizeof(adverbs))
    bits += ({ ENGLISH_D -> item_list(adverbs) });
  bits += ({ verb });
  if(sizeof(direct)) {
    tmp = "";
    for(i = 0, n = sizeof(direct); i < n; i++) {
      tmp += unparse_noun_phrase(direct[i]);
      if(n > 2 && i == n-2) {
        tmp += ", and ";
      }
      else if(n == 2 && i == 0) {
        tmp += " and ";
      }
      else if(n > 1) {
        tmp += ", ";
      }
    }
    bits += ({ tmp });
  }
  if(sizeof(indirect)) {
    tmp = "";
    for(i = 0, n = sizeof(indirect); i < n; i++) {
      tmp += indirect[i][0] + " " + implode(indirect[i][1], " ");
      if(n > 2 && i == n-2) {
        tmp += ", and ";
      }
      else if(n == 2 && i == 0) {
        tmp += " and ";
      }
      else if(n > 1) {
        tmp += ", ";
      }
    }
    bits += ({ tmp });
  }
  if(sizeof(instrument)) {
    tmp = "";
    for(i = 0, n = sizeof(instrument); i < n; i++) {
      tmp += instrument[i][0] + " " + implode(instrument[i][1], " ");
      if(n > 2 && i == n-2) {
        tmp += ", and ";
      }
      else if(n == 2 && i == 0) {
        tmp += " and ";
      }
      else if(n > 1) {
        tmp += ", ";
      }
    }
    bits += ({ tmp });
  }
  /* we want this to be a pending fail message since we might have other
   * options.
   */
  actor -> message(implode(bits - ({ nil, "" }), " ") + ".\n");
}
