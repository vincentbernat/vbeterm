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

#ifndef _BOOTSTRAP_H
#define _BOOTSTRAP_H

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdk.h>

#define TERM_WORD_CHARS "-A-Za-z0-9:./?%&#_=+@~"
#define TERM_OPACITY 1.0
#define TERM_FONT "Terminus 12"
#define TERM_WORDCHARS_MAX 32
#define TERM_THEME_NAMELEN 32
#define TERM_THEMES_MAX 8
#define TERM_THEME_PALETTE_MAX 16
#define TERM_THEME_FONT_MAX 64
#define TERM_CONFIG_PATH ".config/vbeterm/vbeterm.conf"
#define TERM_CONFIG_DEFAULT ("[main]\n"                                 \
                             "WordChars=-A-Za-z0-9:./?%&#_=+@~\n"       \
                             "Themes=default\n"                         \
                             "[default]\n"                              \
                             "Font=DejaVu Sans Mono for Powerline 10\n" \
                             "Opacity=0.85\n"                           \
                             "Bold=true\n"                              \
                             "Cursor=#008800\n"                         \
                             "Foreground=#ffffff\n"                     \
                             "Background=#000000\n"                     \
                             "Palette=#111111;#d36265;#xaece91;"        \
                             "#e7e18c;#5297cf;"                         \
                             "#963c59;#5E7175;#bebebe;"                 \
                             "#666666;#ef8171;#cfefb3;"                 \
                             "#fff796;#74b8ef;#b85e7b;#A3BABF;#ffffff\n")

typedef struct {
    gchar name[TERM_THEME_NAMELEN];
    GdkColor fg;
    GdkColor bg;
    GdkColor cursor;
    gboolean bold;
    gdouble opacity;
    gsize palette_size;
    GdkColor palette[TERM_THEME_PALETTE_MAX];
    gchar font[TERM_THEME_FONT_MAX];
} TermTheme;

typedef struct {
    gchar word_chars[TERM_WORDCHARS_MAX];
    int theme_count;
    int theme_index;
    TermTheme themes[TERM_THEMES_MAX];
} TermConfig;

#endif
