/***************************************************************************
                          linkedlist.h  -  description
                             -------------------
    begin                : Mon Sep 2 2002
    copyright            : (C) 2002 by Tim-Philipp M�ller
    email                : t.i.m@orange.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ed2k_hash_linkedlist_h_included_
#define _ed2k_hash_linkedlist_h_included_

/* a single linked list */

struct _SList
{
	void	*next;
	void	*data;
	void	*last;
} ;

typedef struct _SList SList;

SList			*slist_append (SList *list, void *data);
unsigned int	 slist_length (SList *list);

#endif /* ifndef _ed2k_hash_linkedlist_h_included_ */


