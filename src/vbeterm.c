/* -*- mode: c; c-file-style: "openbsd" -*- */
/* TODO:5002 You may want to change the copyright of all files. This is the
 * TODO:5002 ISC license. Choose another one if you want.
 */
/*
 * Copyright (c) 2014 Vincent Bernat <bernat@luffy.cx>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "vbeterm.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <vte/vte.h>

int
main(int argc, char *argv[])
{
	GtkWidget *window, *terminal;
	GdkGeometry geo_hints;

	/* Initialise GTK and the widgets */
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	terminal = vte_terminal_new();
	gtk_window_set_title(GTK_WINDOW(window), "vbeterm");

	/* Set geo hint because this is a terminal */
	geo_hints.base_width = vte_terminal_get_char_width(VTE_TERMINAL(terminal));
	geo_hints.base_height = vte_terminal_get_char_height(VTE_TERMINAL(terminal));
	geo_hints.min_width = vte_terminal_get_char_width(VTE_TERMINAL(terminal));
	geo_hints.min_height = vte_terminal_get_char_height(VTE_TERMINAL(terminal));
	geo_hints.width_inc = vte_terminal_get_char_width(VTE_TERMINAL(terminal));
	geo_hints.height_inc = vte_terminal_get_char_height(VTE_TERMINAL(terminal));
	gtk_window_set_geometry_hints(GTK_WINDOW(window), terminal, &geo_hints,
	    GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

	/* Start a new shell */
	vte_terminal_fork_command(VTE_TERMINAL (terminal),
	    NULL,		/* binary to run (NULL=user's shell) */
	    NULL,		/* arguments */
	    NULL,		/* environment */
	    NULL,		/* dir to start (NULL=CWD) */
	    FALSE,		/* log session to lastlog */
	    FALSE,		/* log session to utmp/utmpx log */
	    FALSE);		/* log session to wtmp/wtmpx log */

	/* Connect some signals */
	g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
	g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);

	/* Configure terminal */
	vte_terminal_set_word_chars(VTE_TERMINAL(terminal),
	    VBETERM_WORD_CHARS);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal),
	    VBETERM_SCROLLBACK_LINES);
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal),
	    FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal),
	    TRUE);

#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_16(x)  (((x) << 8) | (x))
#define CLR_GDK(x) (const GdkColor){ 0, CLR_16(CLR_R(x)), CLR_16(CLR_G(x)), CLR_16(CLR_B(x)) }

	vte_terminal_set_colors(VTE_TERMINAL(terminal),
	    &CLR_GDK(0xffffff),
	    &CLR_GDK(0),
	    (const GdkColor[]){	CLR_GDK(0x111111),
			CLR_GDK(0xd36265),
			CLR_GDK(0xaece91),
			CLR_GDK(0xe7e18c),
			CLR_GDK(0x5297cf),
			CLR_GDK(0x963c59),
			CLR_GDK(0x5E7175),
			CLR_GDK(0xbebebe),
			CLR_GDK(0x666666),
			CLR_GDK(0xef8171),
			CLR_GDK(0xcfefb3),
			CLR_GDK(0xfff796),
			CLR_GDK(0x74b8ef),
			CLR_GDK(0xb85e7b),
			CLR_GDK(0xA3BABF),
			CLR_GDK(0xffffff)
	    }, 16);
	vte_terminal_set_opacity(VTE_TERMINAL(terminal),
	    (1 - VBETERM_SATURATION_LEVEL) * 65535);

	/* Pack widgets and start the terminal */
	gtk_container_add(GTK_CONTAINER(window), terminal);
	gtk_widget_show_all(window);
	gtk_main();
}
