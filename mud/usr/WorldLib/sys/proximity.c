# include <worldlib/proximity.h>
# include <iflib.h>
# include <data.h>

/*
 * Handles proximity-based calculations
 */

int *opposite_relations;
int *relation_addition;

void create(varargs int clone) {
  /* We should read these from data files */
  opposite_relations = ({
    PROX_CONTAINS,
    PROX_CLOSE,
    PROX_AGAINST,
    PROX_OVER,
    PROX_UNDER,
    PROX_NEAR,
    PROX_UNDER,
    PROX_BEFORE,
    PROX_BEHIND,
    PROX_BESIDE,
    PROX_INSIDE,
  });
  /* Used when I want to know how close we are to C 
   * when we know A->B, and B->C 
   * If A is ___ B, and B is ___ C, then A is ___ C.
   */
  relation_addition = ({
  /* PROX_INSIDE - like adding 0 */
    PROX_INSIDE, PROX_CLOSE, PROX_AGAINST, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_BEHIND, PROX_BEFORE, PROX_BESIDE, PROX_CLOSE,
  /* PROX_CLOSE - like adding 1 */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_NEAR,
    PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE,
  /* PROX_AGAINST */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_NEAR,
    PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE,
  /* PROX_UNDER */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_BEHIND, PROX_BEFORE, PROX_BESIDE, PROX_CLOSE,
  /* PROX_ON */
    PROX_INSIDE, PROX_CLOSE, PROX_CLOSE, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_BEHIND, PROX_BEFORE, PROX_BESIDE, PROX_CLOSE,
  /* PROX_NEAR */
    PROX_INSIDE, PROX_NEAR, PROX_NEAR, PROX_NEAR, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_NEAR, PROX_NEAR, PROX_NEAR, PROX_NEAR,
  /* PROX_OVER */
    PROX_INSIDE, PROX_CLOSE, PROX_CLOSE, PROX_UNDER, PROX_OVER, PROX_OVER,
    PROX_OVER, PROX_NEAR, PROX_NEAR, PROX_NEAR, PROX_OVER,
  /* PROX_BEHIND */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_CLOSE, PROX_BEFORE, PROX_CLOSE, PROX_CLOSE,
  /* PROX_BEFORE */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_BEHIND, PROX_CLOSE, PROX_CLOSE, PROX_CLOSE,
  /* PROX_BESIDE */
    PROX_INSIDE, PROX_NEAR, PROX_CLOSE, PROX_UNDER, PROX_ON, PROX_NEAR,
    PROX_OVER, PROX_CLOSE, PROX_CLOSE, PROX_NEAR, PROX_CLOSE,
  /* PROX_CONTAINS */
    PROX_CLOSE, PROX_CONTAINS, PROX_CONTAINS, PROX_CONTAINS, PROX_CONTAINS,
                                                             PROX_CONTAINS, 
    PROX_CONTAINS, PROX_CONTAINS, PROX_CONTAINS, PROX_CONTAINS, PROX_CONTAINS,
  });
}

int get_opposite_proximity(int p) {
  if(p < 0 || p > PROX_MAX) return -1;
  return opposite_relations[p];
}

/*
 * Given A p B, and B q C, return A _ C
 */
static int combine_proximities(int p, int q) {
  if(p == PROX_GUARD || p == PROX_BLOCK) return p;
  if(q == PROX_GUARD || q == PROX_BLOCK) return PROX_NEAR;

  if(p < 0 || q < 0 || p > PROX_MAX || q > PROX_MAX) return -1;

  return relation_addition[p*PROX_MAX + q];
}

/*
 * Given the relation (p) between the caller and the detail, what's
 * the caller's relation with the thing?
 *
 * A detail path is ({ a, b, c }) such that the property root for
 * each detail is "detail:$a:detail:$b:detail:$c" ...
 */
int get_composite_proximity(int p, object thing, string *detail_path) {
  string *prop;
  int q;

  prop = ({ "detail" }) + explode(implode(detail_path, ":detail:"), ":");
  while(sizeof(prop) > 1) {
    q = WORDS_D -> preposition_number(thing -> get_property(prop + ({ "preposition" })));
    p = combine_proximities(p, q);
    prop = prop[0..sizeof(prop)-2];
  }
  return p;
}

/* returns the 2*log(dist)/log(2) of the distance. Sets 0 to being
 * "close" (-4 == in, -2 == on)
 * "near" = 1 - each "near" == +1
 *
 * This represents how much distance attenuates sound and such in a scene.
 */
int get_son_distance(object src, object LOCATION_DATA dest) {
  return 0;
}
