# include <devlib.h>

/*
 * Holds ur objects and finds them based on name and details.
 */

mapping objects;

static void create(varargs int clone) {
  objects = ([ ]);
}

mapping get_objects() { return objects; }

int add_object(object ur) {
  string *nouns;
  int i, n;

  if(ur -> is_ur_object()) return FALSE;
  if(!ur -> is_valid_object()) return FALSE;

  nouns = ur->get_property(({ "detail", "default", "noun" }));
  if(!nouns || sizeof(nouns) == 0) return FALSE;
  for(i = 0, n = sizeof(nouns); i < n; i++)
    if(objects[nouns[i]] && sizeof(({ ur }) & objects[nouns[i]])) return 0;
  for(i = 0, n = sizeof(nouns); i < n; i++)
    if(objects[nouns[i]]) objects[nouns[i]] += ({ ur }) - ({ nil });
    else objects[nouns[i]] = ({ ur });

  ur -> set_is_ur_object(1);

  return 1;
}

object get_object(string name) {
  string *input, *adj_prop, *adjs;
  string noun;
  object *candidates;
  int i, n, adj_len;

  candidates = ({ });
  adj_prop = ({ "detail", "default", "adjective" });
  input = explode(name, " ");
  noun = input[sizeof(input)-1];
  input = input[0..sizeof(input)-2];
  adj_len = sizeof(input);

  if(!objects[noun]) return nil;
  objects[noun] -= ({ nil });
  if(!sizeof(objects[noun])) return nil;

  for(i = 0, n = sizeof(objects[noun]); i < n; i++) {
    if(sizeof(input & (adjs = objects[noun][i]->get_property(adj_prop))) == adj_len) {
      /* if our list of adjectives exactly matches the adjectives for the
       * object, go ahead and return it. Otherwise, add it to the list of
       * candidates.
       */
      if(sizeof(adjs) == adj_len) return objects[noun][i];
      else candidates += ({ objects[noun][i] });
    }
  }
  if(sizeof(candidates) == 1) return candidates[0];
  return nil;
}

string *get_object_names() {
  /* list each object with its adjectives followed by its nouns */
  /* {adjectives} {nouns} */
  int i, j, n, m;
  string *nouns;
  string *results, *adjs;

  results = ({ });

  nouns = map_indices(objects);
  for(i = 0, n = sizeof(nouns); i < n; i++) {
    for(j = 0, m = sizeof(objects[nouns[i]]); j < m; j++) {
      adjs = objects[nouns[i]][j] -> get_property(({ "detail", "default", "adjective" }));
      if(adjs && sizeof(adjs) > 0) {
        results += ({ "{" + implode(adjs, "|") + "} " + nouns[i] });
      }
      else {
        results += ({ nouns[i] });
      }
    }
  }

  return results;
}
