/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright 2008 Sun Microsystems, Inc. All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 *
 * Neither the name of the  nor the names of its contributors may be
 * used to endorse or promote products derived from this software without 
 * specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*

CONFIDENTIAL AND PROPRIETARY SOURCE CODE OF NETSCAPE COMMUNICATIONS
CORPORATION

Copyright (c) 1996 Netscape Communications Corporation.  All Rights Reserved.

Use of this Source Code is subject to the terms of the applicable
license agreement from Netscape Communications Corporation.

The copyright notice(s) in this Source Code does not indicate actual or
intended publication of this Source Code.

*/

/*
 * Description (nseframe.c)
 *
 *	This module is part of the NSACL_RES_ERROR facility.  It contains functions
 *	for allocating, freeing, and managing error frame structures.  It
 *	does not contain routines for generating error messages through
 *	the use of a message file.
 */

#include "base/systems.h"
#include "netsite.h"
#include "nserror.h"

/*
 * Description (nserrDispose)
 *
 *	This function is used to dispose of an entire list of error
 *	frames when the error information is no longer needed.  It
 *	does not free the list head, since it is usually not dynamically
 *	allocated.
 *
 * Arguments:
 *
 *	errp			- error frame list head pointer
 */

void nserrDispose(NSErr_t * errp)
{
    /* Ignore null list head */
    if (errp == 0) return;

    while (errp->err_first) {

	nserrFFree(errp, errp->err_first);
    }
}

/*
 * Description (nserrFAlloc)
 *
 *	This is the default allocator for error frame structures.  It
 *	calls an allocator function indicated by err_falloc, if any,
 *	or else uses MALLOC().
 *
 * Arguments:
 *
 *	errp			- error frame list head pointer
 *				  (may be null)
 *
 * Returns:
 *
 *	The return value is a pointer to a cleared error frame.  The
 *	frame will not have been added to the list referenced by errp.
 *	An allocation error is indicated by a null return value.
 */

NSEFrame_t * nserrFAlloc(NSErr_t * errp)
{
    NSEFrame_t * efp;			/* return error frame pointer */

    /* Allocate the error frame */
    efp = (errp && errp->err_falloc)
			? (*errp->err_falloc)(errp)
			: (NSEFrame_t *)MALLOC(sizeof(NSEFrame_t));

    if (efp) {
	/* Clear the error frame */
	memset((void *)efp, 0, sizeof(NSEFrame_t));
    }

    return efp;
}

/*
 * Description (nserrFFree)
 *
 *	This function frees an error frame structure.  If an error list
 *	head is specified, it first checks whether the indicated error
 *	frame is on the list, and removes it if so.  If the ef_dispose
 *	field is non-null, the indicated function is called.  The error
 *	frame is deallocated using either a function indicated by
 *	err_free in the list head, or FREE() otherwise.
 *
 * Arguments:
 *
 *	errp			- error frame list head pointer
 *				  (may be null)
 *	efp			- error frame pointer
 */

void nserrFFree(NSErr_t * errp, NSEFrame_t * efp)
{
    NSEFrame_t **lefp;		/* pointer to error frame pointer */
    NSEFrame_t * pefp;		/* previous error frame on list */
    int i;

    /* Ignore null error frame pointer */
    if (efp == 0) return;

    /* Got a list head? */
    if (errp) {

	/* Yes, see if this frame is on the list */
	pefp = 0;
	for (lefp = &errp->err_first; *lefp != 0; lefp = &pefp->ef_next) {
	    if (*lefp == efp) {

		/* Yes, remove it */
		*lefp = efp->ef_next;
		if (errp->err_last == efp) errp->err_last = pefp;
		break;
	    }
	    pefp = *lefp;
	}
    }

    /* Free strings referenced by the frame */
    for (i = 0; i < efp->ef_errc; ++i) {
	if (efp->ef_errv[i]) {
	    FREE(efp->ef_errv[i]);
	}
    }

    /* Free the frame */
    if (errp && errp->err_ffree) {
	(*errp->err_ffree)(errp, efp);
    }
    else {
	FREE(efp);
    }
}

/*
 * Description (nserrGenerate)
 *
 *	This function is called to generate an error frame and add it
 *	to a specified error list.
 *
 * Arguments:
 *
 *	errp			- error frame list head pointer
 *				  (may be null)
 *	retcode			- return code (ef_retcode)
 *	errorid			- error id (ef_errorid)
 *	program			- program string pointer (ef_program)
 *	errc			- count of error arguments (ef_errc)
 *	...			- values for ef_errv[]
 *
 * Returns:
 *
 *	The return value is a pointer to the generated error frame, filled
 *	in with the provided information.  An allocation error is indicated
 *	by a null return value.
 */

NSAPI_PUBLIC NSEFrame_t * nserrGenerate(NSErr_t * errp, long retcode, long errorid,
			   char * program, int errc, ...)
{
    NSEFrame_t * efp;			/* error frame pointer */
    char * esp;				/* error string pointer */
    int i;
    va_list ap;

    /* Null frame list head pointer means do nothing */
    if (errp == 0) {
	return 0;
    }

    /* Limit the number of values in ef_errv[] */
    if (errc > NSERRMAXARG) errc = NSERRMAXARG;

    /* Allocate the error frame */
    efp = nserrFAlloc(errp);

    /* Did we get it? */
    if (efp) {

	/* Yes, copy information into it */
	efp->ef_retcode = retcode;
	efp->ef_errorid = errorid;
	efp->ef_program = program;
	efp->ef_errc = errc;

	/* Get the string arguments and copy them */
	va_start(ap, errc);
	for (i = 0; i < errc; ++i) {
	    esp = va_arg(ap, char *);
	    efp->ef_errv[i] = STRDUP(esp);
	}

	/* Add the frame to the list (if any) */
	if (errp) {
	    efp->ef_next = errp->err_first;
	    errp->err_first = efp;
	    if (efp->ef_next == 0) errp->err_last = efp;
	}
    }

    /* Return the error frame pointer */
    return efp;
}
