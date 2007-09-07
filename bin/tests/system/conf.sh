#!/bin/sh
#
# Copyright (C) 2000  Internet Software Consortium.
# 
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
# ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
# CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.

# $Id: conf.sh.in,v 1.4.2.1 2000/07/10 04:51:44 gson Exp $

# 
# Common configuration data for system tests, to be sourced into
# other shell scripts.
#

# Find the top of the BIND9 tree.
TOP=${SYSTEMTESTTOP:=.}/../../..

# Make it absolute so that it continues to work after we cd.
TOP=`cd $TOP && pwd`

NAMED=$TOP/bin/named/named
LWRESD=$TOP/bin/named/lwresd
DIG=$TOP/bin/dig/dig
NSUPDATE=$TOP/bin/nsupdate/nsupdate
KEYGEN=$TOP/bin/dnssec/dnssec-keygen
SIGNER=$TOP/bin/dnssec/dnssec-signzone
KEYSIGNER=$TOP/bin/dnssec/dnssec-signkey
KEYSETTOOL=$TOP/bin/dnssec/dnssec-makekeyset
SUBDIRS="dnssec glue limits lwresd notify nsupdate stub views xfer xferquota"

# PERL will be an empty string if no perl interpreter was found.
PERL=/usr/local/bin/perl5

export NAMED LWRESD DIG NSUPDATE KEYGEN SIGNER KEYSIGNER KEYSETTOOL PERL \
    SUBDIRS
