# include <devlib.h>

/*
 * Holds ur objects and finds them based on name and details.
 */

mapping objects;

static void create(varargs int clone) {
  if(clone) {
    objects = ([ ]);
  }
}

mapping get_objects() { return objects; }

string *get_object_names() { return map_indices(objects); }

void destructed(int clone) {
  int i, n;
  object *obs;

  if(clone) {
    obs = map_values(objects);
    for(i = 0, n = sizeof(obs); i < n; i++)
      destruct_object(obs[i]);
  }
}

atomic int add_object(string id, object ur) {
  string *nouns;
  int i, n;

  if(ur -> is_template_object()) return FALSE;

  if(objects[id]) return FALSE;

  objects[id] = ur;
  return TRUE;
}

int query_object_exists(string name) {
  if(objects[name]) return TRUE;
  return FALSE;
}

object get_object(string name) {
  string *input, *adj_prop, *adjs;
  string noun;
  object *candidates;
  int i, n, adj_len;

  if(!name) return nil;

  if(objects[name]) return objects[name];

  candidates = ({ });
  input = explode(name, " ");
  noun = input[sizeof(input)-1];
  input = input[0..sizeof(input)-2];
  adj_len = sizeof(input);

  candidates = map_values(objects);
  for(i = 0, n = sizeof(candidates); i < n; i++) {
    if(!sizeof(({ noun }) & candidates[i]->parse_command_id_list()))
      candidates[i] = nil;
  }
  candidates -= ({ nil });

  for(i = 0, n = sizeof(candidates); i < n; i++) {
    if(sizeof(input & (adjs = candidates[i]->parse_command_adjective_id_list())) == adj_len) {
      /* if our list of adjectives exactly matches the adjectives for the
       * object, go ahead and return it. Otherwise, add it to the list of
       * candidates.
       */
      if(sizeof(adjs) == adj_len) return candidates[i];
    }
    else {
      candidates[i] = nil;
    }
  }
  candidates -= ({ nil });
  if(sizeof(candidates) == 1) return candidates[0];
  return nil;
}
