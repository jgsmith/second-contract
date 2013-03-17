# include <kernel/kernel.h>
# include <gamelib.h>
# include <devlib.h>
# include <system.h>

/* All compiled event handlers will be under here somewhere */

/* this is a 32-bit signed integer, so we can only handle 2147483647
 * handler compilations before we overflow. Hopefully DGD will go 64-bit
 * by then.
 */
int handler_count;

/*
 * This daemon handles storing/compiling/cleaning up event handler code.
 *
 * Event code is stored under /usr/Game/events/. All filenames are created
 * here, so there shouldn't be any way for files to get modified outside of
 * /usr/Game/events/. Just in case, though, the Kernel lib keeps us from
 * modifying anything outside /usr/Game/.
 */

void create(varargs int clone) {
  handler_count = 0;
}

/*
 * takes a mapping of event names to LPC code and writes them to a single
 * file for compilation. It returns the name of the object that can be used
 * as a daemon to run the event handlers. This object will already be compiled
 * and ready to be used. Nothing on the file system is touched.
 */
string compile_events(string events) {
  string fname;
  int length;
  int i, n;
  string tmp;
  string content;
  string err;
  object ob;

  /* only the /usr/DevLib/sys/event_script.c daemon can call this */
  if(object_name(previous_object()) != EVENT_SCRIPT_D) {
    error("Only the event script handler may call compile_events()");
    return nil;
  }

  content = 
    "inherit \"" + THING_EVENT_HANDLER_INHERIT + "\";\n" +
    "void create(varargs int clone){}\n" +
    events;

  fname = COMPILE_PATH + "handlers_" + handler_count;
  handler_count++;
  (users())[0] -> message("Compiling " + fname + ": [" + content + "]");
  err = catch( ob = compile_object(fname, content) );
  if(err) error("Unable to compile handlers: " + err);
  return fname;
}

/*
 * We need a way to track when we can cleanup stale daemons. We don't overwrite
 * old handler collections, but create a new file instead. I expect that we'll
 * have one file per ur-object, and one ur-object per file, but I'm not ready
 * to commit to that yet.
 */

void destruct_handler(string daemon) {

  /* only /usr/DevLib/sys/event_script.c can call this */
  if(object_name(previous_object()) != EVENT_SCRIPT_D)
    error("Only the event script handler may call destruct_handler()");
  if(!daemon) return;
  if(strlen(daemon) <= strlen(COMPILE_PATH) || daemon[0..strlen(COMPILE_PATH)-1] != COMPILE_PATH)
    error("The provided path to destruct_handler (" + daemon + ") does not begin with " + COMPILE_PATH);

  destruct_object(daemon);
}
