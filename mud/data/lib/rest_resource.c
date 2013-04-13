# include <toollib.h>
# include <gamelib.h>
# include <system.h>
# include <system/http.h>
# include <status.h>
# include <kernel/kernel.h>

object request;
object response;
string base;

/* We have a few convenience methods that let us set up common auth
 * requirements.
 * HTTP_REQUIRES_ADMIN -> requires an admin account
 * HTTP_REQUIRES_GROUP -> requires membership in the named creator group
 * HTTP_REQUIRES_OWNER -> requires the owner of the named character
 * HTTP_DISALLOWS_DELETE -> requires ADMIN to delete
 * HTTP_ALLOWS_PUBLIC_READ -> allows GET requests for this resource if
 *     not authenticated - useful for allowing public browsing of things
 *     like recipes.
 */
int auth_flags;
string owner;
string auth_account;
mapping parameters;

string *allowed_methods();

static void create(varargs int clone) {
  auth_flags = HTTP_REQUIRES_ADMIN;
  if(clone) {
    parameters = ([ ]);
  }
}

void set_request(object r) { request = r; }
object get_request() { return request; }

void set_response(object r) { response = r; }
object get_response() { return response; }

void set_base(string b) { base = b; }

string get_resource_id() {
  return parameters["id"];
}

void set_resource_id(string id) {
  parameters["id"] = id;
}

string get_parameter(string p) { return parameters[p]; }
string get_parameters(string p) { return parameters[p]; }

void set_parameters(mapping p) { parameters += p; }

/*
 * If using the default authorization handler, this will return the
 * account that was authenticated.
 */
string get_auth_account() { return auth_account; }

/*
 * Only the resource object should manage the authentication requirements.
 */
static int set_authentication_flags(int x) {
  auth_flags = x;
}

int resource_exists() { return 1; }

int service_available() { return 1; }

/* This checks if we have authentication information if we need it.
 * Assumes Basic authentication .
 * N.B.: This should only be done over SSL/TLS connections.
 */
mixed is_authorized(string auth) { 
  string *bits;

  if(request -> get_method() == "OPTIONS") return TRUE;

  if((auth_flags & HTTP_ALLOWS_PUBLIC_READ) && (request->get_method() == "GET" || request->get_method() == "HEAD" || request->get_method() == "OPTIONS")) {
    return TRUE;
  }

  if(auth && strlen(auth) > 6 && auth[0..5] == "Basic ") {
    auth = BASE64_D->decode(explode(auth, " ")[1]);
    bits = explode(auth, ":");
    if(sizeof(bits) > 2) bits[1] = implode(bits[1..], ":");
    bits[0] = STRING_D -> lower_case(bits[0]);
    if(AUTH_D->user_authenticated(bits[0], bits[1])) {
      auth_account = bits[0];
      return TRUE;
    }
  }
  auth = GAME_D -> get_name();
  get_response() -> add_headers(this_object() -> options());
  return "Basic realm=\"" + auth + "\"";
}


int allow_missing_post() { return 0; }

int malformed_request() { return 0; }

int uri_too_long(string uri) { return 0; }

int known_content_type(string content_type) { return 1; }

int valid_content_headers(mapping headers) { return 1; }

int valid_entity_length(int length) {
  return length <= status(ST_STRSIZE);
}


mapping options() { return ([ 
  "Access-Control-Allow-Methods": implode(allowed_methods(), ", "),
  "Access-Control-Allow-Headers": implode(({ get_request()->get_header("Access-Control-Request-Headers"), "Authorization" }) - ({ nil, "" }), ","),
]); }

string *allowed_methods() { 
  string *allowed;
  int full_access;

  return ({ "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE" });
}

int forbidden() { 
  string *allowed;
  int full_access;

  if(request -> get_method() == "OPTIONS") return FALSE;

  if(auth_account && AUTH_D -> is_in_group("ADMIN", auth_account)) {
    return FALSE;
  }

  allowed = ({ "OPTIONS" });

  if(auth_flags & HTTP_ALLOWS_PUBLIC_READ) {
    allowed |= ({ "GET", "HEAD" });
  }

  switch(auth_flags & HTTP_REQUIRES_USER_BITS) {
    case HTTP_REQUIRES_ADMIN: return FALSE; /* handled above */
    case HTTP_REQUIRES_GROUP:
      full_access = AUTH_D->is_in_group(owner, auth_account);
      break;
    case HTTP_REQUIRES_OWNER:
      full_access = AUTH_D->owns_character(auth_account, owner);
      break;
  }
  if(full_access) allowed |= ({ "POST", "PUT", "GET", "HEAD" });
  if(full_access && !(auth_flags & HTTP_DISALLOWS_DELETE))
    allowed |= ({ "DELETE" });

  if(sizeof(({ request->get_method() }) & allowed)) return FALSE;
  return TRUE;
}

string *known_methods() { return ({ "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT", "OPTIONS" }); }

int delete_resource() { return 0; }

int delete_completed() { return 1; }

int post_is_create() { return 0; }

string create_path() { return nil; }

string base_uri() { return nil; }

int process_post() { return 0; }

mixed *content_types_provided() {
  return ({ ({ "application/json", "to_json" }) });
}

mixed *content_types_accepted() {
  return ({ ({ "application/json", "from_json" }) });
}

/* 
mixed *content_types_provided() { return ({ }); }

mixed *content_types_accepted() { return ({ }); }
*/

mixed *charsets_provided() { return ({ }); }

mixed *languages_provided() { return ({ }); }

mixed *encodings_provided() { 
  return ({
    ({ "identity", "encoding_identity" }),
  });
}

mixed *variances() { return ({ }); }

int is_conflict() { return 0; }

int multiple_choices() { return 0; }

int previously_existed() { return 0; }

int moved_permanently() { return 0; }

int moved_temporarily() { return 0; }

string last_modified() { return nil; }

string expires() { return nil; }

string generate_etag() { return nil; }

void finish_request() { }

/*
 * The following are callbacks based on information returned by the above API
 */

string encoding_identity(string s) { return s; }
