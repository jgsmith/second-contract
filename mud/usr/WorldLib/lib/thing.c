# include <worldlib.h>
# include <devlib.h>
# include <kernel/kernel.h>

/*
 * This represents anything in the world that can be manipulated or entered.
 * Anything that can be someplace.
 */

/* we'll start out simple, but we can develop this into a set of libraries
 * and LOWs
 */

inherit actions  "/usr/WorldLib/lib/event/actions";
inherit basic    "/usr/WorldLib/lib/comp/basic";
inherit bulk     "/usr/WorldLib/lib/prop/bulk";
inherit counters "/usr/WorldLib/lib/prop/counters";
inherit details  "/usr/WorldLib/lib/comp/details";
inherit genetics "/usr/WorldLib/lib/comp/genetics";
inherit id       "/usr/WorldLib/lib/prop/id";
inherit parser   "/usr/WorldLib/lib/prop/parser";
inherit relations "/usr/WorldLib/lib/comp/relations";
inherit reputations "/usr/WorldLib/lib/prop/reputations";
inherit sensations "/usr/WorldLib/lib/event/sensations";
inherit skills   "/usr/WorldLib/lib/prop/skills";
inherit stats    "/usr/WorldLib/lib/prop/stats";
inherit traits   "/usr/WorldLib/lib/prop/traits";
inherit ur       "/usr/WorldLib/lib/comp/ur";

static void create(varargs int clone) {
  counters::create(clone);
  details::create(clone);
  genetics::create(clone);
  relations::create(clone);
  reputations::create(clone);
  sensations::create(clone);
  skills::create(clone);
  stats::create(clone);
  traits::create(clone);
  id::create(clone);
}

atomic void set_ur_object_path(string path) {
  ur::set_ur_object_path(path);
  bulk::set_ur_object(ur::get_ur_object());
}

string *parse_command_id_list() {
  return (id::parse_command_id_list() 
       | details::parse_command_id_list()) - ({ nil, "" });
}

string *parse_command_plural_id_list() {
  return (id::parse_command_plural_id_list() 
       | details::parse_command_plural_id_list()) - ({ nil, "" });
}

string *parse_command_adjective_id_list() {
  return (id::parse_command_adjective_id_list() 
       | details::parse_command_adjective_id_list()) - ({ nil, "" });
}

string *parse_command_plural_adjective_id_list() {
  return (id::parse_command_plural_adjective_id_list() 
       | details::parse_command_plural_adjective_id_list()) - ({ nil, "" });
}

mixed get_properties() {
  mapping info;

  info = ([
    "basic": basic::get_properties(),
    /* "counter": counters::get_properties(), */
    "detail": details::get_properties(),
    "physical": genetics::get_properties(),
    /* "reputation": reputations::get_properties(),
    "skill": skills::get_properties(),
    "stat": stats::get_properties(),
    "trait": traits::get_properties(),
    "id": id::get_properties(), */
    "events": actions::get_event_handler_source(),
  ]);
  return info;
}

mixed get_property(string *path) {
  if(!sizeof(path)) return nil;

  switch(path[0]) {
    case "basic":    return basic::get_property(path[1..]);
    case "counter":  return counters::get_property(path[1..]);
    case "detail":   return details::get_property(path[1..]);
    case "physical": return genetics::get_property(path[1..]);
    case "reputation": return reputations::get_property(path[1..]);
    case "skill":    return skills::get_property(path[1..]);
    case "stat":     return stats::get_property(path[1..]);
    case "trait":    return traits::get_property(path[1..]);
    case "id":       return id::get_property(path[1..]);
    default: return nil;
  }
}

int set_property(string *path, mixed value) {
  object e;
  int result;

  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "basic":  result = basic::set_property(path[1..], value); break;
    case "counter": result = counters::set_property(path[1..], value); break;
    case "detail": result = details::set_property(path[1..], value); break;
    case "physical": result = genetics::set_property(path[1..], value); break;
    case "reputation": result = reputations::set_property(path[1..], value); break;
    case "skill":  result = skills::set_property(path[1..], value); break;
    case "stat":   result = stats::set_property(path[1..], value); break;
    case "trait":  result = traits::set_property(path[1..], value); break;
    case "id":     result = id::set_property(path[1..], value); break;
    default: result = FALSE;
  }

  if(result) {
    e = EVENTS_D -> create_event(
      "post:prop-change:" + implode(path, ":")
    );
    e -> set_args(([
      "value": value,
    ]));

    EVENTS_D -> queue_event(e);
  }
  return result;
}

int add_event_handlers(mapping code) {
  string po;

  po = object_name(previous_object());
  if(SYSTEM() || po == "/usr/WorldLib/initd" || po == "/usr/WorldLib/data/resource/thing") {
    return EVENT_SCRIPT_D -> set_event_handlers(code);
  }
}
