object target;
string *detail_path;
int *coords;
int milestone;
int proximity;

void create(varargs int clone) {
  if(clone) {
  }
}

object set_object(object obj) {
  target = obj;
}

object get_object() { return target; }

void set_detail_path(string dp) {
  if(dp == "default")
    detail_path = nil;
  else
    detail_path = ({ "details" }) + explode(implode(explode(dp, ":"), ":details:"), ":");
    
}
/* BUG: this will return the wrong information if a detail in the path is
 *      using the id "details". For now, don't do that!
 */
string get_detail_path() {
  if(!detail_path) return "default";
  return implode(detail_path - ({ "details" }), ":");
}

int is_detail() { return detail_path ? TRUE : FALSE; }

mixed get_property(string *path) {
  if(detail_path) {
    if(sizeof(path) == 2 && path[0] == "base" && path[1] == "environment")
      return target;
    else return target -> get_property(detail_path + path);
  }
  else return target -> get_property(({ "details", "default" }) + path);
}

int get_relation() { return proximity; }
void set_relation(int p) { proximity = p; }
