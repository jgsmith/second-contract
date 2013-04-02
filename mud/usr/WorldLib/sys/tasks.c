/*
 * Various helpers for skill test management
 */

int *critical_chances;

static void create(varargs int clone) {
  float a, b;
  int i;

  critical_chances = allocate(100);
  a = 0.93260;
  b = 0.06978;
  for(i = 0; i < 100; i++) critical_chances[i] = (int)(a*exp(b*((float)i+1.0)));
}

int is_critical(int margin) {
  if(margin < 0) margin *= -1;
  if(margin > 100) margin = 100;
  if(margin == 0) return FALSE;
  return random(10000) < critical_chances[margin-1];
}
