# include <limits.h>
# include <type.h>

/*
 * handles all of the skill information for this object.
 */

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

  ur = this_object() -> get_ur_object();

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

  ur = this_object() -> get_ur_object();

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

void evt_train_skill(string skill, int pro, int con, varargs int a...) {
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
