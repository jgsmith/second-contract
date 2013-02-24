# include <kernel/kernel.h>
# include <kernel/user.h>
# include <kernel/access.h>
# include <system.h>
# include <worldlib.h>

inherit LIB_USER;
inherit user API_USER;
inherit access API_ACCESS;

/*
 * We want the following states:
 *   login -> password -> select character
 *   login -> password -> create character
 *   new login -> new password -> ... -> create (first) character
 *
 * STATE_NORMAL: we pass all input through the verb parser with the
 *    attached character as the actor
 */

# define STATE_NORMAL      0
# define STATE_LOGIN       1
# define STATE_OLDPASSWD   2
# define STATE_NEWPASSWD1  3
# define STATE_NEWPASSWD2  4

# define STATE_NEWACCOUNT           10
# define STATE_NEWACCOUNT_PASSWORD  11
# define STATE_NEWACCOUNT_PASSWORD2 12

# define STATE_SELECT_CHARACTER  20
# define STATE_CHAR_SELECTED     21

# define STATE_NEW_CHARACTER             30
# define STATE_NEW_CHAR_NAME_CONFIRM     31
# define STATE_NEW_CHAR_CAP_NAME         32
# define STATE_NEW_CHAR_CAP_NAME_CONFIRM 33
# define STATE_NEW_CHAR_GENDER           34

static string email;            /* email address - used to log on */
static mapping state;           /* state for a connection object */
static string Name, CapName;     /* used during character creation */
static int Gender;
static string newpasswd;        /* new password */
static object wiztool;          /* command handler */
static int nconn;               /* # of connections */
static object Character;        /* the character we're using as the actor */
static object *chars;
static int i, n;
static string oldpassword, password, name;

/*
 * NAME:        create()
 * DESCRIPTION:        initialize user object
 */
static void create(int clone)
{
    if (clone) {
        user::create();
        access::create();
        state = ([ ]);
    }
}

/*
 * NAME:        login()
 * DESCRIPTION:        login a new user
 */
int login(string str)
{
    if (previous_program() == LIB_CONN) {
        if (nconn == 0) {
            ::login(str);
        }
        nconn++;
        if (strlen(str) == 0 || sscanf(str, "%*s ") != 0 ||
            sscanf(str, "%*s/") != 0) {
            return MODE_DISCONNECT;
        }
        str = STRING_D -> lower_case(str);
        if(AUTH_D -> user_exists(str)) {
          previous_object() -> message("Password:");
          email = str;
          state[previous_object()] = STATE_LOGIN;
          return MODE_NOECHO;
        }
        if(str == "n") {
          /* new user */
          previous_object() -> message("What's your e-mail?");
          state[previous_object()] = STATE_NEWACCOUNT;
          return MODE_ECHO;
        }
        else {
          previous_object() -> message("\nThat user doesn't exist here.\n");
          return MODE_DISCONNECT;
        }
    }
}

/*
 * NAME:        logout()
 * DESCRIPTION:        logout user
 */
void logout(int quit)
{
    if (previous_program() == LIB_CONN && --nconn == 0) {
        if (query_conn()) {
            if (quit) {
            } else {
            }
        }
        ::logout();
        if (wiztool) {
            destruct_object(wiztool);
        }
        destruct_object(this_object());
    }
}

/*
 * NAME:        receive_message()
 * DESCRIPTION:        process a message from the user
 */
int receive_message(string str)
{
    if (previous_program() == LIB_CONN) {
        string cmd;
        object user, *users;
        int i, sz;

        switch (state[previous_object()]) {
        case STATE_NORMAL:
            cmd = str;
            if (strlen(str) != 0 && str[0] == '!') {
                cmd = cmd[1 ..];
            }

            if (!wiztool || !query_editor(wiztool) || cmd != str) {
                /* check standard commands */
                if (strlen(cmd) != 0) {
                }
            }

            if (str && str != "" && str[0..0] == "%" && wiztool) {
                wiztool->input(str[1..]);
            }
            break;

        case STATE_NEWACCOUNT:
            email = str;
            connection(previous_object());
            if(AUTH_D -> user_exists(email)) {
              message("\nThat user already has an account.");
              return MODE_DISCONNECT;
            }
            message("Select a password:");
            state[previous_object()] = STATE_NEWACCOUNT_PASSWORD;
            return MODE_NOECHO;

        case STATE_NEWACCOUNT_PASSWORD:
            newpasswd = str;
            if(AUTH_D -> user_exists(email)) {
              message("\nThat user already has account.");
              return MODE_DISCONNECT;
            }
            message("\nRetype password:");
            state[previous_object()] = STATE_NEWACCOUNT_PASSWORD2;
            return MODE_NOECHO;

        case STATE_NEWACCOUNT_PASSWORD2:
            if(newpasswd != str) {
              message("\nThose passwords don't match. Please come back and try again another time.");
              return MODE_DISCONNECT;
            }
            if(AUTH_D -> user_exists(email)) {
              message("\nThat user already has an account.");
              return MODE_DISCONNECT;
            }
            AUTH_D -> set_user_password(email, newpasswd);
            message("\nNow we'll walk you through creating your first character.");
            message("\n\nWhat name do you wish?");
            state[previous_object()] = STATE_NEW_CHARACTER;
            return MODE_ECHO;

        case STATE_NEW_CHARACTER:
            if(!str || str == "") {
              message("\nPerhaps try another time then?\n");
              return MODE_DISCONNECT;
            }
            if(AUTH_D -> character_exists(str)) {
              message("\nA character by that name already exists.\n");
              message("\nWhat name do you wish?");
              return MODE_ECHO;
            }
            message("\nAre you sure you want \"" + str + "\"? (Y/n)");
            Name = str;
            state[previous_object()] = STATE_NEW_CHAR_NAME_CONFIRM;
            return MODE_ECHO;

        case STATE_NEW_CHAR_NAME_CONFIRM:
            if(str == "Y" || str == "y" || str == "") {
              message("\nHow would you like your name capitalized?");
              state[previous_object()] = STATE_NEW_CHAR_CAP_NAME;
            }
            else {
              message("\n\nWhat name do you wish?");
              state[previous_object()] = STATE_NEW_CHARACTER;
            }
            return MODE_ECHO;

        case STATE_NEW_CHAR_CAP_NAME:
            message("\nAre you sure you want your name capitalized as \"" + str + "\"? (Y/n)");
            CapName = str;
            state[previous_object()] = STATE_NEW_CHAR_CAP_NAME_CONFIRM;
            return MODE_ECHO;

        case STATE_NEW_CHAR_CAP_NAME_CONFIRM:
            if(str == "Y" || str == "y" || str == "") {
              message("\nPlease choose an interesting gender (male, female, neutral, or none):\n");
              state[previous_object()] = STATE_NEW_CHAR_GENDER;
            }
            else {
              message("\nHow would you like your name capitalized?");
              state[previous_object()] = STATE_NEW_CHAR_CAP_NAME;
            }
            return MODE_ECHO;

        case STATE_NEW_CHAR_GENDER:
            switch(str) {
              case "male": Gender = 0; break;
              case "female": Gender = 1; break;
              case "neutral": case "neuter": case "none":
                Gender = 2; break;
              default:
                message("\nPlease choose an interesting gender (male, female, neutral, or none):\n");
                return MODE_ECHO;
            }

            message("\nPreparing to enter the game as " + CapName + ".\n");
            Character = CHARACTER_D -> create_character(Name, CapName, "human");
            Character -> set_gender(Gender);
            AUTH_D -> add_character(email, Character);
            state[previous_object()] = STATE_NORMAL;
            return MODE_ECHO;
            
        case STATE_LOGIN:
            if(!AUTH_D -> user_authenticated(email, str)) {
                previous_object()->message("\nBad password.\n");
                return MODE_DISCONNECT;
            }
            message("\n");
            connection(previous_object());
            if((chars = AUTH_D -> get_characters(email)) && sizeof(chars)) {
              message("Please select a character to play:\n");
              for(i = 0, n = sizeof(chars); i < n; i++) {
                message((i+1) + ") " + chars[i] -> get_name() + "\n");
              }
              if(n < 5) {
                message("\nOr \"N\" to create a new character. You may have up to 5 characters.\n");
              }
              message("\nPlease enter an option:");
              state[previous_object()] = STATE_SELECT_CHARACTER;
              return MODE_ECHO;
            }
            else {
              message("\nNow we'll walk you through creating your first character.");
              message("\n\nWhat name do you wish?");
              state[previous_object()] = STATE_NEW_CHARACTER;
              return MODE_ECHO;
            }

            /* tell_audience(Name + " logs in.\n"); */
            if (!wiztool &&
                (email == "admin" || AUTH_D -> is_wizard(email))) {
                wiztool = clone_object(SYSTEM_WIZTOOL, email);
            }
            break;

        case STATE_SELECT_CHARACTER:
            if(!str || str == "") {
              message("\nPerhaps try again another time.\n");
              return MODE_DISCONNECT;
            }
            if(str == "N" || str == "n") { /* new character! */
              message("\n\nWhat name do you wish?");
              state[previous_object()] = STATE_NEW_CHARACTER;
              return MODE_ECHO;
            }
            if(sscanf(str, "%d", i)) {
              i --;
              if(i < 0 || i > sizeof(chars)) {
                message("\nThat isn't a valid option.\n");
                return MODE_ECHO;
              }
              message("\nEntering the game as " + chars[i]->get_cap_name() + ".\n\n");
              Character = chars[i];
              /* connection(previous_object()); */
              state[previous_object()] = STATE_NORMAL;
              return MODE_ECHO;
            }
            message("\nThat isn't a valid option.\n");
            return MODE_ECHO;

        case STATE_OLDPASSWD:
            if(!AUTH_D -> user_authenticated(email, str)) {
                message("\nBad password.\n");
                break;
            }
            message("\nNew password:");
            oldpassword = str;
            state[previous_object()] = STATE_NEWPASSWD1;
            return MODE_NOECHO;

        case STATE_NEWPASSWD1:
            newpasswd = str;
            if(AUTH_D -> user_exists(email))
              message("\nRetype new password:");
            else
              message("\nRetype password:");
            state[previous_object()] = STATE_NEWPASSWD2;
            return MODE_NOECHO;

        case STATE_NEWPASSWD2:
            if (newpasswd == str) {
                password = hash_string("crypt", str);
                if(AUTH_D -> user_exists(email)) {
                  AUTH_D -> set_user_password(email, str);
                  if (wiztool) {
                      /* save wizards only */
                  }
                  message("\nPassword changed.\n");
                }
                else {
                  AUTH_D -> set_user_password(email, str);
                  state[previous_object()] = STATE_NEW_CHARACTER;
                }
            } else {
                if(AUTH_D -> user_exists(email)) {
                  message("\nMismatch. Password not changed.\n");
                }
                else {
                  message("\nMismatch. Please try again.\n");
                  return MODE_DISCONNECT;
                }
            }
            newpasswd = nil;
            break;
        }

        str = (wiztool) ? query_editor(wiztool) : nil;
        if (str) {
            message((str == "insert") ? "*\b" : ":");
        } else {
            message((email == "admin") ? "# " : "> ");
        }
        state[previous_object()] = STATE_NORMAL;
        return MODE_ECHO;
    }
}
