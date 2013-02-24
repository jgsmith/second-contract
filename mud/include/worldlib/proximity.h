# define PROX_INSIDE  0
# define PROX_CLOSE   1
# define PROX_AGAINST 2
# define PROX_UNDER   3
# define PROX_ON      4
# define PROX_NEAR    5
# define PROX_OVER    6
# define PROX_BEHIND  7
# define PROX_BEFORE  8
# define PROX_BESIDE  9
# define PROX_CONTAINS 10
# define PROX_GUARD   11
# define PROX_BLOCK   12
# define PROX_MAX 12

# define DISTANCE_IN 0
# define DISTANCE_ON 1
# define DISTANCE_CLOSE 2
# define DISTANCE_NEAR 3
# define DISTANCE_VICINITY 4
# define DISTANCE_FAR 5
# define DISTANCE_BACKDROP 6
# define DISTANCE_MAX 6

/*

We want direction for things that are vicinity or farther, in general. This
helps us have things like a "north wall" and a "south wall" that are two
different items, but are both walls in the scene. But this lets us say that
we enter a room from the south, and a table in the middle of the room will
be between us and a door to the north.

This does mean that "behind" and "before" are relative to the direction
we're looking - that is, along the line made by the two points of the
observer and the observed.

We might be getting too caught up in the physical locations instead of the
narrative description. Directionality is only needed when describing things
outside the current scene.

Might be best to keep directionality out of the current scene, but allow it
between scenes - that is, when we look at where things are outside this scene,
we use directionality, or we have to provide coordinates for scene loci and
use those to provide relative locations and line-of-sight narrative.

 */

# define DIR_CENTER 0
# define DIR_NORTH 1
# define DIR_SOUTH 2
# define DIR_EAST 4
# define DIR_WEST 8
# define DIR_UP 16
# define DIR_DOWN 32
# define DIR_NORTHEAST (DIR_NORTH|DIR_EAST)
# define DIR_NORTHWEST (DIR_NORTH|DIR_WEST)
# define DIR_SOUTHEAST (DIR_SOUTH|DIR_EAST)
# define DIR_SOUTHWEST (DIR_SOUTH|DIR_WEST)

/*
  bar on foo
  foo in baz

  moving bar to boo

code "/usr/WorldLib/sys/placement"->add_relation("foo", "bar", 3, 4);
code "/usr/WorldLib/sys/placement"->add_relation("baz", "foo", 2, 0);
code "/usr/WorldLib/sys/placement"->move_object("bar", "boo", 0);
  */

