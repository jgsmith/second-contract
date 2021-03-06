# include <toollib.h>
# include <toollib/template.h>
# include <devlib.h>
# include <devlib/template.h>
# include <type.h>

string var;
int pos;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_name(string s) { var = s; }

void set_pos(int p) { pos = p; }

string render(mapping data, object pov) {
  string pov_var;
  string nom;

  pov_var = "observer";
  if(pov == data["actor"]) pov_var = "actor";
  else if(pov == data["this"]) pov_var = "this";
  else if(data["indirect"] && sizeof(({ pov }) & data["indirect"]))
    pov_var = "indirect";
  else if(data["direct"] && sizeof(({ pov }) & data["direct"]))
    pov_var = "direct";
  else if(data["instrument"] && sizeof(({ pov }) & data["instrument"]))
    pov_var = "instrument";

  if(pov_var != "observer") data["_last_var"] = var;
  if(pov_var == var) {
    switch(pos) {
      case POS_NOMINATIVE: case POS_NAME: case POS_OBJECTIVE:
        nom = "you"; break;
      case POS_POSSESSIVE_NOUN: case POS_POSSESSIVE:
        nom = "your"; break;
      case POS_REFLEXIVE:
        nom = "yourself"; break;
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
          if(var == "actor")
            return ENGLISH_D -> item_list(data[var]);
          else
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
