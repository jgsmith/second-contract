# include <worldlib.h>
# include <data.h>

/*
 * A set of sensations that act together, but with different intensities.
 * A loud Boom might be accompanied by a bright flash, for example, but
 * with proper muffs, only the flash will be sensed (or only the boom will
 * make it to the other room).
 *
 * Sensations can be acute or chronic. 
 */

object SENSATION_DATA *sensations;
/*
 * We can link to a followup set of sensations
 */
object SENSATION_SET next;
/*
 * We wait this long before triggering
 */
int delay;

/*
 * Together with the "next" and "delay" settings, we can schedule a set 
 * of sensations to act together but with a delay. For example, seeing the
 * flash and then hearing the boom. We're not including distance from the
 * originating location in the delay calculation.
 */

void create(varargs int clone) {
  if(clone) {
    sensations = ({ });
    delay = 0;
  }
}

void add_sensation(object SENSATION_DATA s) {
  sensations += ({ s });
}

void add_sensations(object SENSATION_DATA *s) {
  sensations += s - ({ nil });
}

object SENSATION_DATA *get_sensations() {
  return sensations + ({ });
}

void set_delay(int d) {
  if(d >= 0) delay = d;
}

int get_delay() { return delay; }

void set_next_set(object SENSATION_SET s) {
  if(next) s -> set_next_set(next);
  next = s;
}

object SENSATION_SET get_next() { return next; }

void append_set(object SENSATION_SET s) {
  object n, nn;

  if(!next) {
    next = s;
  }
  else {
    n = next;
    while(nn = n->get_next()) n = nn;
    n -> append_set(s);
  }
}
