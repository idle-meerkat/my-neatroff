#include <stdio.h>
#include <stdlib.h>
#include "xroff.h"

#define CPBUF		4

static int cp_nblk;		/* input block depth (text in \{ and \}) */
static int cp_sblk[NIES];	/* skip \} escape at this depth, if set */
static int cp_widreq = 1;	/* inline \w requests */

static int regid(void)
{
	int c1;
	int c2 = 0;
	c1 = cp_next();
	if (c1 == '(') {
		c1 = cp_next();
		c2 = cp_next();
	}
	return REG(c1, c2);
}

static void cp_num(void)
{
	int id;
	int c = cp_next();
	if (c != '-' && c != '+')
		cp_back(c);
	id = regid();
	if (c == '-' || c == '+')
		num_get(id, c == '+' ? 1 : -1);
	if (num_str(id))
		in_push(num_str(id), NULL);
}

static void cp_str(void)
{
	char *buf = str_get(regid());
	if (buf)
		in_push(buf, NULL);
}

static void cp_arg(void)
{
	int c;
	char *arg = NULL;
	c = cp_next();
	if (c >= '1' && c <= '9')
		arg = in_arg(c - '0');
	if (arg)
		in_push(arg, NULL);
}

static void cp_width(void)
{
	char wid[16];
	sprintf(wid, "%d", ren_wid(cp_next, cp_back));
	in_push(wid, NULL);
}

static int cp_raw(void)
{
	int c;
	if (in_top() >= 0)
		return in_next();
	do {
		c = in_next();
	} while (c == c_ni);
	if (c == c_ec) {
		do {
			c = in_next();
		} while (c == c_ni);
		if (c == '\n')
			return cp_raw();
		if (c == '.')
			return '.';
		if (c == '\\') {
			in_back('\\');
			return c_ni;
		}
		if (c == 't') {
			in_back('\t');
			return c_ni;
		}
		if (c == 'a') {
			in_back('');
			return c_ni;
		}
		if (c == '{' && cp_nblk < LEN(cp_sblk))
			cp_sblk[cp_nblk++] = 0;
		if (c == '}' && cp_nblk > 0)
			if (cp_sblk[--cp_nblk])
				return cp_raw();
		in_back(c);
		return c_ec;
	}
	return c;
}

int cp_next(void)
{
	int c;
	if (in_top() >= 0)
		return in_next();
	c = cp_raw();
	if (c == c_ec) {
		c = cp_raw();
		if (c == '"') {
			while (c >= 0 && c != '\n')
				c = cp_raw();
		} else if (c == 'w' && cp_widreq) {
			cp_width();
			c = cp_next();
		} else if (c == 'n') {
			cp_num();
			c = cp_next();
		} else if (c == '*') {
			cp_str();
			c = cp_next();
		} else if (c == '$') {
			cp_arg();
			c = cp_next();
		} else {
			cp_back(c);
			c = c_ec;
		}
	}
	return c;
}

void cp_blk(int skip)
{
	int c;
	int nblk = cp_nblk;
	do {
		c = skip ? cp_raw() : cp_next();
	} while (c == ' ' || c == '\t');
	if (c == c_ec && in_top() == '{') {	/* a troff \{ \} block */
		if (skip) {
			while (skip && cp_nblk > nblk && c >= 0)
				c = cp_raw();
		} else {
			cp_sblk[nblk] = 1;
			cp_raw();
		}
	} else {
		if (!skip)
			cp_back(c);
	}
	while (skip && c != '\n')	/* skip until the end of the line */
		c = cp_raw();
}

void cp_wid(int enable)
{
	cp_widreq = enable;
}
