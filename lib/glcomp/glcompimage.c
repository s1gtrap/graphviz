/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <cgraph/alloc.h>
#include <glcomp/glcompimage.h>
#include <glcomp/glcompfont.h>
#include <glcomp/glcompset.h>
#include <glcomp/glutils.h>
#include <glcomp/glcomptexture.h>
#include <stdbool.h>

glCompImage *glCompImageNew(void *par, float x, float y) {
    glCompImage *p = gv_alloc(sizeof(glCompImage));
    glCompInitCommon((glCompObj *) p, par, x, y);
    p->base.objType = glImageObj;
    p->stretch = 0;
    p->texture = NULL;
    p->base.common.functions.draw = glCompImageDraw;
    return p;
}

/* glCompImageNewFile:
 * Creates image from given input file.
 * At present, we assume png input.
 * Return 0 on failure.
 */
glCompImage *glCompImageNewFile(float x, float y, const char *imgfile) {
    int imageWidth, imageHeight;
    unsigned char *data = glCompLoadPng (imgfile, &imageWidth, &imageHeight);
    glCompImage *p;

    if (!data) return NULL;
    p = glCompImageNew(NULL, x, y);
    if (!glCompImageLoad(p, data, imageWidth, imageHeight, 0)) {
	glCompImageDelete (p);
	return NULL;
    }
    return p;
}

void glCompImageDelete(glCompImage * p)
{
    glCompEmptyCommon(&p->base.common);
    if (p->texture)
	glCompDeleteTexture(p->texture);
    free(p);
}

int glCompImageLoad(glCompImage *i, unsigned char *data, int width, int height,
                    bool is2D) {
    if (data != NULL) {		/*valid image data */
	glCompDeleteTexture(i->texture);
	i->texture =
	    glCompSetAddNewTexImage(i->base.common.compset, width, height, data,
				    is2D);
	if (i->texture) {
	    i->base.common.width = width;
	    i->base.common.height = height;
	    return 1;
	}

    }
    return 0;
}

int glCompImageLoadPng(glCompImage *i, const char *pngFile) {
    int imageWidth, imageHeight;
    unsigned char *data;
    data = glCompLoadPng (pngFile, &imageWidth, &imageHeight);
    return glCompImageLoad(i, data, imageWidth, imageHeight, 1);
}

void glCompImageDraw(void *obj)
{
    glCompImage *p = obj;
    glCompCommon ref = p->base.common;
    float w,h,d;

    glCompCalcWidget(p->base.common.parent, &p->base.common, &ref);
    if (!p->base.common.visible)
	return;
    if (!p->texture)
	return;

    if(p->texture->id <=0)
    {
	glRasterPos2f(ref.pos.x, ref.pos.y);
	glDrawPixels(p->texture->width, p->texture->height, GL_RGBA,GL_UNSIGNED_BYTE, p->texture->data);
    }
    else
    {
	w = p->width;
	h = p->height;
	d = (float)p->base.common.layer * GLCOMPSET_BEVEL_DIFF;
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D,p->texture->id);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0f, 1.0f);glVertex3d(ref.pos.x,ref.pos.y,d);
		glTexCoord2d(1.0f, 1.0f);glVertex3d(ref.pos.x+w,ref.pos.y,d);
		glTexCoord2d(1.0f, 0.0f);glVertex3d(ref.pos.x+w,ref.pos.y+h,d);
		glTexCoord2d(0.0f, 0.0f);glVertex3d(ref.pos.x,ref.pos.y+h,d);
	glEnd();


	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
    }

}
