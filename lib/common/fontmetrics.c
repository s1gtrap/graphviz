/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "render.h"

static double timesFontWidth[] = {
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.3329, 0.4079, 0.5000, 0.5000, 0.8329, 0.7779, 0.3329,	/*  !"#$%&' */
    0.3329, 0.3329, 0.5000, 0.5639, 0.2500, 0.3329, 0.2500, 0.2779,	/* ()*+,-./ */
    0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000,	/* 01234567 */
    0.5000, 0.5000, 0.2779, 0.2779, 0.5639, 0.5639, 0.5639, 0.4439,	/* 89:;<=>? */
    0.9209, 0.7219, 0.6669, 0.6669, 0.7219, 0.6109, 0.5559, 0.7219,	/* @ABCDEFG */
    0.7219, 0.3329, 0.3889, 0.7219, 0.6109, 0.8889, 0.7219, 0.7219,	/* HIJKLMNO */
    0.5559, 0.7219, 0.6669, 0.5559, 0.6109, 0.7219, 0.7219, 0.9439,	/* PQRSTUVW */
    0.7219, 0.7219, 0.6109, 0.3329, 0.2779, 0.3329, 0.4689, 0.5000,	/* XYZ[\]^_ */
    0.3329, 0.4439, 0.5000, 0.4439, 0.5000, 0.4439, 0.3329, 0.5000,	/* `abcdefg */
    0.5000, 0.2779, 0.2779, 0.5000, 0.2779, 0.7779, 0.5000, 0.5000,	/* hijklmno */
    0.5000, 0.5000, 0.3329, 0.3889, 0.2779, 0.5000, 0.5000, 0.7219,	/* pqrstuvw */
    0.5000, 0.5000, 0.4439, 0.4799, 0.1999, 0.4799, 0.5409, 0.2500,	/* xyz{|}~  */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.3329, 0.5000, 0.5000, 0.1669, 0.5000, 0.5000, 0.5000,	/*  ������� */
    0.5000, 0.1799, 0.4439, 0.5000, 0.3329, 0.3329, 0.5559, 0.5559,	/* �������� */
    0.2500, 0.5000, 0.5000, 0.5000, 0.2500, 0.2500, 0.4529, 0.3499,	/*  ���� �� */
    0.3329, 0.4439, 0.4439, 0.5000, 1.0000, 1.0000, 0.2500, 0.4439,	/* ������ � */
    0.2500, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329,	/*  ������� */
    0.3329, 0.2500, 0.3329, 0.3329, 0.2500, 0.3329, 0.3329, 0.3329,	/* � �� ��� */
    1.0000, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/* �        */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.8889, 0.2500, 0.2759, 0.2500, 0.2500, 0.2500, 0.2500,	/*  � �     */
    0.6109, 0.7219, 0.8889, 0.3099, 0.2500, 0.2500, 0.2500, 0.2500,	/* ����     */
    0.2500, 0.6669, 0.2500, 0.2500, 0.2500, 0.2779, 0.2500, 0.2500,	/*  �   �   */
    0.2779, 0.5000, 0.7219, 0.5000, 0.2500, 0.2500, 0.2500, 0.2500,	/* ����     */
};
static double arialFontWidth[] = {
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.3549, 0.5559, 0.5559, 0.8889, 0.6669, 0.2209,	/*  !"#$%&' */
    0.3329, 0.3329, 0.3889, 0.5839, 0.2779, 0.3329, 0.2779, 0.2779,	/* ()*+,-./ */
    0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559,	/* 01234567 */
    0.5559, 0.5559, 0.2779, 0.2779, 0.5839, 0.5839, 0.5839, 0.5559,	/* 89:;<=>? */
    1.0149, 0.6669, 0.6669, 0.7219, 0.7219, 0.6669, 0.6109, 0.7779,	/* @ABCDEFG */
    0.7219, 0.2779, 0.5000, 0.6669, 0.5559, 0.8329, 0.7219, 0.7779,	/* HIJKLMNO */
    0.6669, 0.7779, 0.7219, 0.6669, 0.6109, 0.7219, 0.6669, 0.9439,	/* PQRSTUVW */
    0.6669, 0.6669, 0.6109, 0.2779, 0.2779, 0.2779, 0.4689, 0.5559,	/* XYZ[\]^_ */
    0.2219, 0.5559, 0.5559, 0.5000, 0.5559, 0.5559, 0.2779, 0.5559,	/* `abcdefg */
    0.5559, 0.2219, 0.2219, 0.5000, 0.2219, 0.8329, 0.5559, 0.5559,	/* hijklmno */
    0.5559, 0.5559, 0.3329, 0.5000, 0.2779, 0.5559, 0.5000, 0.7219,	/* pqrstuvw */
    0.5000, 0.5000, 0.5000, 0.3339, 0.2599, 0.3339, 0.5839, 0.2779,	/* xyz{|}~  */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.3329, 0.5559, 0.5559, 0.1669, 0.5559, 0.5559, 0.5559,	/*  ������� */
    0.5559, 0.1909, 0.3329, 0.5559, 0.3329, 0.3329, 0.5000, 0.5000,	/* �������� */
    0.2779, 0.5559, 0.5559, 0.5559, 0.2779, 0.2779, 0.5369, 0.3499,	/*  ���� �� */
    0.2219, 0.3329, 0.3329, 0.5559, 1.0000, 1.0000, 0.2779, 0.6109,	/* ������ � */
    0.2779, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329,	/*  ������� */
    0.3329, 0.2779, 0.3329, 0.3329, 0.2779, 0.3329, 0.3329, 0.3329,	/* � �� ��� */
    1.0000, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/* �        */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 1.0000, 0.2779, 0.3699, 0.2779, 0.2779, 0.2779, 0.2779,	/*  � �     */
    0.5559, 0.7779, 1.0000, 0.3649, 0.2779, 0.2779, 0.2779, 0.2779,	/* ����     */
    0.2779, 0.8889, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*  �   �   */
    0.2219, 0.6109, 0.9439, 0.6109, 0.2779, 0.2779, 0.2779, 0.2779,	/* ����     */
};
static double courFontWidth[] = {
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  !"#$%&' */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ()*+,-./ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* 01234567 */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* 89:;<=>? */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* @ABCDEFG */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* HIJKLMNO */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* PQRSTUVW */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* XYZ[\]^_ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* `abcdefg */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* hijklmno */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* pqrstuvw */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* xyz{|}~  */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ������� */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* �������� */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ���� �� */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ������ � */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ������� */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* � �� ��� */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* �        */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  � �     */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ����     */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  �   �   */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ����     */
};

#if defined(WITH_CODEGENS) && !defined(HAVE_GD_FREETYPE) && defined(HAVE_LIBGD)
extern codegen_t GD_CodeGen;
#endif

/* estimate_textsize:
 * Estimate width and height of text, for given face and size, in points.
 * Value is stored para->width.
 * NOTE: Tables are based on a font of size 1. Need to multiply by
 * fontsize to get appropriate value.
 */
static void
estimate_textlayout(graph_t *g, textpara_t * para, char **fontpath)
{
    double *Fontwidth;
    char c, *p, *fp;

    para->width = 0.0;
    para->height = para->fontsize * LINESPACING;
    para->yoffset_layout = 0.0;
    para->yoffset_centerline = 0.1 * para->fontsize;
    para->xshow = NULL;
    para->layout = para->fontname;
    para->free_layout = NULL;

    if (!strncasecmp(para->fontname, "cour", 4)) {
	fp = "[internal courier]";
	Fontwidth = courFontWidth;
    } else if (!strncasecmp(para->fontname, "arial", 5)
	       || !strncasecmp(para->fontname, "helvetica", 9)) {
	fp = "[internal arial]";
	Fontwidth = arialFontWidth;
    } else {
	fp = "[internal times]";
	Fontwidth = timesFontWidth;
    }
    if (fontpath)
	*fontpath = fp;
    if ((p = para->str)) {
	while ((c = *p++))
	    para->width += Fontwidth[(unsigned char) c];
	para->width *= para->fontsize;
    }
}

/*
 * This table maps standard Postscript font names to URW Type 1 fonts.
 *
 *   The original source is in ps_font_equiv.txt.  This is sorted 
 *   during make into ps_font_equiv.h to ensure that it is in the right
 *   order for bsearch()
 */
static PostscriptAlias postscript_alias[] = {
#include "ps_font_equiv.h"
};

static int fontcmpf(const void *a, const void *b)
{
    return (strcasecmp(((PostscriptAlias*)a)->name, ((PostscriptAlias*)b)->name));
}

static PostscriptAlias* translate_postscript_fontname(char* fontname)
{
    static PostscriptAlias key;
    static PostscriptAlias *result;

    if (key.name == NULL || strcasecmp(key.name, fontname)) {
        key.name = fontname;
        result = (PostscriptAlias *) bsearch((void *) &key,
			(void *) postscript_alias,
			sizeof(postscript_alias) / sizeof(PostscriptAlias),
			sizeof(PostscriptAlias),
                        fontcmpf);
    }
    return result;
}

pointf textsize(graph_t *g, textpara_t * para, char *fontname, double fontsize)
{
    char **fp = NULL, *fontpath = NULL;
    pointf size;

    para->fontname = fontname;
    para->fontsize = fontsize;

    para->postscript_alias = translate_postscript_fontname(fontname);

    if (Verbose && emit_once(para->fontname))
	fp = &fontpath;

    if (! gvtextlayout(GD_gvc(g), para, fp))
	estimate_textlayout(g, para, fp);

    if (fp)
	fprintf(stderr, "%s: fontname \"%s\" resolved to: %s\n",
		    GD_gvc(g)->common.cmdname, para->fontname, fontpath);

    size.x = para->width;
    size.y = para->height;
    return size;
}
