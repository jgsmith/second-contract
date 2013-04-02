/*
 * All non-transient info about this object. These can change, but they
 * don't change often, and are considered fundamental. Nothing in a
 * heartbeat or through a normal verb should modify anything here.
 */

int living;
int medium;
int terrain;
int respiration;
int height;
mapping senses;
string skin; /* what object is produced by skinning this thing.
              * This should be the name that can be given to a hospital
              * to get the ur object from the "skins" ward or similar.
              */
string description; /* a description of the genetics - the race/species */

void create(varargs int clone) {
  if(clone) {
    living = FALSE;
  }
}

/*
 * -2 - unobservable
 * -1 - unnoticable
 *  0 - normal
 *  1 - noticable
 *  2 - overwhelming
 */
int is_dark(string sense, int level) {
  if(!senses[sense]) return -2; /* unobservable */
  if(level < senses[sense][0]) return -2;
  if(level < senses[sense][1]) return -1;
  if(level > senses[sense][3]) return 2;
  if(level > senses[sense][2]) return 1;
  return 0;
}

void set_sense(string sense, int *levels) {
  senses[sense] = levels;
}

void set_skin(string s) { skin = s; }
string get_skin() { return skin; }

int is_living() { return living; }

int get_medium() { return medium; }
int get_terrain() { return terrain; }
int get_respiration() { return respiration; }

void set_living(int l) { living = l; }

void set_description(string d) { description = d; }
string get_description() { return description; }


