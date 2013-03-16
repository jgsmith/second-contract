/*
 * Based on the Skotos Bulk System
 * http://www.skotos.net/articles/bulksystem.html
 *
 * Material + one of Mass|LDim is required for a valid object
 *
 * Mass: in grams - max mass = 2,147,483 kgs
 * Density: mass/m^3 (* 1000 as an int) (comes from material object)
 * LDim: in (m)meters - max ldim: 2,147 kms
 *
 * Capacity: cubic meters
 * Max Weight: in grams
 * Max Depth: in meters
 * Max Entrance Area: in square meters
 *
 * Minimum Volume: cubic meters
 * Actual Volume: cubic meters
 * Minimum Frontal Area: square meters
 *
 */
object material;
int mass;
int ldim;
int relative_size;
int capacity;

/*
 * carrying_capacity is a percentage of the nominal capacity based on the
 * material, capacity, relative_size, and 
 *
 * max carrying mass = carrying_capacity * material:tensile-strength * mass / ldim^2
 *
 * mass ?= ldim^3 * material:density
 * 
 *
 */

int carrying_capacity;
int max_depth;
int max_entrance_area;

void create(varargs int clone) {
  relative_size = 100;
  carrying_capacity = 1;
  max_depth = 0;
  capacity = -1;
  mass = -1;
  ldim = -1;
  max_entrance_area = -1;
}

object get_material() { 
  object ob;

  if(material) return material;
  if(ob = this_object() -> get_ur_object()) return ob -> get_material();
  return nil;
}
int get_mass() { return mass; }
int get_ldim() { return ldim; }
int get_relative_size() { return relative_size; }
int get_capacity() { return capacity; }
int get_max_depth() { return max_depth; }
int get_max_entrance_area() { return max_entrance_area; }

atomic void set_ur_object(object ob) {
  if(mass < 0 && ob->get_mass() >= 0) mass = ob -> get_mass();
  if(ldim < 0 && ob->get_ldim() >= 0) ldim = ob -> get_ldim();
  if(ob->get_relative_size() >= 0) relative_size = ob -> get_relative_size();
  if(capacity < 0 && ob->get_capacity() >= 0) capacity = ob -> get_capacity();
  if(ob->get_max_depth() >= 0) max_depth = ob -> get_max_depth();
  if(ob->get_max_entrance_area() >= 0) max_entrance_area = ob -> get_max_entrance_area();
}

int calc_mass() {
  object mat;
  if(mass >= 0) return mass;
  mat = get_material();
  if(mat && ldim >= 0) return ldim*ldim*ldim /1000 * mat->get_density() / 1000;
}

int calc_ldim() {
  object mat;
  if(ldim >= 0) return ldim;
  if(mat = get_material()) {
    if(capacity >= 0)
      return ((int)(
        exp(
          log(
            (float)(capacity + 1000000*calc_mass()/mat->get_density())
          )/3.0
        )
      ));
    else
      return ((int)(
        exp(
          log(
            (float)(1000000*calc_mass()/mat->get_density())
          )/3.0
        )
      ));
  }
}

mixed get_property(string *path) {
  object mat;
  int tmp;
  mat = get_material();

  if(path[0] == "material")
    if(mat) return mat -> get_property(path[1..]);
    else return nil;

  if(sizeof(path) > 1) return nil;
  switch(path[0]) {
    case "mass": return calc_mass();
    case "ldim": return calc_ldim();
    case "relative-size": return relative_size;
    case "capacity": 
      if(capacity >= 0) return capacity;
      break;
    case "max-weight": 
      tmp = calc_ldim();
      return carrying_capacity * mat->get_tensile_strength() * calc_mass()/(tmp*tmp);
    case "max-depth":  return max_depth;
    case "max-entrance-area":
      if(max_entrance_area >= 0) return max_entrance_area;
      break;
    case "min-volume": 
      if(mass >= 0) return mass / mat->get_density();
      if(ldim >= 0) return ldim*ldim*ldim;
      return 0;
    case "volume":
      if(capacity >= 0 && mass >= 0) return capacity + mass/mat->get_density();
      if(capacity >= 0) return capacity + ldim*ldim*ldim;
      
      break;
    case "min-frontal-area":
      if(capacity >= 0) return (capacity + mass/mat->get_density())/ldim;
      break;
  }
}

