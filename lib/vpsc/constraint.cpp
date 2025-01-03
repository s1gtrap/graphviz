/**
 * \brief A constraint determines a minimum or exact spacing required between
 * two variables.
 *
 * Authors:
 *   Tim Dwyer <tgdwyer@gmail.com>
 *
 * Copyright (C) 2005 Authors
 *
 * This version is released under the CPL (Common Public License) with
 * the Graphviz distribution.
 * A version is also available under the LGPL as part of the Adaptagrams
 * project: https://github.com/mjwybrow/adaptagrams.  
 * If you make improvements or bug fixes to this code it would be much
 * appreciated if you could also contribute those changes back to the
 * Adaptagrams repository.
 */

#include <vpsc/constraint.h>
#include <cassert>
Constraint::Constraint(Variable *left_, Variable *right_, double gap_)
    : left(left_), right(right_), gap(gap_), timeStamp(0), active(false),
      visited(false) {
	left->out.push_back(this);
	right->in.push_back(this);
}
Constraint::~Constraint() {
	Constraints::iterator i;
	for(i=left->out.begin(); i!=left->out.end(); i++) {
		if(*i==this) break;
	}
	left->out.erase(i);
	for(i=right->in.begin(); i!=right->in.end(); i++) {
		if(*i==this) break;
	}
	right->in.erase(i);
}
std::ostream& operator <<(std::ostream &os, const Constraint &c)
{
	os<<*c.left<<"+"<<c.gap<<"<="<<*c.right<<"("<<c.slack()<<")"<<(c.active?"-active":"");
	return os;
}
