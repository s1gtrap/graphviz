#include	<cdt/dthdr.h>

/*	Return the # of objects in the dictionary
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

static int treecount(Dtlink_t* e)
{	return e ? treecount(e->left) + treecount(e->right) + 1 : 0;
}

int dtsize(Dt_t* dt)
{
	UNFLATTEN(dt);

	if (dt->data.size < 0) // !(dt->data.type & (DT_SET|DT_BAG))
	{	if (dt->data.type & (DT_OSET|DT_OBAG))
			dt->data.size = treecount(dt->data.here);
	}

	return dt->data.size;
}
