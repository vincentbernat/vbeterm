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
	vte_terminal_fork_command_full(VTE_TERMINAL (terminal),
	    VTE_PTY_DEFAULT,
	    NULL,		/* working directory */
	    (char *[]){ g_strdup(g_getenv("SHELL")), 0 },
	    NULL,		/* envv */
	    0,			/* spawn flags */
	    NULL, NULL,		/* child setup */
	    NULL,		/* child pid */
	    NULL);

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
	vte_terminal_set_rewrap_on_resize(VTE_TERMINAL(terminal),
	    TRUE);

#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_GDK(x, a) (const GdkRGBA){ CLR_R(x)/255., CLR_G(x)/255., CLR_B(x)/255., a }
	vte_terminal_set_colors_rgba(VTE_TERMINAL(terminal),
	    &CLR_GDK(0xffffff, 1),
	    &CLR_GDK(0, VBETERM_OPACITY),
	    (const GdkRGBA[]){	CLR_GDK(0x111111, 1),
			CLR_GDK(0xd36265, 1),
			CLR_GDK(0xaece91, 1),
			CLR_GDK(0xe7e18c, 1),
			CLR_GDK(0x5297cf, 1),
			CLR_GDK(0x963c59, 1),
			CLR_GDK(0x5E7175, 1),
			CLR_GDK(0xbebebe, 1),
			CLR_GDK(0x666666, 1),
			CLR_GDK(0xef8171, 1),
			CLR_GDK(0xcfefb3, 1),
			CLR_GDK(0xfff796, 1),
			CLR_GDK(0x74b8ef, 1),
			CLR_GDK(0xb85e7b, 1),
			CLR_GDK(0xA3BABF, 1),
			CLR_GDK(0xffffff, 1)
	    }, 16);
	vte_terminal_set_color_cursor_rgba(VTE_TERMINAL(terminal),
	    &CLR_GDK(0x007700, 1));
	vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal),
	    VTE_CURSOR_BLINK_OFF);
	vte_terminal_set_allow_bold(VTE_TERMINAL(terminal),
	    TRUE);

	vte_terminal_set_audible_bell(VTE_TERMINAL(terminal),
	    FALSE);
	vte_terminal_set_visible_bell(VTE_TERMINAL(terminal),
	    FALSE);

	/* Pack widgets and start the terminal */
	gtk_container_add(GTK_CONTAINER(window), terminal);
	gtk_widget_show_all(window);
	gtk_main();
}
