# include <type.h>
# include <system.h>
# include <toollib.h>
# include <kernel/kernel.h>

/*
 * This is the Finite State Machine that manages the actual REST resource
 * cycle. This is based on the Perl Web::Machine port of the original
 * Erlang by Basho.
 */

static void create(varargs int clone) {
}

static int is_status_code(mixed c) {
  if(typeof(c) != T_INT) return 0;
  if(c < 100 || c >= 600) return 0;
  return 1;
}

static int is_error(mixed c) {
  if(is_status_code(c) && c >= 400) return TRUE;
  return FALSE;
}

static int is_redirect(mixed c) {
  if(is_status_code(c) && (c == 301)) return TRUE;
  return FALSE;
}

static int is_new_state(mixed c) {
  if(c && typeof(c) == T_STRING) return 1;
  return 0;
}

static int is_bad_result(mixed c) {
  switch(typeof(c)) {
    case T_STRING: case T_INT: return FALSE;
    default: return TRUE;
  }
}

static string create_header(string key, string value) {
  switch(STRING_D -> lower_case(key)) {
    case "link":
    case "content-type":
    case "accept":
    case "accept-charset":
    case "accept-encoding":
    case "date":
    case "client-date":
    case "expires":
    case "last-modified":
    case "if-unmodified-since":
    case "if-modified-since":
    case "www-authenticate":
    case "authentication-info":
    case "authorization":
    default :
      return "";
  }
  return "";
}

static string choose_media_type(mixed *types, string accept_header) {
  return "application/json";
}

static string choose_language(mixed *languages, string accept_header) {
  return "en";
}

static string choose_charset( mixed *charsets, string accept_header ) {
  return "utf-8";
}

static string choose_encoding( mixed *encodings, string accept_header ) {
  return "identity";
}

static string ensure_quoted_header(string v) {
  if(v[0..0] == "\"" && v[strlen(v)-1..] == "\"") return v;
  return "\"" + v + "\"";
}

static string unquote_header(string v) {
  if(v[0..0] != "\"" || v[strlen(v)-1..] != "\"") return v;
  return v[1..strlen(v)-2];
}

static string _get_acceptable_content_type_handler(object resource, object request) {
  string handler;
  mixed *types;
  types = resource -> content_types_accepted();
  /* set the default handler as the first one */
  if(sizeof(types)) handler = types[0][1];
  return handler;
}

static void encode_body_if_set(object resource, object request) {
}

static void encode_body(object resource, object request) {
}

static void _add_caching_headers(object resource, object response) {
  string etag, expires, modified;

  if(etag = resource->generate_etag())
    response -> add_header("Etag", ensure_quoted_header(etag));
  if(expires = resource->expires())
    response -> add_header("Expires", expires);
  if(modified = resource->last_modified())
    response -> add_header("Last-Modified", modified);
}

static mixed _handle_304(object resource, object response) {
  response->remove_header("Content-Type");
  response->remove_header("Content-Encoding");
  response->remove_header("Content-Language");
  _add_caching_headers(resource, response);
  return 304;
}

/*
 * This won't return until the request is completed. We may change this in the
 * future to allow asynchronous processing of parts of the request cycle,
 * but we expect everything to go fairly quickly for now since we're dealing
 * with quick modification of core resources.
 */
atomic void run(object resource) {
  string state;
  mapping metadata;
  object response;
  mixed result;
  int counter;

  metadata = ([ ]);
  state = "b13";
  /* we want to do this in a way that doesn't hang - eventually, ditch the
   * while(1) in favor of yielding?
   */
  response = resource->get_response();
  counter = 0;
  while(counter < 1000) {
    counter ++;
    catch {
      result = call_other(this_object(), state, resource, resource->get_request(), response, metadata);
    } : {
      result = 500;
    }
    if( is_bad_result( result ) ) {
      response->set_status(500);
      response->add_header("Content-Type", "text/plain");
      response->set_body( ({ "Got bad state: " + (result ? result : "undef") }) );
      break;
    }
    else if( is_status_code( result ) ) { 
      response->set_status( result );
      if( is_error( result ) && !response->has_body() ) {
        response -> set_body("");
      }
      break;
    }
    else if( is_new_state( result ) ) {
      state = result;
    }
    else {
      response -> set_status( 500 );
      response->add_header("Content-Type", "text/plain");
      response->set_body( ({ "Got bad state: " + (result ? result : "undef") }) );
      break;
    }
  }
  if(counter >= 1000) {
    response -> set_status( 500 );
    response->add_header("Content-Type", "text/plain");
    response->set_body( ({ "Script ran too long." }) );
  }
}

mixed b13(object resource, object request, object response, mapping metadata) { /* service_available */
  return resource->service_available() ? "b12" : 500;
}

mixed b12(object resource, object request, object response, mapping metadata) { /* known_method */
  string method;
  method = request->get_method();
  return sizeof(({ method }) & resource->known_methods()) ? "b11" : 501;
}

mixed b11(object resource, object request, object response, mapping metadata) { /* uri_too_long */
  return resource->uri_too_long(request->get_uri()) ? 414 : "b10";
}

mixed b10(object resource, object request, object response, mapping metadata) { /* method_allowed */
  string method;
  method = request->get_method();
  if( sizeof( ({ method }) & resource->allowed_methods() ) ) return "b9";

  response -> add_header("Allow", implode(resource->allowed_methods(), ", "));
  return 405;
}

mixed b9(object resource, object request, object response, mapping metadata) { /* malformed_request */
  return resource->malformed_request() ? 400 : "b8";
}

mixed b8(object resource, object request, object response, mapping metadata) { /* is_authorized */
  mixed result;
  string str;

  str = request->get_header("Authorization");
  if(!str) str = request->get_header("authorization");
  if(str && str[strlen(str)-1] == 13) str = str[0..strlen(str)-2];
  
  result = resource->is_authorized( str );
  if(typeof(result) == T_INT) {
    if( result >= 100 ) return result;
    if( result ) return "b7";
  }
  if( typeof(result) == T_STRING && result ) {
    response -> add_header( "WWW-Authenticate", result );
  }
  return 401;
}

mixed b7(object resource, object request, object response, mapping metadata) { /* forbidden */
  return resource->forbidden() ? 403 : "b6";
}

mixed b6(object resource, object request, object response, mapping metadata) { /* content_headers_okay */
  mapping headers, content_headers;
  string *header_keys;
  int i, n;

  headers = request->get_headers();
  content_headers = ([ ]);
  header_keys = map_indices(headers);
  for(i = 0, n = sizeof(header_keys); i < n; i++)
    if(strlen(header_keys[i]) > 8 && header_keys[i][0..7] == "content-")
      content_headers[header_keys[i]] = headers[header_keys[i]];
  if(!content_headers["content-length"] && sizeof(({ request->get_method() }) & ({ "POST", "PUT" }))) return 411;
  return resource->valid_content_headers( content_headers ) ? "b5" : 501;
}

mixed b5(object resource, object request, object response, mapping metadata) { /* known_content_type */
  return resource->known_content_type( request -> get_content_type() ) ? "b4" : 415;
}

mixed b4(object resource, object request, object response, mapping metadata) { /* request_entity_too_large */
  return resource->valid_entity_length( request->get_content_length() ) ? "b3" : 413;
}

mixed b3(object resource, object request, object response, mapping metadata) { /* method_is_options */
  if( request->get_method() == "OPTIONS" ) {
    response->add_headers( resource -> options() );
    return 200;
  }
  return "c3";
}


mixed c3(object resource, object request, object response, mapping metadata) { /* accept_header_exists */
  if( 1 || !request -> get_header("Accept") ) {
    metadata["Content-Type"] = create_header("MediaType", resource->content_types_provided()[0][0]);
    return "d4";
  }
  return "c4";
}


mixed c4(object resource, object request, object response, mapping metadata) { /* acceptable_media_type_available */
  string chosen_type;

  chosen_type = choose_media_type( resource->content_types_provided(), request->get_header("Accept") );
  if(chosen_type) {
    metadata["Content-Type"] = chosen_type;
    return "d4";
  }
  return 406;
}

mixed d4(object resource, object request, object response, mapping metadata) { /* accept_language_header_exists */
  return request->get_header("Accept-Language") ? "d5" : "e5";
}

mixed d5(object resource, object request, object response, mapping metadata) { /* accept_language_choice_available */
  mixed language;

  language = choose_language( resource->languages_provided(), request->get_header("Accept-Language") );
  if( language ) {
    metadata["Language"] = language;
    if(!(typeof(language) == T_INT && language == 1))
      response->add_header("Content-Language", language);
    return "e5";
  }
  return 406;
}

mixed e5(object resource, object request, object response, mapping metadata) { /* accept_charset_exists */
  return request->get_header("Accept-Charset") ? "e6" : "f6";
}


mixed e6(object resource, object request, object response, mapping metadata) { /* accept_charset_choice_available */
  mixed charset;

  charset = choose_charset( resource->charsets_provided(), request->get_header("Accept-Charset") );
  if( charset ) {
    if( typeof(charset) != T_INT || charset != 1 )
      metadata["Charset"] = charset;
    return "f6";
  }
  return 406;
}

mixed f6(object resource, object request, object response, mapping metadata) { /* accept_encoding_exists */
  string charset, encoding;

  metadata["Content-Encoding"] = "identity";
  return "g7";

  if( charset = metadata["Charset"] )
    metadata["Content-Type"] -> add_param("charset", charset);
  response -> add_header( "Content-Type", metadata["Content-Type"] );

  if( request -> get_header("Accept-Encoding") ) return "f7";

  if( encoding = choose_encoding( resource->encodings_provided(), "identity;q=1.0,*;q=0.5" ) ) {
    if(encoding != "identity")
      response->add_header("Content-Encoding", encoding);
    metadata["Content-Encoding"] = encoding;
    return "g7";
  }
  return 406;
}

mixed f7(object resource, object request, object response, mapping metadata) { /* accept_encoding_choice_available */
  string encoding;

  if( encoding = choose_encoding( resource->encodings_provided(), request->get_header("Accept-Encoding") ) ) {
    if(encoding != "identity")
      response->add_header("Content-Encoding", encoding);
    metadata["Content-Encoding"] = encoding;
    return "g7";
  }
  return 406;
}

mixed g7(object resource, object request, object response, mapping metadata) { /* resource_exists */
  mixed *variances;

  variances = resource->variances();

  if(sizeof(resource->content_types_provided()) > 1)
    variances += ({ "Accept" });
  if(sizeof(resource->encodings_provided()) > 1)
    variances += ({ "Accept-Encoding" });
  if(resource->charsets_provided() && sizeof(resource->charsets_provided()) > 1)
    variances += ({ "Accept-Charset" });
  if(sizeof(resource->languages_provided()))
    variances += ({ "Accept-Language" });
  if(sizeof(variances) > 0)
    response -> add_header("Vary", implode(variances, ", "));
  return resource -> resource_exists() ? "g8" : "h7";
}

mixed g8(object resource, object request, object response, mapping metadata) { /* if_match_exists */
  return request->get_header("If-Match") ? "g9" : "h10";
}

mixed g9(object resource, object request, object response, mapping metadata) { /* if_match_is_wildcard */
  return unquote_header( request->get_header("If-Match") ) == "*" ? "h10" : "g11";
}


mixed g11(object resource, object request, object response, mapping metadata) { /* etag_in_if_match_list */
  string *etags;
  string etag;
  int i, n;

  etags = explode(request->header("If-Match"), ",");
  for(i = 0, n = sizeof(etags); i < n; i++)
    etags[i] = unquote_header(etags[i]);
  etag = resource->generate_etag();
  return sizeof(({ etag }) & etags) ? "h10" : 412;
}

mixed h7(object resource, object request, object response, mapping metadata) { /* if_match_exits_and_if_match_is_wildcard */
  if(request->get_header("If-Match") && unquote_header(request->get_header("If-Match")) == "*") {
    return 412;
  }
  else {
    return "i7";
  }
}

mixed h10(object resource, object request, object response, mapping metadata) { /* if_unmodified_since_exists */
  return request->get_header("If-Unmodified-Since") ? "h11" : "i12";
}

mixed h11(object resource, object request, object response, mapping metadata) { /* if_unmodified_since_is_valid_date */
  string date;

  date = request->get_header("If-Unmodified-Since");
  if(date) {
    metadata["If-Unmodified-Since"] = date;
    return "h12";
  }
  return "i12";
}

mixed h12(object resource, object request, object response, mapping metadata) { /* last_modified_is_greater_than_if_unmodified_since */
  if(resource -> last_modified() && resource -> last_modified() > metadata["If-Unmodified-Since"]) {
    return 412;
  }
  else {
    return "i12";
  }
}

mixed i4(object resource, object request, object response, mapping metadata) { /* moved_permanently */
  mixed uri;

  if(uri = resource->moved_permanently()) {
    if(is_status_code(uri)) return uri;
    response -> add_header("Location", uri);
    return 301;
  }
  return "p3";
}

mixed i7(object resource, object request, object response, mapping metadata) { /* method_is_put */
  return request->get_method() == "PUT" ? "i4" : "k7";
}

mixed i12(object resource, object request, object response, mapping metadata) { /* if_none_match_exists */
  return request->get_header("If-None-Match") ? "i13" : "l13";
}

mixed i13(object resource, object request, object response, mapping metadata) { /* if_none_match_is_wildcard */
  return request->get_header("If-None-Match") == "*" ? "j18" : "k13";
}

mixed j18(object resource, object request, object response, mapping metadata) { /* method_is_get_or_head */
  switch(request->get_method()) {
    case "GET": case "HEAD": return _handle_304(resource, request);
    default : return 412;
  }
}

mixed k5(object resource, object request, object response, mapping metadata) { /* moved_permanently */
  mixed uri;

  if(uri = resource -> moved_permanently()) {
    if( is_status_code(uri) ) return uri;
    response->add_header("Location", uri);
    return 301;
  }
  return "l5";
}

mixed k7(object resource, object request, object response, mapping metadata) { /* previously_existed */
  return resource -> previously_existed() ? "k5" : "l7";
}

mixed k13(object resource, object request, object response, mapping metadata) { /* etag_in_if_none_match */
  string *etags;
  string etag;
  int i, n, m, j;

  etags = explode(request -> get_header("If-None-Match"), ",");
  for(i = 0, n = sizeof(etags); i < n; i++) {
    j = 0; m = strlen(etags[i]);
    while(j < m && etags[i][j] == ' ') j++;
    etags[i] = etags[i][j..];
    j = strlen(etags[i]) - 1;
    while(j && etags[i][j] == ' ') j--;
    etags[i] = etags[i][0..j];
  }
  etags -= ({ "" });
  etag = resource -> generate_etag();
  for(i = 0, n = sizeof(etags); i < n; i++) {
    if(etags[i] == etag) return "j18";
  }
  return "l13";
}

mixed l5(object resource, object request, object response, mapping metadata) { /* moved_temporarily */
  mixed uri;

  if(uri = resource -> moved_temporarily()) {
    if(is_status_code(uri)) return uri;
    response -> add_header("Location", uri);
    return 307;
  }
  return "m5";
}

mixed l7(object resource, object request, object response, mapping metadata) { /* method_is_post */
  return request->get_method() == "POST" ? "m7" : 404;
}

mixed l13(object resource, object request, object response, mapping metadata) { /* if_modified_since_exists */
  return request->get_header("If-Modified-Since") ? "l14" : "m16";
}

mixed l14(object resource, object request, object response, mapping metadata) { /* if_modified_since_is_valid_date */
  string date;

  if(date = request->get_header("If-Modified-Since")) {
    metadata["If-Modified-Since"] = date;
    return "l15";
  }
  return "m16";
}

mixed l15(object resource, object request, object response, mapping metadata) { /* if_modified_since_greater_than_now */
  if(metadata["If-Modified-Since"] > time()) {
    return "m16";
  }
  else {
    return "l17";
  }
}

mixed l17(object resource, object request, object response, mapping metadata) { /* last_modified_is_greater_than_if_modified_since */
  if(resource -> last_modified() && resource->last_modified() > metadata["If-Modified-Since"]) {
    return "m16";
  }
  else {
    return _handle_304(resource, response);
  }
}

mixed m5(object resource, object request, object response, mapping metadata) { /* method_is_post */
  return request->get_method() == "POST" ? "n5" : 410;
}

mixed m7(object resource, object request, object response, mapping metadata) { /* allow_post_to_missing_resource */
  return resource->allow_missing_post() ? "n11" : 404;
}

mixed m16(object resource, object request, object response, mapping metadata) { /* method_is_delete */
  return resource->get_method() == "DELETE" ? "m20" : "n16";
}

mixed m20(object resource, object request, object response, mapping metadata) { /* delete_enacted_immediately */
  return resource -> delete_resource() ? "m20b" : 500;
}

mixed m20b(object resource, object request, object response, mapping metadata) { /* did_delete_complete */
  return resource -> delete_completed() ? "o20" : 202;
}

mixed n5(object resource, object request, object response, mapping metadata) { /* allow_post_to_missing_resource */
  return resource -> allow_missing_post() ? "n11" : 410;
}

static mixed _n11_create_path(object resource, object request, object response) {
  string uri;
  string base_uri;
  string new_uri;

  uri = resource -> create_path();
  if(!uri) return 500; /* server error */
  base_uri = resource -> base_uri();
  if(!base_uri) base_uri = request->base();

  if(uri[0] == '/' && base_uri[0] == '/') base_uri = base_uri[1..];
  if(uri[0] != '/' && base_uri[strlen(base_uri)-1] != '/') base_uri += "/";

  new_uri = base_uri + uri;
  response -> add_header("Location", new_uri);
  return TRUE;
}

mixed n11(object resource, object request, object response, mapping metadata) { /* redirect */
  mixed handler;
  mixed result;

  if(resource -> post_is_create()) {
    if(!resource->create_path_after_handler()) 
      if(_n11_create_path(resource, request, response) >= 500) return 500;
    handler = _get_acceptable_content_type_handler(resource, request);
    if(is_status_code(handler)) return handler;

    result = call_other(resource, handler, metadata);
    if(is_status_code(result)) return result;

    if(resource->create_path_after_handler())
      if(_n11_create_path(resource, request, response) >= 500) return 500;
  }
  else {
    handler = _get_acceptable_content_type_handler(resource, request);
    if(is_status_code(handler)) return handler;

    result = call_other(resource, handler, metadata);
    /* result = resource->process_post(); */
    if(result) {
      if(is_status_code(result)) return result;
      encode_body_if_set( resource, response );
    }
    else {
      return 500;
    }
  }

  if( is_redirect( response ) ) {
    if( response -> location() ) return 303;
    else return 506;
  }

  return "p11";
}

mixed n16(object resource, object request, object response, mapping metadata) { /* method_is_post */
  return request->get_method() == "POST" ? "n11" : "o16";
}

mixed o14(object resource, object request, object response, mapping metadata) { /* in_conflict */
  mixed handler;
  mixed result;

  if(resource -> is_conflict()) return 409;

  handler = _get_acceptable_content_type_handler( resource, request );
  if(is_status_code(handler)) return handler;

  result = call_other(resource, handler, metadata);
  if(is_status_code(result)) return result;

  return "p11";
}

mixed o16(object resource, object request, object response, mapping metadata) { /* method_is_put */
  return request->get_method() == "PUT" ? "o14" : "o18";
}

mixed o18(object resource, object request, object response, mapping metadata) { /* multiple_representations */
  string content_type;
  mixed result;
  int i, n;
  mixed *types;
  mixed handler;

  if(request->get_method() == "GET" || request->get_method() == "HEAD") {
    _add_caching_headers( resource, response );
    content_type = metadata["Content-Type"];
    types = resource -> content_types_provided();
    /* set the default handler as the first one */
    if(sizeof(types)) handler = types[0][1];
    for(i = 0, n = sizeof(types); i < n; i++) {
      if(types[i][0] == content_type) {
        handler = types[i][1];
        break;
      }
    }

    result = call_other(resource, handler, metadata);
    if(is_status_code(result)) return result;

    if(request->get_method() != "HEAD") {
      if(typeof(result) == T_STRING || typeof(result) == T_ARRAY) {
        response->set_body(result);
      }
      encode_body(resource, response);
    }
    return "o18b";
  }
  else {
    return "o18b";
  }
}

mixed o18b(object resource, object request, object response, mapping metadata) { /* multiple_choices */
  return resource->multiple_choices() ? 300 : 200;
}

mixed o20(object resource, object request, object response, mapping metadata) { /* response_body_includes_entity */
  return response->has_body() ? "o18" : 204;
}

mixed p3(object resource, object request, object response, mapping metadata) { /* in_conflict */
  mixed handler;
  mixed result;

  if(resource -> is_conflict()) return 409;
  handler = _get_acceptable_content_type_handler(resource, request);
  if(is_status_code(handler)) return handler;

  result = call_other(resource, handler);
  if(is_status_code(result)) return result;
  return "p11";
}

mixed p11(object resource, object request, object response, mapping metadata) { /* new_resource */
  if(!response->get_header("Location")) return "o20";
  else return 201;
}
