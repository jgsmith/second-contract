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

void destructed(int clone) {
  int i, n;
  object *obs;

  if(clone) {
    obs = map_values(objects);
    for(i = 0, n = sizeof(obs); i < n; i++)
      destruct_object(obs[i]);
  }
}

string add_object(object ur) {
  string *nouns;
  int i, n;
  string id;

  if(ur -> is_ur_object()) return nil;
  if(!ur -> is_valid_object()) return nil;

  ur -> set_is_ur_object(1);

  do {
    id = "";
    for(i = 0; i < 5; i++) {
      n = random(20);
      id += "bcdfghjklmnpqrstvwxz"[n..n];
      n = random(6);
      id += "aeiouy"[n..n];
    }
  } while(objects[id]);
  objects[id] = ur;
  return id;
}

object get_object(string name) {
  string *input, *adj_prop, *adjs;
  string noun;
  object *candidates;
  int i, n, adj_len;

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
