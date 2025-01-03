#include	<cdt/dthdr.h>
#include	<stddef.h>

/*	Renew the object at the current finger.
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

void* dtrenew(Dt_t* dt, void* obj)
{
	void*	key;
	Dtlink_t	*e, *t, **s;
	Dtdisc_t*	disc = dt->disc;

	UNFLATTEN(dt);

	if (!(e = dt->data.here) || _DTOBJ(e,disc->link) != obj)
		return NULL;

	if (dt->data.type & (DT_OSET|DT_OBAG))
	{	if(!e->right )	/* make left child the new root */
			dt->data.here = e->left;
		else		/* make right child the new root */
		{	dt->data.here = e->right;

			/* merge left subtree to right subtree */
			if(e->left)
			{	for(t = e->right; t->left; t = t->left)
					;
				t->left = e->left;
			}
		}
	}
	else // if (dt.data->type & (DT_SET|DT_BAG))
	{	s = dt->data.htab + HINDEX(dt->data.ntab, e->hash);
		if((t = *s) == e)
			*s = e->right;
		else
		{	for(; t->right != e; t = t->right)
				;
			t->right = e->right;
		}
		key = _DTKEY(obj,disc->key,disc->size);
		e->hash = dtstrhash(key, disc->size);
		dt->data.here = NULL;
	}

	--dt->data.size;
	return dt->meth->searchf(dt, e, DT_RENEW) ? obj : NULL;
}
