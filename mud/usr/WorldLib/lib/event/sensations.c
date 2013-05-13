# include <worldlib.h>
# include <data.h>

/*
 * Manages the chronic sensations associated with this object
 */

object SENSATION_DATA *sensations;

void create(varargs int clone) {
  if(clone) {
    sensations = ({ });
  }
}

int add_sensation(object SENSATION_DATA s) {
  int i, n;
  if(n = sizeof(sensations)) {
    for(i = 0; i < n; i++) {
      if(!sensations[i]) {
        sensations[i] = s;
        return i+1;
      }
    }
  }
  sensations += ({ s });
  return sizeof(sensations);
}

void remove_sensation(int id) {
  if(id < 1 || id > sizeof(sensations)) return;

  sensations[id-1] = nil;
}

object SENSATION_DATA *get_sensations(varargs string sense, int intensity) { 
  object *list;
  int i, n;

  list = sensations + ({ });
  if(sense && sense != "") {
    for(i = 0, n = sizeof(list); i < n; i++)
      if(list[i]->get_sense() != sense) list[i] = nil;
    list -= ({ nil });
  }
  if(intensity && intensity > 0) {
    for(i = 0, n = sizeof(list); i < n; i++)
      if(list[i]->get_intensity() < intensity) list[i] = nil;
    list -= ({ nil });
  }
  return list; 
}