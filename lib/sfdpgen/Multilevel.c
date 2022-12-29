/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <sfdpgen/Multilevel.h>
#include <sfdpgen/PriorityQueue.h>
#include <assert.h>
#include <cgraph/alloc.h>
#include <common/arith.h>
#include <stddef.h>
#include <stdbool.h>

Multilevel_control Multilevel_control_new(void) {
  Multilevel_control ctrl = gv_alloc(sizeof(struct Multilevel_control_struct));
  ctrl->minsize = 4;
  ctrl->min_coarsen_factor = 0.75;
  ctrl->maxlevel = 1<<30;

  ctrl->coarsen_mode = COARSEN_MODE_FORCEFUL;
  return ctrl;
}

void Multilevel_control_delete(Multilevel_control ctrl){
  free(ctrl);
}

static Multilevel Multilevel_init(SparseMatrix A, SparseMatrix D, double *node_weights){
  if (!A) return NULL;
  assert(A->m == A->n);
  Multilevel grid = gv_alloc(sizeof(struct Multilevel_struct));
  grid->level = 0;
  grid->n = A->n;
  grid->A = A;
  grid->D = D;
  grid->P = NULL;
  grid->R = NULL;
  grid->node_weights = node_weights;
  grid->next = NULL;
  grid->prev = NULL;
  grid->delete_top_level_A = FALSE;
  return grid;
}

void Multilevel_delete(Multilevel grid){
  if (!grid) return;
  if (grid->A){
    if (grid->level == 0) {
      if (grid->delete_top_level_A) {
	SparseMatrix_delete(grid->A);
	if (grid->D) SparseMatrix_delete(grid->D);
      }
    } else {
      SparseMatrix_delete(grid->A);
      if (grid->D) SparseMatrix_delete(grid->D);
    }
  }
  SparseMatrix_delete(grid->P);
  SparseMatrix_delete(grid->R);
  if (grid->node_weights && grid->level > 0) free(grid->node_weights);
  Multilevel_delete(grid->next);
  free(grid);
}

static void maximal_independent_vertex_set(SparseMatrix A, int **vset, int *nvset, int *nzc){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  (void)n;
  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *vset = gv_calloc(m, sizeof(int));
  for (i = 0; i < m; i++) (*vset)[i] = MAX_IND_VTX_SET_U;
  *nvset = 0;
  *nzc = 0;

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if ((*vset)[i] == MAX_IND_VTX_SET_U){
      (*vset)[i] = (*nvset)++;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	(*vset)[ja[j]] = MAX_IND_VTX_SET_F;
	(*nzc)++;
      }
    }
  }
  free(p);
  (*nzc) += *nvset;
}

static void maximal_independent_vertex_set_RS(SparseMatrix A, int **vset, int *nvset, int *nzc){
  /* The Ruge-Stuben coarsening scheme. Initially all vertices are in the U set (with marker MAX_IND_VTX_SET_U),
     with gain equal to their degree. Select vertex with highest gain into a C set (with
     marker >= MAX_IND_VTX_SET_C), and their neighbors j in F set (with marker MAX_IND_VTX_SET_F). The neighbors of
     j that are in the U set get their gains incremented by 1. So overall 
     gain[k] = |{neighbor of k in U set}|+2*|{neighbors of k in F set}|.
     nzc is the number of entries in the restriction matrix
   */
  int i, jj, ii, *p = NULL, j, k, *ia, *ja, m, n, gain, removed, nf = 0;
  PriorityQueue q;
  (void)removed;
  (void)n;
  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));

  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *vset = gv_calloc(m, sizeof(int));
  for (i = 0; i < m; i++) {
    (*vset)[i] = MAX_IND_VTX_SET_U;
  }
  *nvset = 0;
  *nzc = 0;

  q = PriorityQueue_new(m, 2*(m-1));

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    PriorityQueue_push(q, i, ia[i+1] - ia[i]);
  }
  free(p);

  while (PriorityQueue_pop(q, &i, &gain)){
    assert((*vset)[i] == MAX_IND_VTX_SET_U);
    (*vset)[i] = (*nvset)++; 
    for (j = ia[i]; j < ia[i+1]; j++){
      jj = ja[j];
      assert((*vset)[jj] == MAX_IND_VTX_SET_U || (*vset)[jj] == MAX_IND_VTX_SET_F);
      if (i == jj) continue;
      
      if ((*vset)[jj] == MAX_IND_VTX_SET_U){
	removed = PriorityQueue_remove(q, jj);
	assert(removed);
	(*vset)[jj] = MAX_IND_VTX_SET_F;
	nf++;
	
	for (k = ia[jj]; k < ia[jj+1]; k++){
	  if (jj == ja[k]) continue;
	  if ((*vset)[ja[k]] == MAX_IND_VTX_SET_U){
	    gain = PriorityQueue_get_gain(q, ja[k]);
	    assert(gain >= 0);
	    PriorityQueue_push(q, ja[k], gain + 1);
	  }
	}
      }
      (*nzc)++;
    }
  }
  (*nzc) += *nvset;
  PriorityQueue_delete(q);
  
}

static void maximal_independent_edge_set(SparseMatrix A, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = gv_calloc(m, sizeof(int));
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
        (*matching)[ja[j]] = i;
        (*matching)[i] = ja[j];
        (*nmatch)--;
      }
    }
  }
  free(p);
}

static void maximal_independent_edge_set_heavest_edge_pernode(SparseMatrix A, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  double *a, amax = 0;
  int first = TRUE, jamax = 0;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = gv_calloc(m, sizeof(int));
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  assert(SparseMatrix_is_symmetric(A, false));
  assert(A->type == MATRIX_TYPE_REAL);

  a = A->a;
  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if ((*matching)[i] != i) continue;
    first = TRUE;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
        if (first) {
          amax = a[j];
          jamax = ja[j];
          first = FALSE;
        } else {
          if (a[j] > amax){
            amax = a[j];
            jamax = ja[j];
          }
        }
      }
    }
    if (!first){
      (*matching)[jamax] = i;
      (*matching)[i] = jamax;
      (*nmatch)--;
    }
  }
  free(p);
}





#define node_degree(i) (ia[(i)+1] - ia[(i)])

static void maximal_independent_edge_set_heavest_edge_pernode_leaves_first(SparseMatrix A, int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL, q;
  (void)n;
  double *a, amax = 0;
  int first = TRUE, jamax = 0;
  int *matched, nz, ncmax = 0, nz0, nzz,k ;
  enum {MATCHED = -1};

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = gv_calloc(m, sizeof(int));
  *clusterp = gv_calloc(m + 1, sizeof(int));
  matched = gv_calloc(m, sizeof(int));

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, false));
  assert(A->type == MATRIX_TYPE_REAL);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = A->a;
  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if (matched[i] == MATCHED || node_degree(i) != 1) continue;
    q = ja[ia[i]];
    assert(matched[q] != MATCHED);
    matched[q] = MATCHED;
    (*cluster)[nz++] = q;
    for (j = ia[q]; j < ia[q+1]; j++){
      if (q == ja[j]) continue;
      if (node_degree(ja[j]) == 1){
        matched[ja[j]] = MATCHED;
        (*cluster)[nz++] = ja[j];
      }
    }
    ncmax = MAX(ncmax, nz - (*clusterp)[*ncluster]);
    nz0 = (*clusterp)[*ncluster];
    if (nz - nz0 <= MAX_CLUSTER_SIZE){
      (*clusterp)[++(*ncluster)] = nz;
    } else {
      (*clusterp)[++(*ncluster)] = ++nz0;	
      nzz = nz0;
      for (k = nz0; k < nz && nzz < nz; k++){
        nzz += MAX_CLUSTER_SIZE - 1;
        nzz = MIN(nz, nzz);
        (*clusterp)[++(*ncluster)] = nzz;
      }
    }
  }

 #ifdef DEBUG_print
  if (Verbose)
    fprintf(stderr, "%d leaves and parents for %d clusters, largest cluster = %d\n",nz, *ncluster, ncmax);
#endif
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    first = TRUE;
    if (matched[i] == MATCHED) continue;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
        if (first) {
          amax = a[j];
          jamax = ja[j];
          first = FALSE;
        } else {
          if (a[j] > amax){
            amax = a[j];
            jamax = ja[j];
          }
        }
      }
    }
    if (!first){
        matched[jamax] = MATCHED;
        matched[i] = MATCHED;
        (*cluster)[nz++] = i;
        (*cluster)[nz++] = jamax;
        (*clusterp)[++(*ncluster)] = nz;
    }
  }

  for (i = 0; i < m; i++){
    if (matched[i] == i){
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }

  free(p);

  free(matched);
}

static void maximal_independent_edge_set_heavest_edge_pernode_supernodes_first(SparseMatrix A, int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  (void)n;
  double *a, amax = 0;
  int first = TRUE, jamax = 0;
  int *matched, nz, nz0;
  enum {MATCHED = -1};
  int  nsuper, *super = NULL, *superp = NULL;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = gv_calloc(m, sizeof(int));
  *clusterp = gv_calloc(m + 1, sizeof(int));
  matched = gv_calloc(m, sizeof(int));

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, false));
  assert(A->type == MATRIX_TYPE_REAL);

  SparseMatrix_decompose_to_supervariables(A, &nsuper, &super, &superp);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = A->a;

  for (i = 0; i < nsuper; i++){
    if (superp[i+1] - superp[i] <= 1) continue;
    nz0 = (*clusterp)[*ncluster];
    for (j = superp[i]; j < superp[i+1]; j++){
      matched[super[j]] = MATCHED;
      (*cluster)[nz++] = super[j];
      if (nz - nz0 >= MAX_CLUSTER_SIZE){
	(*clusterp)[++(*ncluster)] = nz;
	nz0 = nz;
      }
    }
    if (nz > nz0) (*clusterp)[++(*ncluster)] = nz;
  }

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    first = TRUE;
    if (matched[i] == MATCHED) continue;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
        if (first) {
          amax = a[j];
          jamax = ja[j];
          first = FALSE;
        } else {
          if (a[j] > amax){
            amax = a[j];
            jamax = ja[j];
          }
        }
      }
    }
    if (!first){
        matched[jamax] = MATCHED;
        matched[i] = MATCHED;
        (*cluster)[nz++] = i;
        (*cluster)[nz++] = jamax;
        (*clusterp)[++(*ncluster)] = nz;
    }
  }

  for (i = 0; i < m; i++){
    if (matched[i] == i){
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }
  free(p);

  free(super);

  free(superp);

  free(matched);
}

static int scomp(const void *s1, const void *s2){
  const double *ss1 = s1;
  const double *ss2 = s2;

  if (ss1[1] > ss2[1]){
    return -1;
  } else if (ss1[1] < ss2[1]){
    return 1;
  }
  return 0;
}

static void maximal_independent_edge_set_heavest_cluster_pernode_leaves_first(SparseMatrix A, int csize, 
									      int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL, q, iv;
  (void)n;
  double *a;
  int *matched, nz,  nz0, nzz,k, nv;
  enum {MATCHED = -1};
  double *vlist;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = gv_calloc(m, sizeof(int));
  *clusterp = gv_calloc(m + 1, sizeof(int));
  matched = gv_calloc(m, sizeof(int));
  vlist = gv_calloc(2 * m, sizeof(double));

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, false));
  assert(A->type == MATRIX_TYPE_REAL);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = A->a;

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if (matched[i] == MATCHED || node_degree(i) != 1) continue;
    q = ja[ia[i]];
    assert(matched[q] != MATCHED);
    matched[q] = MATCHED;
    (*cluster)[nz++] = q;
    for (j = ia[q]; j < ia[q+1]; j++){
      if (q == ja[j]) continue;
      if (node_degree(ja[j]) == 1){
	matched[ja[j]] = MATCHED;
	(*cluster)[nz++] = ja[j];
      }
    }
    nz0 = (*clusterp)[*ncluster];
    if (nz - nz0 <= MAX_CLUSTER_SIZE){
      (*clusterp)[++(*ncluster)] = nz;
    } else {
      (*clusterp)[++(*ncluster)] = ++nz0;	
      nzz = nz0;
      for (k = nz0; k < nz && nzz < nz; k++){
	nzz += MAX_CLUSTER_SIZE - 1;
	nzz = MIN(nz, nzz);
	(*clusterp)[++(*ncluster)] = nzz;
      }
    }
  }
  
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if (matched[i] == MATCHED) continue;
    nv = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	vlist[2*nv] = ja[j];
	vlist[2*nv+1] = a[j];
	nv++;
      }
    }
    if (nv > 0){
      qsort(vlist, nv, sizeof(double)*2, scomp);
      for (j = 0; j < MIN(csize - 1, nv); j++){
	iv = (int) vlist[2*j];
	matched[iv] = MATCHED;
	(*cluster)[nz++] = iv;
      }
      matched[i] = MATCHED;
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }
  
  for (i = 0; i < m; i++){
    if (matched[i] == i){
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }
  free(p);


  free(matched);
}

static void maximal_independent_edge_set_heavest_edge_pernode_scaled(SparseMatrix A, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  double *a, amax = 0;
  int first = TRUE, jamax = 0;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = gv_calloc(m, sizeof(int));
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  assert(SparseMatrix_is_symmetric(A, false));
  assert(A->type == MATRIX_TYPE_REAL);

  a = A->a;
  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if ((*matching)[i] != i) continue;
    first = TRUE;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
        if (first) {
          amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
          jamax = ja[j];
          first = FALSE;
        } else {
          if (a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]) > amax){
            amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
            jamax = ja[j];
          }
        }
      }
    }
    if (!first){
        (*matching)[jamax] = i;
        (*matching)[i] = jamax;
        (*nmatch)--;
    }
  }
  free(p);
}

static void Multilevel_coarsen_internal(SparseMatrix A, SparseMatrix *cA, SparseMatrix D, SparseMatrix *cD,
					double *node_wgt, double **cnode_wgt,
					SparseMatrix *P, SparseMatrix *R, Multilevel_control ctrl){
  int *matching = NULL, nc, nzc, n, i;
  int *irn = NULL, *jcn = NULL;
  double *val = NULL;
  SparseMatrix B = NULL;
  int *vset = NULL, j;
  int *cluster=NULL, *clusterp=NULL, ncluster;

  assert(A->m == A->n);
  *cA = NULL;
  *cD = NULL;
  *P = NULL;
  *R = NULL;
  n = A->m;

  maximal_independent_edge_set_heavest_edge_pernode_supernodes_first(A, &cluster, &clusterp, &ncluster);
  assert(ncluster <= n);
  nc = ncluster;
  if ((ctrl->coarsen_mode == COARSEN_MODE_GENTLE && nc > ctrl->min_coarsen_factor*n) || nc == n || nc < ctrl->minsize) {
#ifdef DEBUG_PRINT
    if (Verbose)
      fprintf(stderr, "nc = %d, nf = %d, minsz = %d, coarsen_factor = %f coarsening stops\n",nc, n, ctrl->minsize, ctrl->min_coarsen_factor);
#endif
    goto RETURN;
  }
  irn = gv_calloc(n, sizeof(int));
  jcn = gv_calloc(n, sizeof(int));
  val = gv_calloc(n, sizeof(double));
  nzc = 0; 
  for (i = 0; i < ncluster; i++){
    for (j = clusterp[i]; j < clusterp[i+1]; j++){
      assert(clusterp[i+1] > clusterp[i]);
      irn[nzc] = cluster[j];
      jcn[nzc] = i;
      val[nzc++] = 1.;
   }
  }
  assert(nzc == n);
  *P = SparseMatrix_from_coordinate_arrays(nzc, n, nc, irn, jcn, val,
                                           MATRIX_TYPE_REAL, sizeof(double));
  *R = SparseMatrix_transpose(*P);

  *cD = NULL;

  *cA = SparseMatrix_multiply3(*R, A, *P); 

  /*
    B = SparseMatrix_multiply(*R, A);
    if (!B) goto RETURN;
    *cA = SparseMatrix_multiply(B, *P); 
    */
  if (!*cA) goto RETURN;

  SparseMatrix_multiply_vector(*R, node_wgt, cnode_wgt);
  *R = SparseMatrix_divide_row_by_degree(*R);
  SparseMatrix_set_symmetric(*cA);
  SparseMatrix_set_pattern_symmetric(*cA);
  *cA = SparseMatrix_remove_diagonal(*cA);

 RETURN:
  free(matching);
  free(vset);
  free(irn);
  free(jcn);
  free(val);
  if (B) SparseMatrix_delete(B);

  free(cluster);
  free(clusterp);
}

void Multilevel_coarsen(SparseMatrix A, SparseMatrix *cA, SparseMatrix D, SparseMatrix *cD, double *node_wgt, double **cnode_wgt,
			       SparseMatrix *P, SparseMatrix *R, Multilevel_control ctrl){
  SparseMatrix cA0 = A,  cD0 = NULL, P0 = NULL, R0 = NULL, M;
  double *cnode_wgt0 = NULL;
  int nc = 0, n;
  
  *P = NULL; *R = NULL; *cA = NULL; *cnode_wgt = NULL, *cD = NULL;

  n = A->n;

  do {/* this loop force a sufficient reduction */
    node_wgt = cnode_wgt0;
    Multilevel_coarsen_internal(A, &cA0, D, &cD0, node_wgt, &cnode_wgt0, &P0, &R0, ctrl);
    if (!cA0) return;
    nc = cA0->n;
#ifdef DEBUG_PRINT
    if (Verbose) fprintf(stderr,"nc=%d n = %d\n",nc,n);
#endif
    if (*P){
      assert(*R);
      M = SparseMatrix_multiply(*P, P0);
      SparseMatrix_delete(*P);
      SparseMatrix_delete(P0);
      *P = M;
      M = SparseMatrix_multiply(R0, *R);
      SparseMatrix_delete(*R);
      SparseMatrix_delete(R0);
      *R = M;
    } else {
      *P = P0;
      *R = R0;
    }

    if (*cA) SparseMatrix_delete(*cA);
    *cA = cA0;
    if (*cD) SparseMatrix_delete(*cD);
    *cD = cD0;

    if (*cnode_wgt) free(*cnode_wgt);
    *cnode_wgt = cnode_wgt0;
    A = cA0;
    D = cD0;
    node_wgt = cnode_wgt0;
    cnode_wgt0 = NULL;
  } while (nc > ctrl->min_coarsen_factor*n && ctrl->coarsen_mode ==  COARSEN_MODE_FORCEFUL);

}

void print_padding(int n){
  int i;
  for (i = 0; i < n; i++) fputs (" ", stderr);
}
static Multilevel Multilevel_establish(Multilevel grid, Multilevel_control ctrl){
  Multilevel cgrid;
  double *cnode_weights = NULL;
  SparseMatrix P, R, A, cA, D, cD;

#ifdef DEBUG_PRINT
  if (Verbose) {
    print_padding(grid->level);
    fprintf(stderr, "level -- %d, n = %d, nz = %d nz/n = %f\n", grid->level, grid->n, grid->A->nz, grid->A->nz/(double) grid->n);
  }
#endif
  A = grid->A;
  D = grid->D;
  if (grid->level >= ctrl->maxlevel - 1) {
#ifdef DEBUG_PRINT
  if (Verbose) {
    print_padding(grid->level);
    fprintf(stderr, " maxlevel reached, coarsening stops\n");
  }
#endif
    return grid;
  }
  Multilevel_coarsen(A, &cA, D, &cD, grid->node_weights, &cnode_weights, &P, &R, ctrl);
  if (!cA) return grid;

  cgrid = Multilevel_init(cA, cD, cnode_weights);
  grid->next = cgrid;
  cgrid->coarsen_scheme_used = COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_SUPERNODES_FIRST;
  cgrid->level = grid->level + 1;
  cgrid->n = cA->m;
  cgrid->A = cA;
  cgrid->D = cD;
  cgrid->P = P;
  grid->R = R;
  cgrid->prev = grid;
  cgrid = Multilevel_establish(cgrid, ctrl);
  return grid;
  
}

Multilevel Multilevel_new(SparseMatrix A0, SparseMatrix D0, Multilevel_control ctrl){
  /* A: the weighting matrix. D: the distance matrix, could be NULL. If not null, the two matrices must have the same sparsity pattern */
  Multilevel grid;
  SparseMatrix A = A0, D = D0;

  if (!SparseMatrix_is_symmetric(A, false) || A->type != MATRIX_TYPE_REAL){
    A = SparseMatrix_get_real_adjacency_matrix_symmetrized(A);
  }
  if (D && (!SparseMatrix_is_symmetric(D, false) || D->type != MATRIX_TYPE_REAL)){
    D = SparseMatrix_symmetrize_nodiag(D);
  }
  grid = Multilevel_init(A, D, NULL);
  grid = Multilevel_establish(grid, ctrl);
  if (A != A0) grid->delete_top_level_A = TRUE;/* be sure to clean up later */
  return grid;
}


Multilevel Multilevel_get_coarsest(Multilevel grid){
  while (grid->next){
    grid = grid->next;
  }
  return grid;
}

