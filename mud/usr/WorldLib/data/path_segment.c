# include <type.h>

/*
 * Data for a path segment. They always start at (0,0,0).
 *
 * We really should provide a graphical way of editing paths and
 * terrains and assigning landmarks/scenes to milestones and coords.
 */

/*
 * We support lines and quadratic bezier curves
 */

/* 
 * We update the length when we update the control points
 */
int **control_points;
int length;

static void create(varargs int clone) {
  if(clone) {
    control_points = ({ });
    length = 0;
  }
}

int get_length() {
  return length;
}


static int linear_length() {
  float l;

  l = pow((float)(control_points[1][0] - control_points[0][0]), 2.0);
  l += pow((float)(control_points[1][1] - control_points[0][1]), 2.0);
  l += pow((float)(control_points[1][2] - control_points[0][2]), 2.0);
  return (int)(sqrt(l));
}

static int *linear_curve(int t) {
  if(!length) return ({ 0, 0, 0 });
  /* "Linear curves" have constant velocity in t */
  return ({
    control_points[0][0] + t * (control_points[1][0] - control_points[0][0]) / length,
    control_points[0][1] + t * (control_points[1][1] - control_points[0][1]) / length,
    control_points[0][2] + t * (control_points[1][2] - control_points[0][2]) / length
  });
}

static float arsinh(float x) {
  return log(x+sqrt(x*x+1.0));
}

static int quadratic_length(varargs float u) {
  int a, b, c;
  int dx0, dy0, dz0;
  int dx1, dy1, dz1;

  if(typeof(u) == T_NIL) u = 1.0;

  dx0 = control_points[1][0] - control_points[0][0];
  dx1 = control_points[2][0] - control_points[1][0];
  dy0 = control_points[1][1] - control_points[0][1];
  dy1 = control_points[2][1] - control_points[1][1];
  dz0 = control_points[1][2] - control_points[0][2];
  dz1 = control_points[2][2] - control_points[1][2];

  c = dx0*dx0+dy0*dy0;
  b = dx0*(dx1-dx0) + dy0*(dy1-dy0);
  a = (dx1-dx0)*(dx1-dx0) + (dy1-dy0)*(dy1-dy0);

  return (int)(
    (
      (u*(float)a+(float)b)
      *
      sqrt((float)c+2.0*u*(float)b+u*u*(float)a) 
      + 
      ((float)(a*c-b*b))
      *
      arsinh((u*(float)a+(float)b)/sqrt((float)(a*c-b*b)))
      /
      sqrt((float)a)
    )
    /
    (float)a
  );
}

static int *quadratic_curve(int dist) {
  float t, min_t, max_t;
  int guess;
  if(!length) return ({ 0, 0, 0 });

  /* we do a binary search along the curve until we find a t that gives us
   * length dist -- then return the point for that t
   */
  t = (float)dist / (float)length;
  min_t = 0.0;
  max_t = 1.0;
  guess = quadratic_length(t);
  while(guess != dist) {
    if(dist < guess) {
      max_t = t;
      t = (t + min_t)/2.0;
    }
    else {
      min_t = t;
      t = (t + max_t)/2.0;
    }
  }
  return ({
    (int)((1.0-t)*(1.0-t)*(float)control_points[0][0] +
          2.0*(1.0-t)*t*(float)control_points[1][0] +
          t*t*(float)control_points[2][0]),
    (int)((1.0-t)*(1.0-t)*(float)control_points[0][1] +
          2.0*(1.0-t)*t*(float)control_points[1][1] +
          t*t*(float)control_points[2][1]),
    (int)((1.0-t)*(1.0-t)*(float)control_points[0][2] +
          2.0*(1.0-t)*t*(float)control_points[1][2] +
          t*t*(float)control_points[2][2])
  });
}

int *get_coords(int t) {
  switch(sizeof(control_points)) {
    case 2: return linear_curve(t);
    case 3: return quadratic_curve(t);
    default: return ({ 0, 0, 0 });
  }
}

void set_control_point(int c, int x, int y, int z) {
  if(c < 0 || c > sizeof(control_points)) return;

  if(c == sizeof(control_points)) control_points += ({ ({ x, y, z }) });
  else control_points[c] = ({ x, y, z });
  switch(sizeof(control_points)) {
    case 1: length = 0; break;
    case 2: length = linear_length(); break;
    case 3: length = quadratic_length(); break;
  }
}
