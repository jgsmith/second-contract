# include <worldlib.h>
# include <worldlib/tasks.h>
# include <iflib.h>
# include <limits.h>
# include <type.h>

/*
 * handles all of the skill information for this object.
 */

object TASK_RESULT_DATA perform_task(string skill, int difficulty, int tm_type, varargs object ADVERB_DATA adverb);
static void attempt_task(int con, int pro, int upper, mixed extra, object TASK_RESULT_DATA result);
static void attempt_task_e(int con, int pro, int upper, int half, object TASK_RESULT_DATA result);

mapping skills;
mapping skill_bonuses;

static void create(varargs int clone) {
  skills = ([ ]);
  skill_bonuses = ([ ]);
}

mixed get_property(string *path) {
  object ur;
  int lvl, cls, x;
  string *idx;
  int i, n;

  ur = this_object() -> get_template_object();

  if(!sizeof(path)) {
    idx =  map_indices(skills) | (ur ? ur -> get_property(({ "skill" })) : ({ }));
    for(i = 0, n = sizeof(idx); i < n; i++)
      if(skills[idx[i]] == "*deleted*") idx[i] = nil;
    return idx - ({ nil });
  }
  if(sizeof(path) == 1) {

    if(skills[path[0]]) {
      if(skills[path[0]] == "*deleted*") return nil;
      if(skill_bonuses[path[0]]) return skills[path[0]]["level"] + skill_bonuses[path[0]];
      return skills[path[0]]["level"];
    }
    if(ur) return ur->get_property(({"skill", path[0], "level"})) + skill_bonuses[path[0]];
    return nil;
  }
  if(sizeof(path) == 2) {
    if(skills[path[0]] == "*deleted*") return nil;

    switch(path[1]) {
      case "class": 
        if(skills[path[0]]) return skills[path[0]]["class"];
        if(ur) return ur->get_property(({"skill"}) + path);
        return nil;
      case "level":
        if(skills[path[0]]) return skills[path[0]]["level"];
        if(ur) return ur->get_property(({"skill"}) + path);
        return nil;
      case "points":
        if(skills[path[0]]) return skills[path[0]]["points"];
        return 0;
      case "bonus":
        return skill_bonuses[path[0]];
      case "max-level":
        if(skills[path[0]]) cls = skills[path[0]]["class"];
        else if(ur) cls = ur -> get_property(({ "skill", path[0], "class" }));
        else cls = 5;
        lvl = this_object() -> get_property(({ "basic", "level" }));
        cls = 4 - cls;
        if(cls < 0) cls = 0;
        /* without this, we'll overflow for max-points */
        lvl = (lvl+cls)*2;
        if(INT_MAX == (1<<31)-1 && lvl > 22) return 22;
        else if(lvl > 1024) return 1024;
        else return lvl;
      case "max-points":
        cls = -1;
        if(skills[path[0]]) {
          cls = skills[path[0]]["class"];
          lvl = skills[path[0]]["level"];
        }
        else if(ur) {
          cls = ur -> get_property(({ "skill", path[0], "class" }));
          lvl = ur -> get_property(({ "skill", path[0], "level" }));
        }
        if(cls < 0) return 0;
        if(lvl == 0) return 200;
        if(cls == 0) return lvl * 600;
        if(cls > 4) cls = 4;
        /* we can get to around level 22 with this in a 32-bit system. */
        x = lvl;
        while(cls--) x *= lvl;
        return x * 400;
    }
  }
  return nil;
}

int set_property(string *path, mixed value) {
  object ur;
  int tmp;

  if(sizeof(path) == 1) {
    if(!value) skills[path[0]] = "*deleted*";
    return TRUE;
  }
  if(sizeof(path) != 2) return FALSE;

  ur = this_object() -> get_template_object();

  switch(path[1]) {
    case "points":
      if(typeof(value) != T_INT) return FALSE;
      if(!skills[path[0]]) {
        skills[path[0]] = ([
          "points": value,
          "class": (ur ? ur -> get_property(({"skill", path[0], "class"})) || 5 : 5),
          "level": (ur ? ur -> get_property(({"skill", path[0], "level"})) || 0 : 0),
        ]);
      }
      else if(skills[path[0]] == "*deleted*") {
        skills[path[0]] = ([
          "points": value,
          "class": 5,
          "level": 0,
        ]);
      } 
      else {
        skills[path[0]]["points"] = value;
      }
      if(skills[path[0]]["points"] < 0) {
        if(skills[path[0]]["level"] < 0) {
          skills[path[0]]["points"] = 0;
        }
        else {
          this_object() -> set_property(({ "skill", path[0], "level" }), skills[path[0]]["level"] - 1);
        }
      }
      else {
        tmp = this_object() -> get_property(({ "skill", path[0], "max-points" }));
        if(skills[path[0]]["points"] > tmp) {
          this_object() -> set_property(({ "skill", path[0], "level" }), skills[path[0]]["level"] + 1);
          this_object() -> set_property(({ "skill", path[0], "points" }), skills[path[0]]["points"] - tmp);
        }
      }
      return TRUE;
    case "level":
      if(value < 0) return FALSE;
      if(!skills[path[0]]) {
        skills[path[0]] = ([
          "points": 0,
          "class": (ur ? ur -> get_property(({"skill", path[0], "class"})) || 5 : 5),
          "level": value,
        ]);
      }
      else if(skills[path[0]] == "*deleted*") {
        skills[path[0]] = ([
          "points": 0,
          "class": 5,
          "level": value,
        ]);
      } 
      else {
        skills[path[0]]["level"] = value;
      }
      /* N.B.: this in conjunction with the similar code above for points
       * assumes that we're changing one level up or down at a time.
       */
      if(skills[path[0]]["points"] < 0) {
        tmp = this_object() -> get_property(({ "skill", path[0], "max-points" }));
        this_object() -> set_property(({ "skill", path[0], "points" }), skills[path[0]]["points"] + tmp);
      }
      return TRUE;
    case "bonus":
      skill_bonuses[path[0]] = value;
      return TRUE;
    case "class":
      if(value < 0 || value > 5) return FALSE;
      if(!skills[path[0]]) {
        skills[path[0]] = ([
          "points": 0,
          "class": value,
          "level": (ur ? ur -> get_property(({"skill", path[0], "level"})) || 0 : 0),
        ]);
      }
      else if(skills[path[0]] == "*deleted*") {
        skills[path[0]] = ([
          "points": 0,
          "class": value,
          "level": 0,
        ]);
      }
      else {
        skills[path[0]]["class"] = value;
      }
      return TRUE;
    default: return FALSE;
  }
  return FALSE;
}

void train_skill(string skill, int pro, int con, varargs int a...) {
  int lvl, val, success, bonus;

  lvl = get_property(({ skill, "level" }));
  if(sizeof(a)) {
    success = a[0];
    if(sizeof(a) == 2) {
      bonus = a[1];
    }
    else {
      bonus = 1;
    }
  }
  else {
    success = 1;
    bonus = 1;
  }

  if(con > 100) con = 100;
  if(con <   0) con =   0;
  if(pro > 100) pro = 100;
  if(pro <   0) pro =   0;

  val = (con - pro + (200 * success) + 100)/8;
  this_object() -> set_property(({ "skill", skill, "points" }),
    get_property(({ skill, "points" })) + 
    (bonus * val * lvl * lvl) + 1
  );
}

static void attempt_task(int con, int pro, int upper, mixed extra, object TASK_RESULT_DATA result) {
  int margin, success_margin, deg, res;
  float tmp;

  if(pro < con) {
    result -> set_result(TASK_FAIL);
    result -> set_degree(
      TASKS_D -> is_critical(-100) ? TASK_CRITICAL : TASK_EXCEPTIONAL
    );
    result -> set_raw(-100);
    return;
  }

  switch(typeof(extra)) {
    case T_NIL: margin = (int)(3.0*sqrt((float)con)); break;
    case T_INT: margin = extra; break;
    case T_ARRAY: margin = (int)(extra[0] + extra[1]*sqrt((float)con)); break;
    default: margin = 0; break;
  }

  if(pro > con + margin) {
    result -> set_result(TASK_SUCCESS);
    result -> set_degree(
      TASKS_D -> is_critical(100) ? TASK_CRITICAL : TASK_EXCEPTIONAL
    );
    result -> set_raw(100);
    return;
  }

  if(!margin) margin = 1;
  success_margin = (( 100 * (pro-con) )/margin) - random(100);
  result -> set_raw(success_margin);
  if(success_margin <= 0) {
    result -> set_result(TASK_FAIL);
    if(TASKS_D -> is_critical(success_margin)) {
      deg = TASK_CRITICAL;
    }
    else {
      if(-success_margin < TASK_MARGINAL_UPPER) {
        deg = TASK_MARGINAL;
      }
      else if(-success_margin < TASK_NORMAL_UPPER) {
        deg = TASK_NORMAL;
      }
      else {
        deg = TASK_EXCEPTIONAL;
      }
    }

    result -> set_degree(deg);
    return;
  }

  result -> set_result(TASK_SUCCESS);
  if(TASKS_D -> is_critical(success_margin)) {
    deg = TASK_CRITICAL;
  }
  else {
    if(success_margin < TASK_MARGINAL_UPPER) {
      deg = TASK_MARGINAL;
    }
    else if(success_margin < TASK_NORMAL_UPPER) {
      deg = TASK_NORMAL;
    }
    else {
      deg = TASK_EXCEPTIONAL;
    }
  }
  result -> set_degree(deg);
}

static void attempt_task_e(int con, int pro, int upper, int half, object TASK_RESULT_DATA result) {
  float fail_chance;
  float tmp;
  int success_margin, deg, res;

  if(pro < con) {
    result->set_result(TASK_FAIL);
    result -> set_degree(TASKS_D -> is_critical(-100) ? TASK_CRITICAL : TASK_EXCEPTIONAL);
    result -> set_raw(-100);
    return;
  }

  if(!half) half = (int)(6.0*sqrt((float)con));
  if(!half) half = 1;
  fail_chance = exp((-0.693*(float)(pro-con))/(float)half);
  success_margin = (random(1000) - (int)(1000.0*fail_chance))/10;
  result -> set_raw(success_margin);
  if(success_margin < 0) {
    result->set_result(TASK_FAIL);
    if(-success_margin < TASK_MARGINAL_UPPER) {
      deg = TASK_MARGINAL;
    }
    else if(-success_margin < TASK_NORMAL_UPPER) {
      deg = TASK_NORMAL;
    }
    else {
      deg = TASK_EXCEPTIONAL;
    }
    result -> set_degree(deg);
    return;
  }

  result->set_result(TASK_SUCCESS);
  if(TASKS_D -> is_critical(success_margin)) {
    deg = TASK_CRITICAL;
  }
  else {
    if(success_margin < TASK_MARGINAL_UPPER) {
      deg = TASK_MARGINAL;
    }
    else if(success_margin < TASK_NORMAL_UPPER) {
      deg = TASK_NORMAL;
    }
    else {
      deg = TASK_EXCEPTIONAL;
    }
  }
  result -> set_degree(deg);
}

object TASK_RESULT_DATA perform_task(string skill, int difficulty, int tm_type, varargs object ADVERB_DATA adverb) {
  /*
   * adverb can apply constraints: time and skill
   *
   *      time
   *        |
   *     A  |  B     s
   *        |        k
   *  ------+------  i
   *        |        l
   *     C  |  D     l
   *        |
   *
   * A: dec time/inc skill  -- mid chance of success -requires focus
   * B: inc time/inc skill  -- highest chance of success
   * C: dec time/dec skill  -- lowest chance of success
   * D: inc time/dec skill  -- mid chance of success - no focus required
   */
  object TASK_RESULT_DATA result;
  int c_time, c_skill;
  int pro, con, bonus, success;
  int trainable;

  result = new_object(TASK_RESULT_DATA);

  if(!skill) return result;

  if(adverb) {
    c_time = adverb->get_constraint("time");
    c_skill = adverb->get_constraint("skill");
  }

  con = difficulty;
  pro = get_property(({ skill }));
  bonus = 1;

  if(c_time < 0) {
    if(c_skill < 0) { /* C */
    }
    else { /* A */
      result -> set_focus_required(1); /* raise requirements for outside events getting noticed */
    }
  }
  else {
    if(c_skill < 0) { /* D */
    }
    else { /* B */
      bonus = 2;
      if(c_time > bonus) bonus = c_time;
      if(c_skill > bonus) bonus = c_skill;
    }
  }

  if(!tm_type) tm_type = SKILLS_D -> get_task_type(skill);
 
  switch(tm_type) {
    case TASK_FIXED: 
      attempt_task(con, pro, 100, 0, result); trainable = 1; break;
    case TASK_FREE: 
      attempt_task(con, pro, 25, 0, result); trainable = 1; break;
    case TASK_CONTINUOUS: 
      attempt_task(con, pro, 50, 0, result); break;
    case TASK_COMMAND: 
      attempt_task(con, pro, 100, 0, result); trainable = 1; break;
    case TASK_RITUAL: 
      attempt_task_e(con, pro, 50, 25, result); trainable = 1; break;
    case TASK_SPELL: 
      attempt_task_e(con, pro, 60, 40, result); trainable = 1; break;
    default: 
      attempt_task_e(con, pro, 1, 0, result); trainable = 0; break;
  }

  if(c_time < 0 && c_skill < 0) trainable = 0;

  /* we double the training available if we are exceptional or better in our
   * task results
   */
  if(trainable) {
    if(result->get_result() && result -> get_degree() >= TASK_EXCEPTIONAL) 
      bonus *= 2;
    train_skill(skill, pro, con, result->get_result(), bonus);
  }

  return result;
}

object TASK_RESULT_DATA compare_skills(string myskill, object other, string otherskill, int modifier, int my_tm_type, int other_tm_type, varargs object ADVERB_DATA my_adv, object ADVERB_DATA other_adv) {
  int offskill, defskill;
  int perc, chance;
  int success_margin, res, deg;
  object TASK_RESULT_DATA result;

  result = new_object(TASK_RESULT_DATA);
  result -> set_result(TASK_FAIL);

  if(!other || !myskill || !otherskill) return result;

  offskill = get_property(({ myskill }));
  defskill = other->get_property(({ "skill", otherskill }));
  if(!defskill) defskill = 1;
  if(!offskill) offskill = 1;

  if(offskill > defskill)
    perc = (50 * offskill * offskill) / (offskill * defskill);
  else
    perc = 100 - (50 * defskill*defskill)/(offskill*defskill);

  if(perc > 99) perc = 99;
  if(perc < 1) perc = 1;

  chance = random(100);
  success_margin = perc-chance;
  if(success_margin > 0) {
    result = perform_task( myskill, defskill-modifier, my_tm_type, my_adv );
    result -> set_winner(1);
  }
  else {
    result = other->perform_task(otherskill, offskill-modifier, other_tm_type, other_adv);
    result -> set_winner(0);
  }
  return result;
}
