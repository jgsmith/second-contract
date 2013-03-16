# include <iflib.h>
# include <iflib/binder.h>
# include <worldlib/proximity.h>
# include <type.h>

/*
 * All the data associated with the command as well as some logic to run it
 */

string verb;
string *adverbs;
mixed *direct;
mixed *indirect;
mixed *instrument;

void create(varargs int clone) {
  if(clone) {
    adverbs = ({ });
    direct = ({ });
    indirect = ({ });
    instrument = ({ });
  }
}

void set_verb(string s) { verb = s; }
void add_adverb(string s) { adverbs |= ({ s }); }

void add_direct(mixed *bits) { direct += bits; }
void add_indirect(string prep, mixed *bits) { indirect += ({ ({ prep, bits }) }); }
void add_instrument(string prep, mixed *bits) { instrument += ({ ({ prep, bits }) }); }

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
  object *verb_obj;
  string *bits;
  string tmp;
  int i, n;
  object BINDER_MATCH direct_obs;
  object BINDER_MATCH indirect_obs;
  object BINDER_MATCH instrument_obs;

  num_args = 0;
  if(sizeof(direct) > 0) num_args |= 1;
  if(sizeof(indirect) > 0) num_args |= 2;
  if(sizeof(instrument) > 0) num_args |= 4;

  verb_obj = VERB_D -> get_verb_handlers(verb, num_args);
  if(!verb_obj || !sizeof(verb_obj)) {
    bits = ({ "You", "can't" });
    actor->message("adverbs: " + implode(adverbs, " ") + "\n");
    if(sizeof(adverbs) > 2) {
      bits += ({ implode(adverbs[0..sizeof(adverbs)-2], ", "), "and", adverbs[sizeof(adverbs)-1] });
    }
    else if(sizeof(adverbs) == 2) {
      bits += ({ adverbs[0], "and", adverbs[1] });
    }
    else {
      bits += adverbs;
    }
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
    return FALSE;
  }
  /*
   * we want to go through the adverbs and combine their modifications
   *
   * two adverbs can't modify the same aspect of the verb
   */

  if(direct && sizeof(direct))
    direct_obs = BINDER_D -> bind_direct(actor, BINDER_MATCH_SINGULAR, direct);
  /* we need to construct an event chain that will be executed with this
   * set of objects.
   */
  if(instrument && sizeof(instrument))
    instrument_obs = BINDER_D -> bind_instrument(actor, instrument);

  if(indirect && sizeof(indirect)) {
    indirect_obs = BINDER_D -> bind_indirect(actor, indirect, direct_obs);
  }
}
