# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>

/*
 * tracks user accounts/passwords
 */

mapping accounts;
mapping groups;

void create() {
  groups = ([
    "ADMIN": ({ }),
  ]);
}

int create_group(string grp) {
  if(!SYSTEM()) {
    error("Only System may create new groups.");
    return FALSE;
  }
  if(groups[grp]) return FALSE;
  groups[grp] = ({ });
}

int is_in_group(string group, string user) {
  if(!groups[group]) return FALSE;
  return sizeof(({ user}) & groups[group]);
}

int add_user_to_group(string group, string user) {
  if(!SYSTEM()) {
    error("Only System may add a user to a group.");
    return FALSE;
  }
  if(!groups[group]) return FALSE;
  groups[group] |= ({ user });
}

int user_exists(string name) {
  if(!accounts) return FALSE;
  name = STRING_D -> lower_case(name);
  if(MAPPING_D -> specific_mapping(accounts, name)[name]) return 1;
  return 0;
}

int character_exists(string name) { 
  return CHARACTER_D -> character_exists(name);
}

int user_authenticated(string name, string password) {
  mapping m;

  if(!accounts) return FALSE;
  if(!name || !password) return FALSE;
  name = STRING_D -> lower_case(name);
  m = MAPPING_D -> specific_mapping(accounts, name);
  if(!m[name] || !m[name]["password"]) return FALSE;
  if(crypt(password, m[name]["password"]) == m[name]["password"]) return TRUE;
  return FALSE;
}

int set_user_password(string name, string password, varargs string old_password) {
  mapping m;
  int first_account;

  name = STRING_D -> lower_case(name);
  if(!accounts) {
    first_account = 1;
    accounts = ([ ]);
  }
  m = MAPPING_D -> specific_mapping(accounts, name);
  if(m[name] && m[name]["password"]) {
    if(crypt(password,m[name]["password"]) != m[name]["password"]) return 0;
  }
  else {
    m[name] = ([ "password": crypt(password) ]);
    if(first_account) {
      find_object(DRIVER)->message("Adding " + name + " to ADMIN group\n");
      add_user_to_group("ADMIN", name);
      previous_object() -> message("\n\nThis is the first account and has been added to the ADMIN group.\n\nUntil you run the bootstrap process to create character templates, you will not be able to create a character. Just hit return at the next prompt and you will be disconnected.\n\n");
    }
    return 1;
  }
}

int owns_character(string email, string char_name) {
  mapping user_m;

  user_m = MAPPING_D -> specific_mapping(accounts, email);
  if(!user_m[email] || !user_m[email]["characters"]) return 0;
  return sizeof(({ char_name }) & user_m[email]["characters"]);
}

int add_character(string email, object char) {
  string name;
  mapping user_m;

  user_m = MAPPING_D -> specific_mapping(accounts, email);

  if(!user_m[email]) return 0;

  if(!user_m[email]["characters"]) user_m[email]["characters"] = ({ });
  user_m[email]["characters"] += ({ char });
  return 1;
}

object *get_characters(string email) {
  mapping user_m;

  user_m = MAPPING_D -> specific_mapping(accounts, email);

  if(!user_m[email]) return ({ });

  if(user_m[email]["characters"]) return user_m[email]["characters"];
  return ({ });
}
