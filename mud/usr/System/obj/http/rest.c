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
object response;
int request_status;
int response_sent;

void create(varargs int clone) {
  if(clone) {
    request = new_object(HTTP_REQUEST);
    response = new_object(HTTP_RESPONSE);
    response_sent = FALSE;
  }
}

void log_request(varargs object resource) {
  string *line;
  string rl;

  rl = request -> get_request_line();
  /* rl = implode(explode(rl, "\x0d"), "");
  rl = implode(explode(rl, "\x0a"), ""); */

  line = ({ "<client ip>" });
  line += ({ (resource && resource -> get_auth() ? resource -> get_auth() : "-") });
  line += ({ "-" });
  line += ({ "[" + ctime(time()) + "]" });
  line += ({ "\"" + rl + "\"" });
  line += ({ ""+response -> get_status() });
  line += ({ ""+(response -> get_content_length() || "-") });
  LOG_D -> log("http:access", implode(line, " "));
} 

int handle_request() {
  object resource;

  if(response_sent) return TRUE;

  if(request_status >= 0) {
    /*
     * given the request, we want to dispatch to the proper resource
     */
    resource = HTTP_D -> create_resource_handler(request);
    if(resource) {
      resource -> set_response(response);
      rlimits(1000;1000000) {
        catch {
          HTTP_FSM_D -> run(resource);
        } : {
          response -> set_status(500);
          log_request(resource);
          message("500 Internal Error - Too Much Time\n");
          response_sent = TRUE;
          return TRUE;
        } 
      }
      if(!resource -> get_response()) {
        response -> set_status(500);
        log_request(resource);
        message("503 Internal Error - No Response\n");
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
    }
    response_sent = TRUE;
    return TRUE;
  }
  return FALSE;
}

int logout() {
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
  request_status = request -> parse_http_request(str);
  message(""); /* to keep things flowing */
  if(request_status >= 0 && handle_request()) return MODE_DISCONNECT;
  return MODE_RAW;
}

int login(string str) { 
  connection(previous_object());
  receive_message(str);
  return MODE_RAW;
}

int message_done() {
  if(handle_request()) {
    destruct_object(this_object());
    return MODE_DISCONNECT;
  }
  return MODE_RAW;
}
