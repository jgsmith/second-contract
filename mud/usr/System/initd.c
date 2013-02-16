# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>

inherit rsrc API_RSRC;

static int create(varargs int clone) {
    object driver;
    object telnet_manager;
    object rsrc_manager;

    driver = find_object(DRIVER);

    if(!find_object("/usr/System/sys/telnetd")) {
        compile_object("/usr/System/sys/telnetd");
    }

    telnet_manager = find_object("/usr/System/sys/telnetd");
    "/kernel/sys/userd" -> set_telnet_manager(0, telnet_manager);

    rsrc::create();

    rsrc::add_owner("IFLib");
    rsrc::add_owner("WorldLib");
    rsrc::add_owner("DevLib");
    rsrc::add_owner("ToolLib");
}
