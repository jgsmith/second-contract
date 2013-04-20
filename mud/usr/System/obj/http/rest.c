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
# include <type.h>

inherit LIB_USER;
inherit user API_USER;

# define RESPONSE_STARTED 1
# define RESPONSE_FINISHED 2

object request;
object response;
int request_status;
int response_state;

void create(varargs int clone) {
  if(clone) {
    request = new_object(HTTP_REQUEST);
    response = new_object(HTTP_RESPONSE);
    response_state = FALSE;
  }
}

void log_request(varargs object resource) {
  string *line;
  string rl, auth;
  object conn;
  int tmp;

  rl = request -> get_request_line();
  /* rl = implode(explode(rl, "\x0d"), "");
  rl = implode(explode(rl, "\x0a"), ""); */

  conn = this_object();
  while(conn <- LIB_USER) {
    conn = conn -> query_conn();
  }

  tmp = response -> get_content_length();
  if(resource) auth = resource -> get_auth_account();
  if(!auth) auth = "-";

  line = ({ 
    query_ip_number(conn), 
    auth, 
    "-",
    "[" + ctime(time()) + "]",
    "\"" + rl + "\"",
    ""+response -> get_status(),
    ""+(typeof(tmp) == T_INT ? tmp : "-"),
  });
  LOG_D -> log("http:access", implode(line, " "));
} 

int handle_request() {
  object resource;
  string err;

  if(response_state) return TRUE;

  if(request_status >= 0) {
    /*
     * given the request, we want to dispatch to the proper resource
     */
    resource = HTTP_D -> create_resource_handler(request);
    if(resource) {
      resource -> set_response(response);
      rlimits(1000;1000000) {
        err = catch( HTTP_FSM_D -> run(resource) );
        if(err) {
          response -> set_status(500);
          log_request(resource);
          response -> set_body(({
            "500 Internal Error - Too Much Time\n\n",
            err
           }));
          response_state = RESPONSE_STARTED;
          return TRUE;
        } 
      }
      if(!resource -> get_response()) {
        response -> set_status(500);
        log_request(resource);
        message("503 Internal Error - No Response\n");
        response_state = RESPONSE_FINISHED;
      }
      else {
        resource -> get_response() -> output(this_object());
        log_request(resource);
      }
    }
    else {
      response -> set_status(401);
      log_request();
      message("401 Not Found\n");
      response_state = RESPONSE_FINISHED;
    }
    if(!response_state) response_state = RESPONSE_STARTED;
    return TRUE;
  }
  return FALSE;
}

int logout() {
  if(request_status < 0) {
    message("501 Bad Request\n");
    response_state = RESPONSE_FINISHED;
  }
  return MODE_DISCONNECT;
}

int receive_message(string str) {
  /* this will break raw binary data uploads, but we aren't doing those for
   * now. Just JSON and plain text. Anyway, we can use base64 encoding if we
   * have to.
   */
  request_status = request -> parse_http_request(str);
  message(""); /* to keep things flowing */
  if(request_status >= 0) { handle_request(); }
  return MODE_RAW;
}

int login(string str) { 
  connection(previous_object());
  receive_message(str);
  return MODE_RAW;
}

int message_done() {
  switch(response_state) {
    case RESPONSE_FINISHED:
      destruct_object(this_object());
      return MODE_DISCONNECT;
    case RESPONSE_STARTED:
      if(response -> spool_finished()) {
        response_state = RESPONSE_FINISHED;
        destruct_object(this_object());
        return MODE_DISCONNECT;
      } 
      else {
        response -> spool_body(this_object());
        return MODE_RAW;
      }
  }
  return MODE_RAW;
}
