# include <type.h>
# include <system.h>
# include <devlib.h>
# include <toollib.h>
# include <gamelib.h>

inherit compiler COMPILER_LIB;

/*
 * This daemon manages event scripts - not their parsing, but their storage
 * and execution.
 *
 * Scripts are compiled into /usr/Game/events/... by /usr/Game/sys/events
 *  Just in case someone manages to crack the scripting->LPC coding that
 *  doesn't allow function calls other than those whitelisted.
 */

mapping objects;

void create(varargs int clone) {
  if(clone) return; /* an error - kernel lib shouldn't let us anyway */

  compiler::create(clone);

  objects = ([ ]);
}

void remove_handlers() {
  object ob;
  string key;
  mapping handlers;

  ob = previous_object();

  handlers = MAPPING_D -> specific_mapping(objects, key = object_name(ob));
  if(handlers[key]) handlers[key] = nil;
}

private string event_function_name(string event_name) {
  return "event_" +
    implode(
      explode(
        implode(
          explode(
            event_name,
            "-"
          ),
          "_"
        ),
        ":"
      ),
      "__"
    );
}

string compile_handlers(mapping code) {
  string src;
  string *events;
  int i, n;
  mixed res;

  src = "";

  events = map_indices(code);

  for(i = 0, n = sizeof(events); i < n; i++) {
    res = EVENT_SCRIPT_P -> parse(code[events[i]]);
    if(!res) error("Unable to parse handler for " + events[i]);
    
    src += "atomic mixed " + event_function_name(events[i]) + "(mapping _args) {\n";
    src += compiler::compile_parse(nil, res);
    src += "}\n";
  }

  return src;
}

mixed set_event_handlers(mapping code) {
  object ob;
  string daemon;
  mixed res;
  mapping handlers;
  mapping daemons;
  string key;
  string *events;
  string *prior_events;
  string new_code;
  string err;
  int i, n;

  ob = previous_object();

  new_code = compile_handlers(code);

  /* we have a new set of handlers */
  handlers = MAPPING_D -> specific_mapping(objects, key = object_name(ob));
  /* now we want to build out the LPC daemons that will handle these
   * events */
  if(!handlers[key]) handlers[key] = ([ ]);
  events = map_indices(handlers[key]);
  daemons = ([ ]);
  for(i = 0, n = sizeof(events); i < n; i++) {
    if(daemons[handlers[key][events[i]]["daemon"]])
      daemons[handlers[key][events[i]]["daemon"]] ++;
    else
      daemons[handlers[key][events[i]]["daemon"]] = 1;
  }
  err = catch ( daemon = EVENT_CODE_D -> compile_events(new_code) );
  if(err) error("Unable to install event handlers: " + err);
  events = map_indices(code);
  for(i = 0, n = sizeof(events); i < n; i++) {
    if(handlers[key][events[i]]) {
      daemons[handlers[key][events[i]]["daemon"]]--;
    }
    handlers[key][events[i]] = ([
      "daemon": daemon,
      "method": event_function_name(events[i])
    ]);
  }
  events = map_indices(daemons);
  for(i = 0, n = sizeof(events); i < n; i++) {
    if(daemons[events[i]] <= 0) {
      /* we can remove this daemon */
      EVENT_CODE_D -> destruct_handler(events[i]);
    }
  }
  return TRUE;
}

mixed run_event_handler(string name, mapping args) {
  object ob;
  string key;
  mapping handlers;

  ob = previous_object();
  do {
    handlers = MAPPING_D -> specific_mapping(objects, key = object_name(ob));

    if(handlers[key] && handlers[key][name] && find_object(handlers[key][name]["daemon"])) {
      /* run handler */
      return call_other(handlers[key][name]["daemon"], handlers[key][name]["method"], args);
    }
    ob = ob -> get_ur_object();
  } while(ob);
  return nil; /* no handler available */
} 

/*
 * Now special handlers for event-based stuff
 */

string compile_return(object ctx, mixed *args) {
  ctx -> reset_void_context();
  return "return " + compiler::compile_parse(ctx, args[0]) + ";";
}
