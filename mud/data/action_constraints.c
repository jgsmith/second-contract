# include <iflib.h>
# include <kernel/kernel.h>

/*
 * In-process data based on adverbs or constraints on actions
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

void set_constraint(string c, int d) {
  if(can_modify()) constraints[c] = d;
}

mapping get_constraints() { return constraints; }

object ACTION_CONSTRAINT_DATA combine_constraints(object other) {
  mapping cons;
  object ACTION_CONSTRAINT_DATA new_acd;
  string *names;
  int i, n;

  cons = other -> get_constraints();
  new_acd = clone_object(this_object());
  names = map_indices(cons) | map_indices(constraints);
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(constraints[names[i]]) {
      if(cons[names[i]]) {
        if(constraints[names[i]] * cons[names[i]] < 0) return nil;
        if(constraints[names[i]] < 0) {
          if(cons[names[i]] < constraints[names[i]])
            new_acd->set_constraint(names[i], cons[names[i]]);
        }
        else if(constraints[names[i]] > 0) {
          if(cons[names[i]] > constraints[names[i]])
            new_acd->set_constraint(names[i], cons[names[i]]);
        }
        else {
          new_acd->set_constraint(names[i], cons[names[i]]);
        }
      }
      else {
        new_acd->set_constraint(names[i], constraints[names[i]]);
      }
    }
    else {
      new_acd->set_constraint(names[i], cons[names[i]]);
    }
  }
  return new_acd;
}

object ACTION_CONSTRAINT_DATA merge_constraints(object other) {
  mapping cons;
  object ACTION_CONSTRAINT_DATA new_acd;
  string *names;
  int i, n;

  cons = other -> get_constraints();
  new_acd = clone_object(this_object());
  names = map_indices(cons) | map_indices(constraints);
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(cons[names[i]]) new_acd -> set_constraint(names[i], cons[names[i]]);
    else new_acd -> set_constraint(names[i], constraints[names[i]]);
  }
  return new_acd;
}
