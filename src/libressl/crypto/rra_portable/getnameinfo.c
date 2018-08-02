/* $Id: getnameinfo.c 4022 2008-03-31 06:11:07Z rra $
 *
 * Replacement for a missing getnameinfo.
 *
 * This is an implementation of getnameinfo for systems that don't have one so
 * that networking code can use a consistant interface without #ifdef.  It is
 * a fairly minimal implementation, with the following limitations:
 *
 *   - IPv4 support only.  IPv6 is not supported.
 *   - NI_NOFQDN is ignored.
 *   - Not thread-safe due to gethostbyaddr, getservbyport, and inet_ntoa.
 *
 * The last two issues could probably be easily remedied, but haven't been
 * needed so far.  Adding IPv6 support isn't worth it; systems with IPv6
 * support should already support getnameinfo natively.
 *
 * Written by Russ Allbery <rra@stanford.edu>
 * This work is hereby placed in the public domain by its author.
 */

#include <rra_portable/getnameinfo.h>
#include <rra_portable/socket.h>
#include <rra_portable/snprintf.h>

#include <errno.h>
#include <string.h>

/* Linux doesn't provide EAI_OVERFLOW, so make up our own for testing. */
#ifndef EAI_OVERFLOW
# define EAI_OVERFLOW 10
#endif

/* Used for unused parameters to silence gcc warnings. */
#define UNUSED  __attribute__((__unused__))

#define Znk_NPOS ((size_t)(-1))
#define Znk_MIN(x,y) ( (x)<(y) ? (x) : (y) )
static void
ZnkS_copy( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	if( buf_size ){
		size_t cpy_size;
		cstr_leng = ( cstr_leng == Znk_NPOS ) ? strlen(cstr) : cstr_leng;
		cpy_size = Znk_MIN( cstr_leng, buf_size-1 );
		memmove( buf, cstr, cpy_size );
		buf[ cpy_size ] = '\0';
	}
}

/*
 * Check to see if a name is fully qualified by seeing if it contains a
 * period.  If it does, try to copy it into the provided node buffer and set
 * status accordingly, returning true.  If not, return false.
 */
static int
try_name(const char *name, char *node, socklen_t nodelen, int *status)
{
    if (strchr(name, '.') == NULL)
        return 0;
    if (strlen(name) > nodelen - 1)
        *status = EAI_OVERFLOW;
    else {
        ZnkS_copy( node, nodelen, name, Znk_NPOS );
        *status = 0;
    }
    return 1;
}


/*
 * Look up an address (or convert it to ASCII form) and put it in the provided
 * buffer, depending on what is requested by flags.
 */
static int
lookup_name(const struct in_addr *addr, char *node, socklen_t nodelen,
            int flags)
{
    struct hostent *host;
    char **alias;
    int status;
    char *name;

    /* Do the name lookup first unless told not to. */
    if (!(flags & NI_NUMERICHOST)) {
        host = gethostbyaddr((const void *) addr, sizeof(struct in_addr),
                             AF_INET);
        if (host == NULL) {
            if (flags & NI_NAMEREQD)
                return EAI_NONAME;
        } else {
            if (try_name(host->h_name, node, nodelen, &status))
                return status;
            for (alias = host->h_aliases; *alias != NULL; alias++)
                if (try_name(*alias, node, nodelen, &status))
                    return status;
        }

        /*
         * We found some results, but none of them were fully-qualified, so
         * act as if we found nothing and either fail or fall through.
         */
        if (flags & NI_NAMEREQD)
            return EAI_NONAME;
    }

    /* Just convert the address to ASCII. */
    name = inet_ntoa(*addr);
    if (strlen(name) > nodelen - 1)
        return EAI_OVERFLOW;
    ZnkS_copy( node, nodelen, name, Znk_NPOS );
    return 0;
}


/*
 * Look up a service (or convert it to ASCII form) and put it in the provided
 * buffer, depending on what is requested by flags.
 */
static int
lookup_service(unsigned short port, char *service, socklen_t servicelen,
               int flags)
{
    struct servent *srv;
    const char *protocol;

    /* Do the name lookup first unless told not to. */
    if (!(flags & NI_NUMERICSERV)) {
        protocol = (flags & NI_DGRAM) ? "udp" : "tcp";
        srv = getservbyport(htons(port), protocol);
        if (srv != NULL) {
            if (strlen(srv->s_name) > servicelen - 1)
                return EAI_OVERFLOW;
            ZnkS_copy( service, servicelen, srv->s_name, Znk_NPOS );
            return 0;
        }
    }

    /* Just convert the port number to ASCII. */
    if ((socklen_t) snprintf(service, servicelen, "%hu", port) > servicelen)
        return EAI_OVERFLOW;
    return 0;
}


/*
 * The getnameinfo implementation.
 */
int
rra_getnameinfo(const struct sockaddr *sa, socklen_t salen UNUSED, char *node,
            socklen_t nodelen, char *service, socklen_t servicelen, int flags)
{
    const struct sockaddr_in *sin;
    int status;
    unsigned short port;

    if ((node == NULL || nodelen <= 0) && (service == NULL || servicelen <= 0))
        return EAI_NONAME;

    /* We only support AF_INET. */
    if (sa->sa_family != AF_INET)
        return EAI_FAMILY;
    sin = (const struct sockaddr_in *) sa;

    /* Name lookup. */
    if (node != NULL && nodelen > 0) {
        status = lookup_name(&sin->sin_addr, node, nodelen, flags);
        if (status != 0)
            return status;
    }

    /* Service lookup. */
    if (service != NULL && servicelen > 0) {
        port = ntohs(sin->sin_port);
        return lookup_service(port, service, servicelen, flags);
    } else
        return 0;
}
