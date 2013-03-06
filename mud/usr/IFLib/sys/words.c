/*
 * Provides translations of various constants into words.
 */

# include <worldlib.h>
# include <iflib.h>
# include <devlib.h>

# include <worldlib/proximity.h>

mapping words;

static void create(varargs int clone) {
  words = ([
    "prepositions": ({
      "in", "close", "against", "under", "on", "near", "over",
      "behind", "before", "beside", "contains", "guards", "blocks",
    }),
    "distances": ({
      "in", "on", "close", "near", "vicinity", "far", "backdrop"
    }),
    "positions": ({
      "standing", "sitting", "lying", "flying", "leaning",
      "kneeling", "floating", "swimming"
    }),
    "respirations": ({
      "air", "water", "vacuum", "methane"
    }),
    "media": ({
      "land", "air", "surface", "space", "water", "methane",
    }),
    "terrains": ({
      "underground", "biological", "road", "underwater", "surface",
      "midair", "swamp", "woods", "jungle", "rough", "seafloor",
      "outdoors", "indoors", "space", "magma", "plasma", "planar",
      "snow", "sand", "ice", "shallow-water", "indoor-window", "rooftop"
    }),
  ]);
}

static string word_from_list(string list, int pos, varargs int offset){
  if(!words[list]) return "";
  pos -= offset;
  if(pos < 0 || pos >= sizeof(words[list])) return "";
  return words[list][pos];
}

static int int_from_list(string list, string word, varargs int offset) {
  int i, n;
  if(!words[list]) return -1;
  for(i = 0, n = sizeof(words[list]); i < n; i++)
    if(words[list][i] == word) return i + offset;
  return -1;
}

int query_in_list(string list, string word) {
  return int_from_list(list, word) != -1;
}

string preposition_name(int prep) {
  return word_from_list("prepositions", prep);
}

int preposition_number(string prep) {
  return int_from_list("prepositions", prep);
}

string position_name(int p) {
  return word_from_list("positions", p, 1);
}

string position_word(int p) { return position_name(p); }

int position_number(string p) {
  return int_from_list("positions", p, 1);
}

string medium_name(int p) {
  return word_from_list("media", p, 1);
}

int medium_position(string p) {
  return int_from_list("media", p, 1);
}

string distance_name(int d) {
  return word_from_list("distances", d);
}

int distance_number(string d) {
  return int_from_list("distances", d);
}

static string *gather_strings_from_bits(string list, int d) {
  int mask, i, n;
  string *ret;
  if(!words[list]) return ({ });
  ret = ({ });
  mask = 1;
  for(i = 0, n = sizeof(words[list]); i < n; i++, mask<<1) {
    if(d & mask) ret += words[list][i];
  }
  return ret;
}

static int gather_int_from_strings(string list, string *bits) {
  int mask, i, n;
  int ret;
  if(!bits || sizeof(bits) == 0) return 0;
  mask = 1;
  ret = 0;
  for(i = 0, n = sizeof(words[list]); i < n; i++, mask<<1) {
    if(sizeof(({ words[list][i] }) & bits)) ret |= mask;
  }
  return ret;
}

string *terrain_name(int d) {
  return gather_strings_from_bits("terrains", d);
}

int terrain_number(string *d) {
  return gather_int_from_strings("terrains", d);
}

string *respiration_name(int d) {
  return gather_strings_from_bits("respirations", d);
}

int respiration_number(string *d) {
  return gather_int_from_strings("respirations", d);
}
