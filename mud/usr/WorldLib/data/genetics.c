/*
 * All non-transient info about this object
 */

int living;
int medium;
int terrain;
int respiration;

void create(varargs int clone) {
  if(clone) {
    living = FALSE;
  }
}

int is_living() { return living; }

int get_medium() { return medium; }
int get_terrain() { return terrain; }
int get_respiration() { return respiration; }

void set_living(int l) { living = l; }
