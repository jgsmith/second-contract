# include <worldlib.h>
# include <kernel/kernel.h>

object template_object;
string template_path;
string my_template_path;

object *children;

static void create(varargs int clone) {
  if(clone) {
  }
}

void destructed(int clone) {
  int i, n;
  if(SYSTEM() && clone) {
    if(children) {
      for(i = 0, n = sizeof(children); i < n; i++)
        destruct_object(children[i]);
    }
  }
}

atomic object create_clone() {
  object ob;

  if(!my_template_path) return nil;
  children -= ({ nil });

  ob = clone_object(THING_OBJ);
  ob -> set_template_object();
  children += ({ ob });
  return ob;
}

/* this can only be called from this library */
void remove_child(object ob) {
  if(previous_program() == "/usr/WorldLib/lib/comp/template") {
    children -= ({ ob });
  }
  children -= ({ nil });
}

void add_child(object ob) {
  children -= ({ nil });
  if(previous_program() == "/usr/WorldLib/lib/comp/template") {
    children |= ({ ob });
  }
}

atomic void set_template_path(string path) {
  object ob;
  /* We can modify if we're the REST api */
  if(my_template_path && !SYSTEM()) {
    error("Only System may modify the template of a template.\n");
    return;
  }
  if(my_template_path && template_path) {
    /* TODO: change this -- we should be able to change the template */
    error("The template of a template can not be changed once it is assigned.\n");
    return;
  }
  /* get the domain/ward/item */
  if(!path) path = previous_object() -> get_template_path();
  if(!path) return;
  ob = DOMAINS_D -> get_template_object(path);
  if(ob) {
    if(template_object) {
      if(template_path == path) return;
      template_object -> remove_child(this_object());
      template_object = nil;
    }
    template_path = path;
    template_object = ob;
    template_object -> add_child(this_object());
  }
}

object get_template_object() { return template_object; }

int set_my_template_path(string x) {
  if(my_template_path) {
    if(!SYSTEM()) {
      error("Only System may modify the template path property of a template.\n");
    }
    else {
      my_template_path = x;
    }
  }
  else {
    my_template_path = x;
  }
  if(my_template_path) {
    if(!children) children = ({ });
  }
  return my_template_path != nil;
}

/*
 * Valid objects are using ur objects that are marked as ur objects in the
 * system. Valid objects are not themselves ur objects.
 *
 * Only valid objects can appear to players in the "real" game. Otherwise,
 * these objects can only be used in test areas by creators.
 *
 * Invalid objects are eliminated from direct/indirect object lists. They
 * are essentially invisible to non-creators and don't receive any events
 * from actions initated by players.
 */
int is_template_object() { return my_template_path != nil; }
