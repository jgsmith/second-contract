/*
 * Data for a path segment. They always start at (0,0,0).
 *
 * We really should provide a graphical way of editing paths and
 * terrains and assigning landmarks/scenes to milestones and coords.
 */

/*
 * We support linear, quadratic, and cubic bezier curves
 */

int length;
int **control_points;

static void create(varargs int clone) {
  if(clone) {
  }
}

int get_length() {
  return length;
}

static int linear_curve(int o, int c, int t) {
  if(!length) return 0;

  return control_points[o][c] + t * (control_points[o+1][c] - control_points[o][c]) / length;
}

static int quadratic_curve(int o, int c, int t) {
  if(!length) return 0;

  return ((length - t) * (linear_curve(o, c, t)) + t * (linear_curve(o+1, c, t))) / length;

static int cubic_curve(int o, int c, int t) {
  if(!length) return 0;

  return ((length - t) * (quadratic_curve(o, c, t)) + t * (quadratic_curve(o+1, c, t))) / length;
}

int *get_coords(int t) {
  switch(sizeof(control_points)) {
    case 2:
      return ({ 
        linear_curve(0, 0, t), 
        linear_curve(0, 1, t), 
        linear_curve(0, 2, t) 
      });
    case 3:
      return ({ 
        quadratic_curve(0, 0, t), 
        quadratic_curve(0, 1, t), 
        quadratic_curve(0, 2, t) 
      });
    case 4:
      return ({ 
        cubic_curve(0, 0, t), 
        cubic_curve(0, 1, t), 
        cubic_curve(0, 2, t) 
      });
    default: return ({ 0, 0, 0 });
  }
}
