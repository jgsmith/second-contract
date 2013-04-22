# include <iflib.h>
# include <kernel/kernel.h>

/*
 * Data about an adverb
 */

/*
 * Adverbs fall into several categories:
 *
 * Time: yesterday, last week, tomorrow, etc.
 * Place: here, below, etc. (we consider these adjectives)
 * Manner: badly, sadly, quickly, well, fast, ...
 * Degree: almost, much, quite, nearly, verb, ...
 * Frequency: almost, again, always, ever, frequently, usually, ...
 *
 * Our use will be primarily manner possibly modified by manner.
 *
 * For example, "run very quickly" will be slightly faster than "run quickly"
 */

/*
 * Adverbs here indicate how we manage constraints.
 *
 * Quickly: minimize time
 * Slowly: maximize time
 * Carefully: minimize required skill
 * Carelessly: maximize required skill
 *
 */

/* We have mappings because we want to indicate to which degree a constraint is
 * minimized or maximized.
 *
 * nil - no opinion
 *   1 - maximize
 *  -1 - minimize
 *   0 - use default (locked at default - not a variable in the calculations)
 *
 * Two adverbs in the same command can't modify the same constraint in opposite
 * signs. Some verbs might not make sense with multiple opposing constraints.
 *
 * For example, "quickly and carefully" might not work, but
 *   "carefully and slowly" might.
 */

string adverb;
string brief;
string help;

mapping constraints;

/*
 * Some adjectives are designed to minimize or maximize noticability.
 *
 * quietly - less noise (minimize sound)
 * stealthly - less sight & sound
 * loudly - more noise
 *
 * These are using senses as constraints.
 */
 

static void create(varargs int clone) {
  if(clone) {
    constraints = ([ ]);
  }
}

static int can_modify() {
  return TRUE; /* for testing */
  if(SYSTEM()) return TRUE;
  if(explode(object_name(previous_object()),"#")[0] == HTTP_ADVERB_RESOURCE) return TRUE;
  error("Adverbs may only be modified by System or through the web administrative interface.");
  return FALSE;
}

void set_constraint(string c, int d) {
  if(can_modify()) constraints[c] = d;
}

void clear_constraints() {
  if(can_modify()) constraints = ([ ]);
}

mapping get_constraints() { return constraints; }

void set_adverb(string a) {
  if(can_modify()) adverb = a;
}

string get_adverb() { return adverb; }

void set_brief(string b) {
  if(can_modify()) brief = b;
}

string get_brief() { return brief; }

void set_help(string h) {
  if(can_modify()) help = h;
}

string get_help() { return help; }
