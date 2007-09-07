/*
 * Copyright (C) 1999, 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* $Id: view.h,v 1.43 2000/06/22 21:56:23 tale Exp $ */

#ifndef DNS_VIEW_H
#define DNS_VIEW_H 1

/*****
 ***** Module Info
 *****/

/*
 * DNS View
 *
 * A "view" is a DNS namespace, together with an optional resolver and a
 * forwarding policy.  A "DNS namespace" is a (possibly empty) set of
 * authoritative zones together with an optional cache and optional
 * "hints" information.
 *
 * XXXRTH  Not all of this items can be set currently, but future revisions
 * of this code will support them.
 *
 * Views start out "unfrozen".  In this state, core attributes like
 * the cache, set of zones, and forwarding policy may be set.  While
 * "unfrozen", the caller (e.g. nameserver configuration loading
 * code), must ensure exclusive access to the view.  When the view is
 * "frozen", the core attributes become immutable, and the view module
 * will ensure synchronization.  Freezing allows the view's core attributes
 * to be accessed without locking.
 *
 * MP:
 *	Before the view is frozen, the caller must ensure synchronization.
 *
 *	After the view is frozen, the module guarantees appropriate
 *	synchronization of any data structures it creates and manipulates.
 *
 * Reliability:
 *	No anticipated impact.
 *
 * Resources:
 *	<TBS>
 *
 * Security:
 *	No anticipated impact.
 *
 * Standards:
 * None.  */

#include <isc/lang.h>
#include <isc/magic.h>
#include <isc/event.h>
#include <isc/net.h>
#include <isc/rwlock.h>
#include <isc/stdtime.h>

#include <dns/types.h>

ISC_LANG_BEGINDECLS

struct dns_view {
	/* Unlocked. */
	unsigned int			magic;
	isc_mem_t *			mctx;
	dns_rdataclass_t		rdclass;
	char *				name;
	dns_zt_t *			zonetable;
	dns_resolver_t *		resolver;
	dns_adb_t *			adb;
	dns_requestmgr_t *		requestmgr;
	dns_cache_t *			cache;
	dns_db_t *			cachedb;
	dns_db_t *			hints;
	dns_keytable_t *		secroots;
	dns_keytable_t *		trustedkeys;
	isc_mutex_t			lock;
	isc_rwlock_t			conflock;
	isc_boolean_t			frozen;
	isc_task_t *			task;
	isc_event_t			resevent;
	isc_event_t			adbevent;
	isc_event_t			reqevent;
	/* Configurable data, locked by conflock. */
	dns_tsig_keyring_t *		statickeys;
	dns_tsig_keyring_t *		dynamickeys;
	dns_peerlist_t *		peers;
	isc_boolean_t			recursion;
	isc_boolean_t			auth_nxdomain;
	dns_transfer_format_t		transfer_format;
	dns_acl_t *			queryacl;
	dns_acl_t *			recursionacl;
	isc_boolean_t			requestixfr;
	isc_boolean_t			provideixfr;
	dns_ttl_t			maxcachettl;
	dns_ttl_t			maxncachettl;
	in_port_t			dstport;

	/*
	 * Configurable data for server use only,
	 * locked by server configuration lock.
	 */
	dns_acl_t *			matchclients;
	
	/* Locked by lock. */
	unsigned int			references;
	unsigned int			weakrefs;
	unsigned int			attributes;
	/* Under owner's locking control. */
	ISC_LINK(struct dns_view)	link;
};

#define DNS_VIEW_MAGIC			0x56696577	/* View. */
#define DNS_VIEW_VALID(view)		ISC_MAGIC_VALID(view, DNS_VIEW_MAGIC)

#define DNS_VIEWATTR_RESSHUTDOWN	0x01
#define DNS_VIEWATTR_ADBSHUTDOWN	0x02
#define DNS_VIEWATTR_REQSHUTDOWN	0x04

isc_result_t
dns_view_create(isc_mem_t *mctx, dns_rdataclass_t rdclass,
		const char *name, dns_view_t **viewp);
/*
 * Create a view.
 *
 * Notes:
 *
 *	The newly created view has no cache, no resolver, and an empty
 *	zone table.  The view is not frozen.
 *
 * Requires:
 *
 *	'mctx' is a valid memory context.
 *
 *	'rdclass' is a valid class.
 *
 *	'name' is a valid C string.
 *
 *	viewp != NULL && *viewp == NULL
 *
 * Returns:
 *
 *	ISC_R_SUCCESS
 *	ISC_R_NOMEMORY
 *
 *	Other errors are possible.
 */

void
dns_view_attach(dns_view_t *source, dns_view_t **targetp);
/*
 * Attach '*targetp' to 'source'.
 *
 * Requires:
 *
 *	'source' is a valid, frozen view.
 *
 *	'targetp' points to a NULL dns_view_t *.
 *
 * Ensures:
 *
 *	*targetp is attached to source.
 *
 *	While *targetp is attached, the view will not shut down.
 */

void
dns_view_detach(dns_view_t **viewp);
/*
 * Detach '*viewp' from its view.
 *
 * Requires:
 *
 *	'viewp' points to a valid dns_view_t *
 *
 * Ensures:
 *
 *	*viewp is NULL.
 */

void
dns_view_weakattach(dns_view_t *source, dns_view_t **targetp);
/*
 * Weakly attach '*targetp' to 'source'.
 *
 * Requires:
 *
 *	'source' is a valid, frozen view.
 *
 *	'targetp' points to a NULL dns_view_t *.
 *
 * Ensures:
 *
 *	*targetp is attached to source.
 *
 * 	While *targetp is attached, the view will not be freed.
 */

void
dns_view_weakdetach(dns_view_t **targetp);
/*
 * Detach '*viewp' from its view.
 *
 * Requires:
 *
 *	'viewp' points to a valid dns_view_t *.
 *
 * Ensures:
 *
 *	*viewp is NULL.
 */

isc_result_t
dns_view_createresolver(dns_view_t *view,
			isc_taskmgr_t *taskmgr, unsigned int ntasks,
			isc_socketmgr_t *socketmgr,
			isc_timermgr_t *timermgr,
			unsigned int options,
			dns_dispatchmgr_t *dispatchmgr,
			dns_dispatch_t *dispatchv4,
			dns_dispatch_t *dispatchv6);
/*
 * Create a resolver and address database for the view.
 *
 * Requires:
 *
 *	'view' is a valid, unfrozen view.
 *
 *	'view' does not have a resolver already.
 *
 *	The requirements of dns_resolver_create() apply to 'taskmgr',
 *	'ntasks', 'socketmgr', 'timermgr', 'options', 'dispatchv4', and
 *	'dispatchv6'.
 *
 * Returns:
 *
 *     	ISC_R_SUCCESS
 *
 *	Any error that dns_resolver_create() can return.
 */

void
dns_view_setcache(dns_view_t *view, dns_cache_t *cache);
/*
 * Set the view's cache database.
 *
 * Requires:
 *
 *	'view' is a valid, unfrozen view.
 *
 *	'cache' is a valid cache.
 *
 * Ensures:
 *
 *     	The cache of 'view' is 'cached.
 *
 *	If this is not the first call to dns_view_setcache() for this
 *	view, then previously set cache is detached.
 */

void
dns_view_sethints(dns_view_t *view, dns_db_t *hints);
/*
 * Set the view's hints database.
 *
 * Requires:
 *
 *	'view' is a valid, unfrozen view, whose hints database has not been
 *	set.
 *
 *	'hints' is a valid zone database.
 *
 * Ensures:
 *
 *     	The hints database of 'view' is 'hints'.
 */

void
dns_view_setkeyring(dns_view_t *view, dns_tsig_keyring_t *ring);
/*
 * Set the view's static TSIG keys
 *
 * Requires:
 *
 *      'view' is a valid, unfrozen view, whose static TSIG keyring has not
 *	been set.
 *
 *      'ring' is a valid TSIG keyring
 *
 * Ensures:
 *
 *      The static TSIG keyring of 'view' is 'ring'.
 */

void
dns_view_setdstport(dns_view_t *view, in_port_t dstport);
/*
 * Set the view's destination port.  This is the port to
 * which outgoing queries are sent.  The default is 53,
 * the standard DNS port.
 *
 * Requires:
 *
 *      'view' is a valid view.
 *
 *      'dstport' is a valid TCP/UDP port number.
 *
 * Ensures:
 *	External name servers will be assumed to be listning
 *	on 'dstport'.  For servers whose address has already
 *	obtained obtained at the time of the call, the view may
 *	continue to use the previously set port until the address
 *	times out from the view's address database.
 */


isc_result_t
dns_view_addzone(dns_view_t *view, dns_zone_t *zone);
/*
 * Add zone 'zone' to 'view'.
 *
 * Requires:
 *
 *	'view' is a valid, unfrozen view.
 *
 *	'zone' is a valid zone.
 */

void
dns_view_freeze(dns_view_t *view);
/*
 * Freeze view.
 *
 * Requires:
 *
 *	'view' is a valid, unfrozen view.
 *
 * Ensures:
 *
 *	'view' is frozen.
 */

isc_result_t
dns_view_find(dns_view_t *view, dns_name_t *name, dns_rdatatype_t type,
	      isc_stdtime_t now, unsigned int options,
	      isc_boolean_t use_hints, dns_name_t *foundname,
	      dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset);
/*
 * Find an rdataset whose owner name is 'name', and whose type is
 * 'type'.
 *
 * Notes:
 *
 *	See the description of dns_db_find() for information about 'options'.
 *	If the caller sets DNS_DBFIND_GLUEOK, it must ensure that 'name'
 *	and 'type' are appropriate for glue retrieval.
 *
 *	If 'now' is zero, then the current time will be used.
 *
 *	If 'use_hints' is ISC_TRUE, and the view has a hints database, then
 *	it will be searched last.  If the answer is found in the hints
 *	database, the result code will be DNS_R_HINT.
 *
 *	'foundname' must meet the requirements of dns_db_find().
 *
 *	If 'sigrdataset' is not NULL, and there is a SIG rdataset which
 *	covers 'type', then 'sigrdataset' will be bound to it.
 *
 * Requires:
 *
 *	'view' is a valid, frozen view.
 *
 *	'name' is valid name.
 *
 *	'type' is a valid dns_rdatatype_t, and is not a meta query type
 *	(e.g. dns_rdatatype_any), or dns_rdatatype_sig.
 *
 *	'foundname' is 
 *
 *	'rdataset' is a valid, disassociated rdataset.
 *
 *	'sigrdataset' is NULL, or is a valid, disassociated rdataset.
 *
 * Ensures:
 *
 *	In successful cases, 'rdataset', and possibly 'sigrdataset', are
 *	bound to the found data.
 *
 * Returns:
 *
 *	Any result that dns_db_find() can return, with the exception of
 *	DNS_R_DELEGATION.
 */

isc_result_t
dns_view_simplefind(dns_view_t *view, dns_name_t *name, dns_rdatatype_t type,
		    isc_stdtime_t now, unsigned int options,
		    isc_boolean_t use_hints,
		    dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset);
/*
 * Find an rdataset whose owner name is 'name', and whose type is
 * 'type'.
 *
 * Notes:
 *
 *	This routine is appropriate for simple, exact-match queries of the
 *	view.  'name' must be a canonical name; there is no DNAME or CNAME
 *	processing.
 *
 *	See the description of dns_db_find() for information about 'options'.
 *	If the caller sets DNS_DBFIND_GLUEOK, it must ensure that 'name'
 *	and 'type' are appropriate for glue retrieval.
 *
 *	If 'now' is zero, then the current time will be used.
 *
 *	If 'use_hints' is ISC_TRUE, and the view has a hints database, then
 *	it will be searched last.  If the answer is found in the hints
 *	database, the result code will be DNS_R_HINT.
 *
 *	If 'sigrdataset' is not NULL, and there is a SIG rdataset which
 *	covers 'type', then 'sigrdataset' will be bound to it.
 *
 * Requires:
 *
 *	'view' is a valid, frozen view.
 *
 *	'name' is valid name.
 *
 *	'type' is a valid dns_rdatatype_t, and is not a meta query type
 *	(e.g. dns_rdatatype_any), or dns_rdatatype_sig.
 *
 *	'rdataset' is a valid, disassociated rdataset.
 *
 *	'sigrdataset' is NULL, or is a valid, disassociated rdataset.
 *
 * Ensures:
 *
 *	In successful cases, 'rdataset', and possibly 'sigrdataset', are
 *	bound to the found data.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS			Success; result is desired type.
 *	DNS_R_GLUE			Success; result is glue.
 *	DNS_R_HINT			Success; result is a hint.
 *	DNS_R_NCACHENXDOMAIN		Success; result is a ncache entry.
 *	DNS_R_NCACHENXRRSET		Success; result is a ncache entry.
 *	DNS_R_NXDOMAIN			The name does not exist.
 *	DNS_R_NXRRSET			The rrset does not exist.
 *	ISC_R_NOTFOUND			No matching data found,
 *					or an error occurred.
 */

isc_result_t
dns_view_findzonecut(dns_view_t *view, dns_name_t *name, dns_name_t *fname,
		     isc_stdtime_t now, unsigned int options,
		     isc_boolean_t use_hints,
		     dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset);
/*
 * Find the best known zonecut containing 'name'.
 *
 * This uses local authority, cache, and optionally hints data.
 * No external queries are performed.
 *
 * Notes:
 *
 *	If 'now' is zero, then the current time will be used.
 *
 *	If 'use_hints' is ISC_TRUE, and the view has a hints database, then
 *	it will be searched last.
 *
 *	If 'sigrdataset' is not NULL, and there is a SIG rdataset which
 *	covers 'type', then 'sigrdataset' will be bound to it.
 *
 * Requires:
 *
 *	'view' is a valid, frozen view.
 *
 *	'name' is valid name.
 *
 *	'rdataset' is a valid, disassociated rdataset.
 *
 *	'sigrdataset' is NULL, or is a valid, disassociated rdataset.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS				Success.
 *
 *	Many other results are possible.
 */

isc_result_t
dns_viewlist_find(dns_viewlist_t *list, const char *name,
		  dns_rdataclass_t rdclass, dns_view_t **viewp);
/*
 * XXX
 */

isc_result_t
dns_view_findzone(dns_view_t *view, dns_name_t *name, dns_zone_t **zone);
/*
 * XXX
 */

isc_result_t
dns_view_load(dns_view_t *view, isc_boolean_t stop);
/*
 * Load all zones attached to this view.  If 'stop' is ISC_TRUE,
 * stop on the first error and return it.  If 'stop'
 * is ISC_FALSE, ignore errors. 
 *
 * Requires:
 *
 *	'view' is a valid.
 */

isc_result_t
dns_view_checksig(dns_view_t *view, isc_buffer_t *source, dns_message_t *msg);
/*
 * Verifies the signature of a message.
 *
 * Requires:
 *
 *	'view' is a valid view.
 *	'source' is a valid buffer containing the message
 *	'msg' is a valid message
 *
 * Returns:
 *	see dns_tsig_verify()
 */

ISC_LANG_ENDDECLS

#endif /* DNS_VIEW_H */
