/***************************************************************************
                          processfile.c  -  description
                             -------------------
    begin                : Tue Sep 3 2002
    copyright            : (C) 2002 by Tim-Philipp Müller
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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#undef ED2K_HASH_USE_MMAP

/* for nmap on unix systems             */
/* TODO: add other *nix OS defines here */
#if (defined(__linux__) || defined(__MAC_OS_X__) || defined(__FreeBSD__))
#  include <unistd.h>
#  include <sys/mman.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef _POSIX_MAPPED_FILES
#    define ED2K_HASH_USE_MMAP	1
#  endif
#endif

#include "md4.h"
#include "options.h"
#include "processfile.h"
#include "ui.h"

/* functions */
static unsigned int			get_file_size (const char *fn);

#ifndef ED2K_HASH_USE_MMAP
static int					process_one_block (fileinfo *fi, unsigned int b);
#endif


/* process_file
 *
 * gets the file size of the file and hashes it.
 *
 * If info is set, it will fill info
 *   with a fileinfo structure (which is undefined on error)
 *
 * The function will regularly call ui_update(), and will
 *   stop hashing if ui_update() returns a negative value.
 *
 * returns 0 on error, otherwise 1
 *
 */

#ifdef ED2K_HASH_USE_MMAP

/************************** version with mmap() ********************************/
/* version using mmap() by Thomas Lussnig <thomas.lussnig@bewegungsmelder.de> */

int
process_file (const char *fn, fileinfo *info)
{
	const char		hexdigits[16] = "0123456789abcdef";
	unsigned int	b, j;
	fileinfo		fi;
	int				fd;

	if (!fn)
		return 0;

	memset (&fi,0x00,sizeof(fi));

	/* get filesize */
	fi.size = get_file_size (fn);
	if (fi.size == (unsigned int)-1)	/* error getting filesize? */
	{
		ui_printerr ("error getting filesize ('%s')?\n", fn);
		return 0;
	}

	if (fi.size == 0)					/* a filesize of 0 bytes? */
	{
		ui_printerr ("filesize of 0 bytes ('%s')?\n", fn);
		return 0;
	}

	fi.filepath = strdup(fn);

	if (!strrchr(fn,SLASH_CHAR))		/* get the basename without path components */
	{
		fi.basename = strdup(fn);
	} else fi.basename = strdup(strrchr(fn,SLASH_CHAR)+1);

	fi.blocks = fi.size/BLOCKSIZE;
	if (fi.size % BLOCKSIZE > 0)
		fi.blocks++;

	fi.ed2k_hash = (unsigned char*) malloc(16);
	fi.ed2k_hash_str = (char*) malloc(33);
	fi.parthashes = (unsigned char*) malloc(fi.blocks*16);	/* 16 bytes for each block's hash */

	if ((!fi.parthashes) || (!fi.ed2k_hash) || (!fi.ed2k_hash_str) || (!fi.filepath))
		return 0;

	if ((fd = open (fi.filepath, O_RDONLY)) < 0)
	{
		ui_printerr ("in %s - fopen(%s) failed: %s\n\n", __FUNCTION__, fi.filepath, strerror(errno));
		return 0;
	}

	for (b=0; b < fi.blocks; b++)
	{
		MD4_CTX			 context;
		unsigned char	 hash[16];
		int				 len, start;
		void			*map;

		len =  BLOCKSIZE;
		if (b == fi.blocks - 1)
			len = fi.size % BLOCKSIZE;

		if (len==0)	/* this case shouldn't happen */
			return 0;

		start = b*BLOCKSIZE;
		map = mmap (NULL, len, PROT_READ, MAP_SHARED, fd, b*BLOCKSIZE);
		if (map == NULL)
		{
			ui_printerr ("in %s - mmap() failed: %s\n", __FUNCTION__, strerror(errno));
			return 0;
		}
		MD4_Init   (&context);
		MD4_Update (&context, map, len);
		MD4_Final  (fi.parthashes+(b*16), &context);
		munmap (map, len);
		if ((option_debug)||(option_verbose))
		{
			int k;
			ui_print ("partial hash block %3u: ", b);
			for (k=0; k<16; k++)
				ui_print ("%02hx", (fi.parthashes+(b*16))[k]);
			ui_print ("\n");
		}
	} /* end of: for (b=0; b < fi.blocks; b++) */

	close(fd);

	/* if only one block: partial hash == final hash */
	if (fi.blocks>1)
	{
		MD4_CTX	context;
		MD4_Init (&context);
		MD4_Update (&context, fi.parthashes, 16*b);
		MD4_Final (fi.ed2k_hash, &context);
	} else memcpy (fi.ed2k_hash, fi.parthashes, 16);


	/* hash to hash string: */
	memset (fi.ed2k_hash_str, 0x00, 33*sizeof(char));
	for (j=0; j<16; j++)
	{
		fi.ed2k_hash_str[(j<<1)  ] = hexdigits[(((fi.ed2k_hash[j]) & 0xf0) >> 4)];
		fi.ed2k_hash_str[(j<<1)+1] = hexdigits[(((fi.ed2k_hash[j]) & 0x0f)     )];
	}

	if (info)
		memcpy (info, &fi, sizeof(fi));

	return 1;
}



#else



/************************** version without mmap() *****************************/

int
process_file (const char *fn, fileinfo *info)
{
	const char		hexdigits[16] = "0123456789abcdef";
	unsigned int	b, j;
	fileinfo		fi;

	if (!fn)
		return 0;

	memset (&fi,0x00,sizeof(fi));

	/* get filesize */
	fi.size = get_file_size (fn);
	if (fi.size == (unsigned int)-1)	/* error getting filesize? */
	{
		ui_printerr ("error getting filesize ('%s')?\n", fn);
		return 0;
	}

	if (fi.size == 0)					/* a filesize of 0 bytes? */
	{
		ui_printerr ("filesize of 0 bytes ('%s')?\n", fn);
		return 0;
	}

	fi.filepath = strdup(fn);

	if (!strrchr(fn,SLASH_CHAR))		/* get the basename without path components */
	{
		fi.basename = strdup(fn);
	} else fi.basename = strdup(strrchr(fn,SLASH_CHAR)+1);

	fi.blocks = fi.size/BLOCKSIZE;
	if (fi.size % BLOCKSIZE > 0)
		fi.blocks++;

	fi.ed2k_hash = (unsigned char*) malloc(16);
	fi.ed2k_hash_str = (char*) malloc(33);
	fi.parthashes = (unsigned char*) malloc(fi.blocks*16);	/* 16 bytes for each block's hash */

	if ((!fi.parthashes) || (!fi.ed2k_hash) || (!fi.ed2k_hash_str))
		return 0;

	for (b=0; b < fi.blocks; b++)
	{
		if (!process_one_block (&fi, b))
			return 0;
	}	

	/* if only one block: partial hash == final hash */
	if (fi.blocks>1)
	{
		MD4_CTX	context;
		MD4_Init (&context);
		MD4_Update (&context, fi.parthashes, 16*b);
		MD4_Final (fi.ed2k_hash, &context);
	} else memcpy (fi.ed2k_hash, fi.parthashes, 16);


	/* hash to hash string: */
	memset (fi.ed2k_hash_str, 0x00, 33*sizeof(char));
	for (j=0; j<16; j++)
	{
		fi.ed2k_hash_str[(j<<1)  ] = hexdigits[(((fi.ed2k_hash[j]) & 0xf0) >> 4)];
		fi.ed2k_hash_str[(j<<1)+1] = hexdigits[(((fi.ed2k_hash[j]) & 0x0f)     )];
	}

	if (info)
		memcpy (info, &fi, sizeof(fi));

	return 1;
}


/* process_one_block
 *
 * hashes one block of 9500kB of the file and
 * calls ui_update() in between.
 *
 * returns 0 on error, otherwise 1
 *
 */

static int
process_one_block (fileinfo *fi, unsigned int b)
{
	MD4_CTX			 context;
	FILE			*f;
	unsigned int	 blocksize = BLOCKSIZE;
	unsigned int	 left;
	unsigned char	 hash[16];

	if ((!fi) || (!fi->parthashes) || (!fi->filepath))
		return 0;

	f = fopen (fi->filepath, "rb");
	if (!f)
	{
		ui_printerr ("in %s - fopen(%s) failed: %s\n\n", __FUNCTION__, fi->filepath, strerror(errno));
		return 0;
	}

	if (fseek(f, b*BLOCKSIZE, SEEK_SET)<0)
	{
		ui_printerr ("in %s - fseek() failed: %s\n", __FUNCTION__, strerror(errno));
		return 0;
	}

	if (b == fi->blocks - 1)
		blocksize = fi->size % BLOCKSIZE;

	if (blocksize==0)	/* this case shouldn't happen */
		return 0;

	left = blocksize;
	MD4_Init (&context);
	while (left>0)
	{
		unsigned char	buf[4*1024];
		int 			ret;
		size_t			readnow = (left>=sizeof(buf)) ? sizeof(buf) : left;

		ret = fread(buf, readnow, 1, f);
		if (ret!=1)
		{
			ui_printerr ("in %s - fread(%s) failed: %s\n", __FUNCTION__, fi->filepath, strerror(errno));
			fclose(f);
			return 0;
		}

		MD4_Update (&context, (void*)buf, readnow);

		left -= readnow;

		if (!ui_update (fi->filepath, fi->size, fi->size-left))
			return 0;

	}

	MD4_Final (hash, &context);
	memcpy (fi->parthashes+(b*16), hash, 16);

	if ((option_debug)||(option_verbose))
	{
		int k;
		ui_print ("partial hash block %3u: ", b);
		for (k=0; k<16; k++)
			ui_print ("%02hx", hash[k]);
		ui_print ("\n");
	}

	fclose(f);
	return 1;
}


#endif /* ifdef ED2K_HASH_USE_MMAP ... else ...*/



/* get_file_size
 *
 * unelegant but portable way to get the size of a file
 *
 * returns (unsigned int)-1 on error, otherwise filesize
 *
 */

static unsigned int
get_file_size (const char *fn)
{
	FILE			*file;
	unsigned long	 fsize;

	if (!fn)
		return (unsigned int)-1;

	file = fopen(fn, "rb");
	if (!file)
	{
		ui_printerr ("in fopen(%s) - %s\n\n", fn, strerror(errno));
		return (unsigned int)-1;
	}

	if (fseek(file, 0, SEEK_END)<0)		/* go to very last byte of file */
		return (unsigned int)-1;

	fsize = ftell(file);				/* and tell us which no. that is */
	if (fsize == (unsigned long) -1)
		return (unsigned int)-1;

	fclose(file);

	if (fsize > (unsigned long)((unsigned int)-1))
	{
		ui_printerr ("the file %s is too big. The donkey can only deal with files smaller than 4GB!\n\n", fn);
		return ((unsigned int)-1);
	}

	return (unsigned int)fsize;
}






