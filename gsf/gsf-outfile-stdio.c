/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gsf-outfile-stdio.c: A directory tree wrapper for Outfile
 *
 * Copyright (C) 2004 Novell, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <gsf-config.h>
#include <gsf/gsf-outfile-impl.h>
#include <gsf/gsf-outfile-stdio.h>
#include <gsf/gsf-output-impl.h>
#include <gsf/gsf-output-stdio.h>
#include <gsf/gsf-impl-utils.h>
#include <gsf/gsf-utils.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

GObjectClass *parent_class;

struct _GsfOutfileStdio {
	GsfOutfile parent;
	char      *root;
};
typedef GsfOutfileClass GsfOutfileStdioClass;

static void
gsf_outfile_stdio_finalize (GObject *obj)
{
	GsfOutfileStdio *ofs = GSF_OUTFILE_STDIO (obj);
	g_free (ofs->root);
	parent_class->finalize (obj);
}

static GsfOutput *
gsf_outfile_stdio_new_child (GsfOutfile *parent, char const *name,
			     gboolean is_dir)
{
	GsfOutfileStdio *ofs = GSF_OUTFILE_STDIO (parent);
	GsfOutput *child;
	char *path = g_build_filename (ofs->root, name, NULL);

	if (is_dir)
		child = (GsfOutput *) gsf_outfile_stdio_new (path, NULL);
	else
		child = (GsfOutput *) gsf_output_stdio_new (path, NULL);
	g_free (path);

	return child;
}

static gboolean
gsf_outfile_stdio_close (G_GNUC_UNUSED GsfOutput *output)
{
	return TRUE;
}

static void
gsf_outfile_stdio_init (GsfOutfileStdio *ofs)
{
	ofs->root = NULL;
}

static void
gsf_outfile_stdio_class_init (GObjectClass *gobject_class)
{
	GsfOutputClass *output_class = GSF_OUTPUT_CLASS (gobject_class);
	GsfOutfileClass *outfile_class = GSF_OUTFILE_CLASS (gobject_class);

	parent_class = g_type_class_peek (GSF_OUTFILE_TYPE);

	gobject_class->finalize = gsf_outfile_stdio_finalize;

	output_class->Close	= gsf_outfile_stdio_close;
	output_class->Seek	= NULL;
	output_class->Write	= NULL;
	output_class->Vprintf	= NULL;

	outfile_class->new_child = gsf_outfile_stdio_new_child;
}

GSF_CLASS (GsfOutfileStdio, gsf_outfile_stdio,
	   gsf_outfile_stdio_class_init, gsf_outfile_stdio_init,
	   GSF_OUTFILE_TYPE)

/**
 * gsf_outfile_stdio_new :
 * @filename : in utf8.
 * @err	     : optionally NULL.
 *
 * Returns a new outfile or NULL.
 **/
GsfOutfileStdio *
gsf_outfile_stdio_new (char const *root, GError **err)
{
	GsfOutfileStdio *ofs;
	
	if (0 != mkdir (root, 0777)) {
		if (err != NULL) {
			char *utf8name = gsf_filename_to_utf8 (root, FALSE);
			*err = g_error_new (gsf_output_error_id (), 0,
				"%s: %s", utf8name, g_strerror (errno));
			g_free (utf8name);
		}
		return NULL;
	}

	ofs = g_object_new (gsf_outfile_stdio_get_type(), NULL);
	ofs->root = g_strdup (root);
	return ofs;
}
