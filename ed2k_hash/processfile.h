/***************************************************************************
                          processfile.h  -  description
                             -------------------
    begin                : Tue Sep 3 2002
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

#ifndef _ed2k_hash_processfile_h_included_
#define _ed2k_hash_processfile_h_included_

#define BLOCKSIZE	(9500*1024)

#ifdef __WIN32
# define SLASH_CHAR '\\'
#else
# define SLASH_CHAR	'/'
#endif /* ifdef __WIN32 */

struct _fileinfo
{
	char			*filepath;
	char			*basename;			/* the filename w/o path component	*/

	unsigned int	 blocks;
	unsigned char	*parthashes;		/* the partial hashes in a row		*/

	unsigned char	*ed2k_hash;			/* the ed2k_hash in binary form		*/
	char			*ed2k_hash_str;		/* the ed2k_hash in ASCII form		*/

	void			*user_data;			/* extra data GUIs might set		*/

	unsigned int	 size;				/* the filesize in bytes			*/
} ;

typedef struct _fileinfo fileinfo;


int		process_file (const char *fn, fileinfo *info);
void	process_file_free_info_structure_content(fileinfo *info);


#endif /* ifndef _ed2k_hash_processfile_h_included_ */



