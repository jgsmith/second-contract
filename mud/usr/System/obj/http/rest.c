/*
 * This object handles connections on a single port and passes them on to
 * data resource objects of the right type manages by the http/rest/fsm daemon.
 *
 * We assume a REST-style response cycle.
 */

# include <system.h>
# include <system/http.h>
# include <kernel/kernel.h>
# include <kernel/user.h>

inherit LIB_USER;
inherit user API_USER;

object request;
int request_status;

void create(varargs int clone) {
  if(clone) {
    request = new_object(HTTP_REQUEST);
  }
}

int logout() {
  message("logout result: ["+request_status+"]\n");
  if(request_status < 0) {
    message("501 Bad Request\n");
  }
  return MODE_DISCONNECT;
}

int receive_message(string str) {
  /* this will break raw binary data uploads, but we aren't doing those for
   * now. Just JSON and plain text. Anyway, we can use base64 encoding if we
   * have to.
   */
  request_status = request -> parse_http_request(
    implode(
      explode(
        implode(
          explode(str,"\x0d\x0a")
          , "\n"
        ), "\x0a"
      ), "\n"
    ) + "\n"
  );
  message(""); /* to keep things flowing */
  if(request_status >= 0) {
    /* we have the full request! */
  }
  return MODE_RAW;
}

int login(string str) { 
  connection(previous_object());
  return MODE_RAW;
}

int message_done() {
  if(request_status >= 0) {
    message("200 OK\n");
    return MODE_DISCONNECT;
  }
  return MODE_RAW;
}
