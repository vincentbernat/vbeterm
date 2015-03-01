/* -*- mode: c; c-file-style: "openbsd" -*- */
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

#include "term.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <vte/vte.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms-compat.h>
#include <glib.h>

GtkWidget *window, *terminal;
TermConfig config;

static void
set_font_size(gint delta)
{
	PangoFontDescription *descr;
	descr = pango_font_description_copy(vte_terminal_get_font(VTE_TERMINAL(terminal)));
	if (!descr) return;

	gint current = pango_font_description_get_size(descr);
	pango_font_description_set_size(descr, current + delta * PANGO_SCALE);
	vte_terminal_set_font(VTE_TERMINAL(terminal), descr);
	pango_font_description_free(descr);
}

static gboolean
term_config_load_theme(GKeyFile *kf, gchar *grp, TermTheme *theme)
{
    gsize len;
    gsize i;
    gchar *val;
    gchar **lval;

    theme->opacity = g_key_file_get_double(kf, grp, "Opacity", NULL);
    theme->bold = g_key_file_get_boolean(kf, grp, "Bold", NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    if ((val = g_key_file_get_string(kf, grp, "Cursor", NULL)) == NULL)
        return FALSE;
    gdk_color_parse(val, &theme->cursor);
    g_free(val);

    if ((val = g_key_file_get_string(kf, grp, "Font", NULL)) == NULL)
        return FALSE;
    g_strlcpy(theme->font, val, sizeof(theme->font));
    g_free(val);

    if ((val = g_key_file_get_string(kf, grp, "Foreground", NULL)) == NULL)
        return FALSE;
    gdk_color_parse(val, &theme->fg);
    g_free(val);

    if ((val = g_key_file_get_string(kf, grp, "Background", NULL)) == NULL)
        return FALSE;
    gdk_color_parse(val, &theme->bg);
    g_free(val);

    if ((lval = g_key_file_get_string_list(kf, grp, "Palette", &len, NULL)) == NULL)
        return FALSE;

    theme->palette_size = len;
    if (len > TERM_THEME_PALETTE_MAX) {
        g_free(lval);
        return FALSE;
    }

    for (i = 0; i < theme->palette_size; i++) {
        gdk_color_parse(lval[i], &theme->palette[i]);
    }
    g_free(lval);

#pragma GCC diagnostic pop
    return TRUE;
}

static void
term_config_load(void)
{
    gchar *path;
    GKeyFile *key_file;
    gboolean ret;
    gchar *start_group;
    gchar *val;
    gchar **lval;
    gsize list_len;
    gsize i;

    key_file = g_key_file_new();
    path = g_strdup_printf("%s/%s", getenv("HOME")?:"", TERM_CONFIG_PATH);
    ret = g_key_file_load_from_file(key_file,
                                    "/home/pyr/.config/vbeterm/vbeterm.conf",
                                    G_KEY_FILE_NONE,
                                    NULL);
    g_free(path);

    if (!ret) {
        /*
         * provide some sensible defaults
         */
        g_key_file_load_from_data(key_file, TERM_CONFIG_DEFAULT,
                                  strlen(TERM_CONFIG_DEFAULT),
                                  G_KEY_FILE_NONE,
                                  NULL);
    }

    start_group = g_key_file_get_start_group(key_file);

    if ((val = g_key_file_get_string(key_file, start_group, "WordChars", NULL)) == NULL) {
        g_strlcpy(config.word_chars, TERM_WORD_CHARS, sizeof(config.word_chars));
    } else {
        g_strlcpy(config.word_chars, val, sizeof(config.word_chars));
    }

    if ((lval = g_key_file_get_string_list(key_file, start_group, "Themes",
                                          &list_len, NULL)) == NULL ||
        list_len <= 0) {
        config.theme_count = 1;
        config.theme_index = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        gdk_color_parse("#838394949696", &config.themes[0].fg);
        gdk_color_parse("#00002b2b3636", &config.themes[0].bg);
        gdk_color_parse("#0f0f49499999", &config.themes[0].cursor);
        gdk_color_parse("#070736364242", &config.themes[0].palette[0]);
        gdk_color_parse("#dcdc32322f2f", &config.themes[0].palette[1]);
        gdk_color_parse("#858599990000", &config.themes[0].palette[2]);
        gdk_color_parse("#b5b589890000", &config.themes[0].palette[3]);
        gdk_color_parse("#26268b8bd2d2", &config.themes[0].palette[4]);
        gdk_color_parse("#d3d336368282", &config.themes[0].palette[5]);
        gdk_color_parse("#2a2aa1a19898", &config.themes[0].palette[6]);
        gdk_color_parse("#eeeee8e8d5d5", &config.themes[0].palette[7]);
        gdk_color_parse("#00002b2b3636", &config.themes[0].palette[8]);
        gdk_color_parse("#cbcb4b4b1616", &config.themes[0].palette[9]);
        gdk_color_parse("#58586e6e7575", &config.themes[0].palette[10]);
        gdk_color_parse("#65657b7b8383", &config.themes[0].palette[11]);
        gdk_color_parse("#838394949696", &config.themes[0].palette[12]);
        gdk_color_parse("#6c6c7171c4c4", &config.themes[0].palette[13]);
        gdk_color_parse("#9393a1a1a1a1", &config.themes[0].palette[14]);
        gdk_color_parse("#fdfdf6f6e3e3", &config.themes[0].palette[15]);
#pragma GCC diagnostic pop

        config.themes[0].bold = FALSE;
        config.themes[0].opacity = 1.0;
        g_strlcpy(config.themes[0].font, "Terminus 12",
                  sizeof(config.themes[0].font));
    } else {
        for (i = 0; i < list_len; i++) {
            TermTheme *theme;

            theme = &config.themes[config.theme_count];
            if (term_config_load_theme(key_file, lval[i], theme))
                config.theme_count++;
            else
                bzero(theme, sizeof(*theme));
        }
    }
}

void
term_theme_apply(gint index)
{
    TermTheme *theme = &config.themes[index];

	vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal), 0);
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal),  FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal), TRUE);
	vte_terminal_set_rewrap_on_resize(VTE_TERMINAL(terminal), TRUE);

	vte_terminal_set_colors(VTE_TERMINAL(terminal), &theme->fg, &theme->bg,
                            theme->palette, theme->palette_size);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	vte_terminal_set_opacity(VTE_TERMINAL(terminal), theme->opacity * 65535);
#pragma GCC diagnostic pop

	vte_terminal_set_color_cursor(VTE_TERMINAL(terminal), &theme->cursor);
	vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal),
	    VTE_CURSOR_BLINK_OFF);
	vte_terminal_set_allow_bold(VTE_TERMINAL(terminal), theme->bold);
	vte_terminal_set_font_from_string(VTE_TERMINAL(terminal), theme->font);
	set_font_size(0);

	vte_terminal_set_audible_bell(VTE_TERMINAL(terminal), FALSE);
	vte_terminal_set_visible_bell(VTE_TERMINAL(terminal), FALSE);
}

void
term_theme_cycle()
{
    gint next_theme = (config.theme_index + 1) % config.theme_count;

    if (config.theme_count <= 0)
        return;
    term_theme_apply(next_theme);
    config.theme_index = next_theme;
}

static gboolean
on_dpi_changed(GtkSettings *settings,
    GParamSpec *pspec,
    gpointer user_data)
{
	set_font_size(0);
	return TRUE;
}

static gboolean
on_char_size_changed(GtkWidget *terminal, guint width, guint height, gpointer user_data)
{
	set_font_size(0);
	return TRUE;
}

static gboolean
on_title_changed(GtkWidget *terminal, gpointer user_data)
{
	gtk_window_set_title(GTK_WINDOW(window),
	    vte_terminal_get_window_title(VTE_TERMINAL(terminal))?:PACKAGE_NAME);
	return TRUE;
}

static gboolean
on_key_press(GtkWidget *terminal, GdkEventKey *event)
{
	if (event->state & GDK_CONTROL_MASK) {
		switch (event->keyval) {
		case GDK_plus:
			set_font_size(1);
			return TRUE;
		case GDK_minus:
			set_font_size(-1);
			return TRUE;
		case GDK_equal:
			set_font_size(0);
			return TRUE;
        case GDK_percent:
            term_theme_cycle();
            return TRUE;
		}
	}
	return FALSE;
}

static gchar**
get_child_environment(void)
{
	guint n;
	gchar **env, **result, **p;
	const gchar *value;

	/* Copy the current environment */
	env = g_listenv();
	n = g_strv_length (env);
	result = g_new (gchar *, n + 2);
	for (n = 0, p = env; *p != NULL; ++p) {
		if (g_strcmp0(*p, "COLORTERM") == 0) continue;
		value = g_getenv (*p);
		if (G_LIKELY(value != NULL))
			result[n++] = g_strconcat (*p, "=", value, NULL);
	}
	g_strfreev(env);

	/* Setup COLORTERM */
	result[n++] = g_strdup_printf("COLORTERM=%s", PACKAGE_NAME);
	result[n] = NULL;
	return result;
}

int
main(int argc, char *argv[])
{
	/* Initialise GTK and the widgets */
	gtk_init(&argc, &argv);

    /* load configuration */
    bzero(&config, sizeof(config));
    term_config_load();
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	terminal = vte_terminal_new();
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	gtk_container_add(GTK_CONTAINER(window), terminal);
	gtk_widget_set_visual(window, gdk_screen_get_rgba_visual(gtk_widget_get_screen(window)));
	gtk_widget_show_all(window);
	gtk_window_set_focus(GTK_WINDOW(window), terminal);

	/* Connect some signals */
	g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
	g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);
	g_signal_connect(terminal, "window-title-changed", G_CALLBACK(on_title_changed), NULL);
	g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL);
	g_signal_connect(terminal, "char-size-changed", G_CALLBACK(on_char_size_changed), NULL);
	g_signal_connect(gtk_settings_get_default(), "notify::gtk-xft-dpi",
	    G_CALLBACK(on_dpi_changed), NULL);

	/* Configure terminal */
	vte_terminal_set_word_chars(VTE_TERMINAL(terminal), config.word_chars);
    term_theme_apply(0);


	/* Start a new shell */
	gchar **env;
	env = get_child_environment();
	vte_terminal_fork_command_full(VTE_TERMINAL (terminal),
	    VTE_PTY_DEFAULT,
	    NULL,		/* working directory */
	    (gchar *[]){ g_strdup(g_getenv("SHELL")), 0 },
	    env,		/* envv */
	    0,			/* spawn flags */
	    NULL, NULL,		/* child setup */
	    NULL,		/* child pid */
	    NULL);
	g_strfreev(env);

	/* Pack widgets and start the terminal */
	gtk_main();
	return FALSE;
}
