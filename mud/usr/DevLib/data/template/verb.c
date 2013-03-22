# include <toollib.h>
# include <toollib/template.h>
# include <type.h>

string var;
string verb;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_name(string s) { var = s; }

void set_verb(string v) { verb = v; }

string render(mapping data, object pov) {
  string pov_var;
  string nom;

  if(pov == data["actor"]) pov_var = "actor";
  else if(pov == data["this"]) pov_var = "this";
  else if(data["indirect"] && sizeof(({ pov }) & data["indirect"]))
    pov_var = "indirect";
  else if(data["direct"] && sizeof(({ pov }) & data["direct"]))
    pov_var = "direct";
  else if(data["instrument"] && sizeof(({ pov }) & data["instrument"]))
    pov_var = "instrument";

  /* render the verb based on the pov and var */
  if(pov_var == var || var == "last_substitution" && pov_var == data["_last_var"]) {
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
