# include <kernel/user.h>
# include <config.h>

static void create(varargs int clone) {
    if(!find_object(SYSTEM_USER)) {
        compile_object(SYSTEM_USER);
    }
}

object select(string str) {
    return clone_object(SYSTEM_USER);
}

int query_timeout(object connection) {
    return DEFAULT_TIMEOUT;
}

string query_banner(object connection) {
    return "Welcome to SC! What's your name? ";
}
