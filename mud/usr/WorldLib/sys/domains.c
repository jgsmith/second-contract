# include <worldlib.h>
/*
 * Manages domain-oriented objects like hospitals and who can access what.
 */

mapping domains;

static void create(varargs int clone) {
  domains = ([ ]);
}

/* We expect domain:area:ward:unique_name */
object get_ur_object(string path) {
  string *bits;

  bits = explode(path, ":");
  if(!domains[bits[0]]) return nil;

  return domains[bits[0]] -> get_ur_object(bits[1..]);
}

object DOMAIN_DATA get_domain(string id) {
  return domains[id];
}

string *get_domains() { return map_indices(domains); }

void remove_domain(string id) {
  domains[id] = nil;
}

object DOMAIN_DATA create_domain(string id) {
  /* TODO: make sure previous object is the REST resource object */
  if(domains[id]) return nil;
  domains[id] = new_object(DOMAIN_DATA);
  return domains[id];
}
