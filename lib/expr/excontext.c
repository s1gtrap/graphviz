/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/*
 * Glenn Fowler
 * AT&T Research
 *
 * expression library
 */

#include <expr/exlib.h>
#include <util/gv_ctype.h>

/*
 * copy input token error context into buf of n chars and reset the context
 * end of buf returned
 */

char*
excontext(Expr_t* p, char* buf, int n)
{
	char*	s;
	char*	t;
	char*	e;

	s = buf;
	if (p->linep > p->line || p->linewrap)
	{
		e = buf + n - 5;
		if (p->linewrap)
		{
			t = p->linep + 1;
			while (t < &p->line[sizeof(p->line)] && gv_isspace(*t))
				t++;
			if ((n = (sizeof(p->line) - (t - (p->linep + 1))) - (e - s)) > 0)
			{
				if (n > &p->line[sizeof(p->line)] - t)
					t = &p->line[sizeof(p->line)];
				else t += n;
			}
			while (t < &p->line[sizeof(p->line)])
				*s++ = *t++;
		}
		t = p->line;
		if (p->linewrap)
			p->linewrap = 0;
		else while (t < p->linep && gv_isspace(*t))
			t++;
		if ((n = (p->linep - t) - (e - s)) > 0)
			t += n;
		while (t < p->linep)
			*s++ = *t++;
		p->linep = p->line;
		t = "<<< ";
		while ((*s = *t++))
			s++;
	}
	*s = 0;
	return s;
}
