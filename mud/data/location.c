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
    detail_path = ({ "detail" }) + explode(implode(explode(dp, ":"), ":detail:"), ":");
    
}
/* BUG: this will return the wrong information if a detail in the path is
 *      using the id "details". For now, don't do that!
 */
string get_detail_path() {
  if(!detail_path) return "default";
  return implode(detail_path - ({ "detail" }), ":");
}

int is_detail() { return detail_path ? TRUE : FALSE; }

mixed get_property(string *path) {
  if(detail_path) {
    if(sizeof(path) == 2 && path[0] == "base" && path[1] == "environment")
      return target;
    else return target -> get_property(detail_path + path);
  }
  else return target -> get_property(({ "detail", "default" }) + path);
}

string get_description(string desc_path) {
  if(detail_path)
    return target -> get_property(detail_path + ({ "description", desc_path }));
  else
    return target -> get_property(({ "detail", "default", "description", desc_path }));
}

int get_relation() { return proximity; }
void set_relation(int p) { proximity = p; }

int *get_coords() { 
  if(target -> is_surface()) return ({})+coords;
  return nil;
}

int get_milestone() {
  if(target -> is_path()) return milestone;
  return 0;
}