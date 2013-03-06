# include <toollib.h>
# include <toollib/template.h>
# include <type.h>

string var;
string verb;
int pos;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_name(string s) { var = s; }

void set_pos(int p) { pos = p; }

void set_verb(string v) { verb = v; }

static item_list(mixed *list, object pov, string pov_repl) {
}

string render(mapping data, object pov) {
  string pov_var;
  string nom;

  if(pov == data["this"]) pov_var == "this";
  else if(pov == data["actor"]) pov_var == "actor";
  else if(data["indirect"] && sizeof(({ pov }) & data["indirect"]))
    pov_var = "indirect";
  else if(data["direct"] && sizeof(({ pov }) & data["direct"]))
    pov_var = "direct";
  else if(data["instrument"] && sizeof(({ pov }) & data["instrument"]))
    pov_var = "instrument";

  if(pos & POS_VERB) {
    /* render the verb based on the pov and var */
    if(pov_var == var || var == "last_var" && pov_var == data["_last_var"]) {
      /* return the verb as-is, appropriate for "you ..." */
      return verb;
    }
    if(var == "last_var" && data["_last_var"]) pov_var = data["_last_var"];
    if(pov_var == "this" || pov_var == "actor") {
      return ENGLISH_D -> pluralize(verb);
    }
    if(data[pov_var] && sizeof(data[pov_var]) > 1) {
      return verb;
    }
    return ENGLISH_D -> pluralize(verb);
  }

  if(pov_var == var) {
    switch(pos) {
      case POS_NOMINATIVE: case POS_NAME: case POS_OBJECTIVE:
        nom = "you";
      case POS_POSSESSIVE_NOUN: case POS_POSSESSIVE:
        nom = "your";
      case POS_REFLEXIVE:
        nom = "yourself";
    }
    if(typeof(data[var]) == T_ARRAY) {
      return ENGLISH_D -> item_list(data[var] - ({ pov }) + ({ nom }));
    }
    else return nom;
  }

  if(!data[var]) return "";
  switch(var) {
    case "this": case "actor":
    case "direct": case "indirect": case "instrument":
      data["_last_var"] = var;
      switch(pos) {
        case POS_NOMINATIVE:
          return ENGLISH_D -> nominative(data[var]);
        case POS_NAME: 
          return ENGLISH_D -> item_list(data[var]);
        case POS_OBJECTIVE:
          return ENGLISH_D -> objective(data[var]);
        case POS_POSSESSIVE_NOUN:
          return ENGLISH_D -> possessive_noun(data[var]);
        case POS_POSSESSIVE:
          return ENGLISH_D -> possessive(data[var]);
        case POS_REFLEXIVE:
          return ENGLISH_D -> reflexive(data[var]);
      }
      break;
  }
  return "";
}
