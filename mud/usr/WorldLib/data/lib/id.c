# include <iflib.h>
# include <worldlib.h>

/*
 * Holds information about identity and description.
 */

string *adjectives;
string *nouns;
string *plural_adjectives;
string *plural_nouns;
mapping descriptions;

void create(varargs int clone) {
  adjectives = ({ });
  nouns = ({ });
  descriptions = ([ ]);
  plural_nouns = ({ });
  plural_adjectives = ({ });
}

mixed get_property(string *path) {
  switch(path[0]) {
    case "adjective":
      if(sizeof(path) > 1)
        return sizeof(({ path[1] }) & adjectives) > 0;
      else
        return adjectives;
    case "noun":
      if(sizeof(path) > 1)
        return sizeof(({ path[1] }) & nouns) > 0;
      else
        return nouns;
    case "plural-adjective":
      if(sizeof(path) > 1)
        return sizeof(({ path[1] }) & plural_adjectives) > 0;
      else
        return plural_adjectives;
    case "plural-noun":
      if(sizeof(path) > 1)
        return sizeof(({ path[1] }) & plural_nouns) > 0;
      else
        return plural_nouns;
    case "description":
      if(sizeof(path) > 1)
        return descriptions[path[1]];
      else
        return map_indices(descriptions);
  }
  return nil;
}

int set_property(string *path, mixed value) {
  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "noun":
      if(sizeof(path) == 2) {
        if(value) nouns = nouns | ({ path[1] });
        else nouns -= ({ path[1] });
        return TRUE;
      }
      return FALSE;
    case "adjective":
      if(sizeof(path) == 2) {
        if(value) adjectives |= ({ path[1] });
        else adjectives -= ({ path[1] });
        return TRUE;
      }
      return FALSE;
    case "plural-noun":
      if(sizeof(path) == 2) {
        if(value) plural_nouns = plural_nouns | ({ path[1] });
        else plural_nouns -= ({ path[1] });
        return TRUE;
      }
      return FALSE;
    case "plural-adjective":
      if(sizeof(path) == 2) {
        if(value) plural_adjectives |= ({ path[1] });
        else plural_adjectives -= ({ path[1] });
        return TRUE;
      }
      return FALSE;
    case "description":
      if(sizeof(path) == 2) {
        descriptions[path[1]] = value;
        return TRUE;
      }
      return FALSE;
  }
  return FALSE;
}
