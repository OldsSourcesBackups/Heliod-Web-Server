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
 * regex_string.cpp
 *
 * A utility class that allows to grow a string indefinitely and then 
 *   use it.  This class is VERY basic and only provides the functionality
 *   that is needed for the task at hand.
 */
#if (defined(__GNUC__) && (__GNUC__ > 2))
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif
#include <string.h>
#include <sys/types.h>
#include "regex_string.h"

RegexString::RegexString() 
{
    int len = INITIAL_STRING_SIZE;
    str = new char[len];
    offset = 0;
    max = len;
};

RegexString::RegexString(int len)
{
	str = new char[len];
	offset = 0;
	max = len;
}

RegexString::~RegexString()
{
	delete[] str;
}

void RegexString::reset()
{
	offset = 0;
}

void RegexString::add(const char *cp, int length)
{
	int expand = 0;
	char *newstr;

	if (!cp)
		return;

	// Always leave room for the EOL
	if (length > (max-offset-1)) {
		if (length > max)
			expand = max + length;
		else
			expand = max + max;

		newstr = new char[expand];
		memcpy (newstr, str, offset);
		delete[] str;
		str = newstr;
		max = expand;
	}
	
	memcpy (str+offset, cp, length);
	offset += length;
}

void RegexString::add(const char *cp)
{
	if (cp)
		add(cp, strlen(cp));
}

int RegexString::length()
{
	return (offset);
}


char* RegexString::takestring()
{
	char *cp = str;
	cp[offset] = '\0';

	offset = 0;
	max = INITIAL_STRING_SIZE;
	str = new char[max];

	return (cp);
}
