# include <type.h>
# include <kernel/kernel.h>
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
    src += "object _this, _actor, *_direct, *_indirect, *_instrument;\n";
    src += "if(_args[\"this\"]) _this = _args[\"this\"];\n";
    src += "_direct = _args[\"direct\"] ? _args[\"direct\"] : ({ });\n";
    src += "_indirect = _args[\"indirect\"] ? _args[\"indirect\"] : ({ });\n";
    src += "_instrument = _args[\"instrument\"] ? _args[\"instrument\"] : ({ });\n";
    src += compiler::compile_parse(nil, res);
    src += "}\n";
  }

  return src;
}

atomic int set_event_handlers(mapping code) {
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
  if(err) error("Unable to install event handlers: " + err + "\n" + new_code + "\n\n");
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

/*
 * We want to assemble all of the sensation sets and then
 * send off to the WorldLib event handler
 */
string compile_sensation_chain(object ctx, mixed *args) {
  int i, n;
  string ret;
  string *bits;

  ret = "_C_sensation_chain(";
  ret += "_this,";
  bits = allocate(n = sizeof(args[0]));
  for(i = 0; i < n; i++) {
    bits[i] = compile_parse(ctx, args[0][i]);
    if(bits[i][strlen(bits[i])-1] == ';')
      bits[i] = bits[i][0..strlen(bits[i])-2];
    if(strlen(bits[i]) > 17 && bits[i][0..16] == "_C_sensation_set("/*)*/)
      bits[i] = "_C_build_sensation_set("+bits[i][17..];
  }
  bits -= ({ "", nil });
  ret += "({"+implode(bits, ",") + "})";
  ret += ");";
  return ret;
}

string compile_sensation_set(object ctx, mixed *args) {
  int i, n;
  string ret;
  string *bits;

  ret = "_C_sensation_set(";
  ret += "_this,";

  bits = allocate(n = sizeof(args[0]));
  for(i = 0; i < n; i++) {
    bits[i] = compile_parse(ctx, args[0][i]);
    if(bits[i][strlen(bits[i])-1] == ';')
      bits[i] = bits[i][0..strlen(bits[i])-2];
    if(strlen(bits[i]) > 13 && bits[i][0..12] == "_C_sensation("/*)*/) 
      bits[i] = "_C_build_sensation("+bits[i][13..];
  }
  bits -= ({ "", nil });
  ret += "({" + implode(bits, ",") + "})";
  if(sizeof(args) > 1) {
    ret += "," + compile_parse(ctx, args[1]);
  }
  ret += ");";
  return ret;
}

string compile_acute_sensation(object ctx, mixed *args) {
  string ret;
  ret = "_C_sensation(";
  ret += "_this,";
  ret += "\"" + args[0] + "\""; /* sense */
    /* message text */
  ret += ",({nil,({" + 
     "\""+args[1][0] + "\"" + /* intensity (env, etc.) */
     ","+args[1][1] + /* intensity mod */
     ","+ compile_parse(ctx, args[1][2]) + "}),nil})";
  ret += ",_args);";
  return ret;
}

string compile_chronic_sensation(object ctx, mixed *args) {
  string ret;
  int i, n;
  mapping parts;

  parts = ([ ]);
  for(i = 0, n = sizeof(args[1]); i < n; i++)
    parts[args[1][i][0]] = args[1][i][1];

  /* If we have a chronic sensation that has only a start, then convert to
   * an acute sensation
   */
  ret = "_C_sensation(";
  ret += "_this,";
  ret += "\"" + args[0] + "\""; /* sense */
  ret += ",({";
  if(parts["start"]) {
    ret += "({";
    ret += "\""+parts["start"][0] + "\""; /* intensity (env, etc.) */
    ret += ","+parts["start"][1]; /* intensity mod */
    ret += "," + compile_parse(ctx, parts["start"][2]); /* message text */
    ret += "})";
  }
  else {
    ret += "nil";
  }
  if(parts["narrative"]) {
    ret += ",({";
    ret += "\""+parts["narrative"][0] + "\""; /* intensity (env, etc.) */
    ret += ","+parts["narrative"][1]; /* intensity mod */
    ret += "," + compile_parse(ctx, parts["narrative"][2]); /* message text */
    ret += "})";
  }
  else {
    ret += ",nil";
  }
  if(parts["end"]) {
    ret += ",({";
    ret += "\""+parts["end"][0] + "\""; /* intensity (env, etc.) */
    ret += ","+parts["end"][1]; /* intensity mod */
    ret += "," + compile_parse(ctx, parts["end"][2]); /* message text */
    ret += "})";
  }
  else {
    ret += ",nil";
  }
  ret += "})";
  if(sizeof(args) > 2) {
    ret += "," + compile_parse(ctx, args[1]);
  }
  ret += ");";
  return ret;
}

string compile_function(object ctx, mixed *args) {
  string real_name;
  string *fargs;
  int i, n;
  int vc;

  switch(args[0]) {
    case "Stop": real_name = "_C_remove_timer"; break;
    case "Every": real_name = "_C_create_timer"; break;
    case "Once": real_name = "_C_create_delay"; break;
    case "Emit": real_name = "_C_queue_message"; break;
    case "Get": real_name = "_C_get_property"; break;
    case "Set": real_name = "_C_set_property"; break;
  }

  if(real_name) {
    fargs = allocate(n = sizeof(args[1]));
    vc = ctx -> void_context();
    ctx -> reset_void_context();
    for(i = 0; i < n; i++) fargs[i] = compile_parse(ctx, args[1][i]);
    if(vc) ctx -> set_void_context();
    return "this_object()->" + real_name + "(_this" + (n > 0 ? ",":"") + implode(fargs, ",") + ")"
         + (vc ? ";" : "");
  }
  else error("Unknown function: " + args[0]);
}
