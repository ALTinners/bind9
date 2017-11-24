/*
 * Portions Copyright (C) 2000, 2001, 2003-2005, 2007, 2011, 2012, 2014-2016  Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 * Copyright (c) 1987, 1993
 *    The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*! \file herror.c
   lwres_herror() prints the string s on stderr followed by the string
   generated by lwres_hstrerror() for the error code stored in the global
   variable lwres_h_errno.

   lwres_hstrerror() returns an appropriate string for the error code
   gievn by err. The values of the error codes and messages are as
   follows:

\li   #NETDB_SUCCESS: Resolver Error 0 (no error)

\li   #HOST_NOT_FOUND: Unknown host

\li #TRY_AGAIN: Host name lookup failure

\li   #NO_RECOVERY: Unknown server error

\li   #NO_DATA: No address associated with name

 */

#if defined(LIBC_SCCS) && !defined(lint)
static const char sccsid[] = "@(#)herror.c	8.1 (Berkeley) 6/4/93";
static const char rcsid[] =
	"$Id$";
#endif /* LIBC_SCCS and not lint */

#include <config.h>

#include <stdio.h>

#include <isc/print.h>

#include <lwres/netdb.h>
#include <lwres/platform.h>

LIBLWRES_EXTERNAL_DATA int	lwres_h_errno;

/*!
 * these have never been declared in any header file so make them static
 */

static const char *h_errlist[] = {
	"Resolver Error 0 (no error)",		/*%< 0 no error */
	"Unknown host",				/*%< 1 HOST_NOT_FOUND */
	"Host name lookup failure",		/*%< 2 TRY_AGAIN */
	"Unknown server error",			/*%< 3 NO_RECOVERY */
	"No address associated with name",	/*%< 4 NO_ADDRESS */
};

static int	h_nerr = sizeof(h_errlist) / sizeof(h_errlist[0]);


/*!
 * herror --
 *	print the error indicated by the h_errno value.
 */
void
lwres_herror(const char *s) {
	fprintf(stderr, "%s: %s\n", s, lwres_hstrerror(lwres_h_errno));
}

/*!
 * hstrerror --
 *	return the string associated with a given "host" errno value.
 */
const char *
lwres_hstrerror(int err) {
	if (err < 0)
		return ("Resolver internal error");
	else if (err < h_nerr)
		return (h_errlist[err]);
	return ("Unknown resolver error");
}
