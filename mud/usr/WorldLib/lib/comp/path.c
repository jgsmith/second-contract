/*
 * Path-related aspects of this object
 */

/*
 * A path is a series of parameterized segments that map the path onto
 * an underlying surface.
 */

int *anchor;
object PATH_SEGMENT_DATA *segments;

object parent_surface;

static void create(varargs int clone) {
  if(clone) {
    anchor = ({ 0, 0, 0 });
    segments = ({ });
  }
}

int is_path() {
  return segments && sizeof(segments);
}

/* Given our present location on the path, what's the surface coordinate? */
int *get_coords(int t) {
  int i, n, tmp;
  int *coords, *start;

  start = anchor + ({ });
  for(i = 0, n = sizeof(segments); i < n && t >= 0; i++) {
    tmp = segments[i]->get_length();
    if(t < tmp) tmp = t;
    coords = segments[i] -> get_coords(tmp);
    t -= tmp;
    start[0] += coords[0];
    start[1] += coords[1];
    start[2] += coords[2];
  }

  return start;
}


