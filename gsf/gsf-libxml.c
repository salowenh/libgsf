/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gsf-libxml.c :
 *
 * Copyright (C) 2002 Jody Goldberg (jody@gnome.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <gsf-config.h>
#include <gsf/gsf-libxml.h>
#include <gsf/gsf-input.h>

static int
gsf_libxml_read (void * context, char * buffer, int len)
{
	if (NULL == gsf_input_read ((GsfInput *)context, (unsigned)len, buffer))
		return -1;
	return len;
}

static int
gsf_libxml_close_read (void * context)
{
	g_object_unref (G_OBJECT (context));
	return TRUE;
}

xmlParserCtxtPtr
gsf_xml_parser_context (GsfInput *input)
{
	xmlParserCtxtPtr context = xmlCreateIOParserCtxt (
		NULL, NULL,
		(xmlInputReadCallback) gsf_libxml_read, 
		(xmlInputCloseCallback) gsf_libxml_close_read,
		input, XML_CHAR_ENCODING_NONE);

	if (context != NULL)
		g_object_ref (G_OBJECT (input));
	return context;
}
