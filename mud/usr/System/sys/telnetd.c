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
    string banner;
    banner = status()[0] + "    Second Contract 0.0\n";
    banner += read_file("/txt/welcome.txt");
    return banner;
}
