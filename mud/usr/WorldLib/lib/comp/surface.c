/*
 * Holds information about large-scale surface characteristics, though this
 * can also be useful for smaller surfaces like tables.
 */

int *extents;

static void create(varargs int clone) {
  if(clone) {
  }
}

int is_surface() {
  return extents && sizeof(extents);
}


