# include <devlib.h>
# include <type.h>
# include <data.h>

/*
 * An event chain is a linked list of event sets
 *
 * Once an event set fails, the rest of the chain fails
 *
 * set guards can insert new sets at the current location or at the mirror
 * location.
 *
 * a -> b -> C -> b' -> a'
 *
 * Once a gets through consequents, b begins. If b inserts any sets, they come before b or after b'
 */

/* So we're running up the chain and then back to the beginning.
 * Thus, a set consists of forward and backward guards, consequents, etc.
 * If there is no next set, then we start down the previous chain.
 */

object EVENT_SET next, previous;

object EVENT_DATA *forward_guards, *backward_guards;
object EVENT_DATA *forward_consequents, *backward_consequents;
object EVENT_DATA *forward_reactions, *backward_reactions;

/* sets may not be inserted if forward_guards_run == 1 or if we're running
 * the backward event sets.
 */
int forward_guards_run;

void create(varargs int clone) {
  if(clone) {
    forward_guards = ({ });
    backward_guards = ({ });
    forward_consequents = ({ });
    backward_consequents = ({ });
    forward_reactions = ({ });
    backward_reactions = ({ });
  }
}

void set_next(object EVENT_SET e) {
  if(next) e -> set_next(next);
  e -> set_previous(this_object());
  next = e;
}

void set_previous(object EVENT_SET e) {
  if(previous) previous -> set_next(e);
  e -> set_next(this_object());
  previous = e;
}

/* guards and consequents are run in order */
object EVENT_DATA *get_guards(int f) {
  if(f) return forward_guards ? forward_guards : ({ });
  return backward_guards ? backward_guards : ({ });
}

void add_guard(object EVENT_DATA g) {
  forward_guards += ({ g }) - ({ nil });
}

void wrap_guards(object EVENT_DATA f, object EVENT_DATA b) {
  forward_guards += ({ f }) - ({ nil });
  backward_guards += ({ b }) - ({ nil });
}

object EVENT_DATA *get_consequents(int f) {
  if(f) return forward_consequents ? forward_consequents : ({ });
  return backward_consequents ? backward_consequents : ({ });
}

void add_consequent(object EVENT_DATA c) {
  forward_consequents += ({ c }) - ({ nil });
}

void wrap_consequents(object EVENT_DATA f, object EVENT_DATA b) {
  forward_consequents += ({ f }) - ({ nil });
  backward_consequents += ({ b }) - ({ nil });
}

/* reactions are run asynchronously */
object EVENT_DATA *get_reactions(int f) {
  if(f) return forward_reactions ? forward_reactions : ({ });
  return backward_reactions ? backward_reactions : ({ });
}

void add_reaction(object EVENT_DATA c) {
  forward_reactions += ({ c }) - ({ nil });
}

void wrap_reactions(object EVENT_DATA f, object EVENT_DATA b) {
  forward_reactions += ({ f }) - ({ nil });
  backward_reactions += ({ b }) - ({ nil });
}

/* new sets can only be inserted during guard execution */
/* guards may be run up to two times - if a guard inserts sets, then the
 * guard will be run again
 *
 * Guards should return a list of sets to be inserted.
 */

/* returns: 0 if no further action is possible
 *          1 if things are proceeding as normal
 *         <1 if we need to back up some (because we inserted sets)
 *
 * Any fail messages are passed to the actor object separately if this
 * is a verb action.
 */
int run_guards(int f) {
  object EVENT_DATA *g;
  int i, n, j;
  int added_sets;
  mixed ret;

  g = get_guards(f);
  added_sets = 0;
  if(!sizeof(g)) return TRUE; /* lack of guards doesn't stand in the way */
  for(i = 0, n = sizeof(g); i < n; i++) {
    ret = EVENTS_D -> call_event(g[i]);
    if(typeof(ret) == T_INT && !ret) return 0; /* nothing we can do */
    if(typeof(ret) == T_ARRAY) { /* we have a list of sets */
      if(forward_guards_run || !f) return FALSE;
      added_sets += sizeof(ret);
      /* we do this backward to make sure they run forward in the right order */
      for(j = sizeof(ret)-1; j >= 0; j--) set_previous(ret[j]);
    }
  }
  return added_sets ? -added_sets : TRUE;
}
