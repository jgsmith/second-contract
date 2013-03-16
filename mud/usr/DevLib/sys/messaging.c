# include <devlib.h>
# include <devlib/template.h>
# include <toollib.h>

/*
 * Handles sending messages to a set of objects.
 */

void create(varargs int clone) {
}

/* Returns a TEMPLATE_DATA object that will handle the actual rendering. */
object interpret_template(mixed *parsed) {
  int i, n;
  object tdata, ob;

  tdata = new_object(TEMPLATE_DATA);

  for(i = 0, n = sizeof(parsed); i < n; i++) {
    if(typeof(parsed[i]) == T_STRING) tdata -> add_bit(parsed[i]);
    else switch(parsed[i][0]) {
      case "substitution":
        ob = new_object(TMPL_POS_DATA);
        ob -> set_name(parsed[i][1]);
        ob -> set_pos(parsed[i][2]);
        tdata -> add_bit(ob);
        break;
      case "verb":
        ob = new_object(TMPL_VERB_DATA);
        ob -> set_name(parsed[i][1]);
        ob -> set_verb(parsed[i][2]);
        tdata -> add_bit(ob);
        break;
    }
  }
}

/*
 * send_messages is aware of the following entries in info:
 *
 * actor: single object
 * direct: array of objects
 * indirect: array of objects
 * instrument: array of objects
 * this: object generating this message
 *
 * If certain objects shouldn't get these messages, they should be in the
 * not_receiving list.
 */
atomic int send_messages(string type, string msg, mapping info, varargs object *receiving, object *not_receiving) {
  object *receivers;
  mixed *parsed_msg;
  object interpreted_msg;
  object e;
  int i, n;

  parsed_msg = TEMPLATE_P -> parse_template(msg);
  if(!parsed_msg) error("Unable to parse template: \"" + msg + "\"");

  interpreted_msg = interpret_template(parsed_msg);

  receivers = ({ });
  if(info["actor"]) receivers |= ({ info["actor"] });
  if(info["direct"]) receivers |= info["direct"];
  if(info["indirect"]) receivers |= info["indirect"];
  if(info["instrument"]) receivers |= info["instrument"];
  if(receiving) receivers |= receiving;
  if(not_receiving) receivers -= not_receiving;

  for(i = 0, n = sizeof(receivers); i < n; i++)
    EVENTS_D -> queue_event(receivers[i], "msg:" + type, ([
      "text": interpreted_msg -> render(info, receivers[i]),
      "receivers": receivers, /* so rooms don't pass the message on and players
                               * see it multiple times.
                               */
    ]));
}
