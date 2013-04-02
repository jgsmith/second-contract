# define USR_DIR		"/usr"	/* default user directory */
# define INHERITABLE_SUBDIR	"/lib/"
# define CLONABLE_SUBDIR	"/obj/"
# define LIGHTWEIGHT_SUBDIR	"/data/"

/*
 * Support both the old pre 1.4 network package, and 1.4's network extensions
 */
# if defined( __NETWORK_PACKAGE__ ) || defined( __NETWORK_EXTENSIONS__ )
#  define SYS_NETWORKING        /* Network package is enabled */
# endif

# define SYS_PERSISTENT			/* off by default */

# define SYSTEM_USER    "/usr/System/obj/user"
# define SYSTEM_WIZTOOL "/usr/System/obj/wiztool"
