# include <system.h>
# include <iflib.h>
# include <kernel/kernel.h>

/*
 * This daemon tracks all of the available adverbs that we care about.
 * Typically, we'll add adverbs when we want to allow meaningful use of
 * them in the game. If an adverb is here, then it should be used somewhere.
 *
 * We can include some help with the adverbs to hint at where they might be
 * most useful beyond simple role playing.
 */

/* We don't anticipate having over a 1,000 adverbs in a game. */
mapping adverbs;

void create(varargs int clone) {
  adverbs = ([ ]);
}

int add_adverb(object adv) {
  adverbs[adv->get_adverb()] = adv;
  return TRUE;
}

int query_adverb(string adv) {
  if(adverbs[adv]) return TRUE;
  return FALSE;
}

string *get_adverbs() { return ({ "quickly", "slowly" }); }
