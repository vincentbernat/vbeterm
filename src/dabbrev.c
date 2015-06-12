/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2015 Vincent Bernat <bernat@luffy.cx>
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

#include <stdlib.h>
#include <string.h>

/* Per-terminal global state */
struct dabbrev_state {
	size_t corpus_len;
	char *corpus;	/* Terminal content */
	char *current;	/* Current position in content */
	char *prefix;	/* Prefix to complete */
	char *last_insert;	/* Last word inserted */
};

static void
dabbrev_free(struct dabbrev_state *state)
{
	if (state == NULL) return;
	free(state->corpus);
	free(state->prefix);
	free(state->last_insert);
}

#define DEL "\x7f"

static char *
append_corpus(VteTerminal *terminal, char *corpus)
{
	char *text = vte_terminal_get_text(terminal, NULL, NULL, NULL);
	if (corpus == NULL) return text;

	char *new_corpus = g_strdup_printf("%s %s", corpus, text);
	free(corpus); free(text);
	return new_corpus;
}

static void
update_corpus(GtkWindow *window, VteTerminal *terminal, struct dabbrev_state *state)
{
	if (state->corpus != NULL) return;

	/* Append all windows */
	char *corpus = NULL;
	for (GList *windows = gtk_application_get_windows(gtk_window_get_application(window));
	     windows;
	     windows = windows->next) {
		GtkWindow *other_window = windows->data;
		VteTerminal *other_terminal = g_object_get_data(G_OBJECT(other_window),
		    "terminal");
		if (other_window == window || other_terminal == NULL) continue;

		corpus = append_corpus(other_terminal, corpus);
	}

	/* Append the current window */
	state->corpus = append_corpus(terminal, corpus);
	state->corpus_len = strlen(state->corpus);
	state->current = state->corpus + state->corpus_len;
}

/* Get next word from current position */
static const char *
next_word(VteTerminal *terminal, struct dabbrev_state *state)
{
	char *end = state->current - 1;
	while (1) {
		if (end < state->corpus)
			end = state->corpus + state->corpus_len;
		if (end == state->current)
			return NULL;
		if (*end == '\0') {
			/* Shortcut case */
			end--;
			continue;
		}
		if (!vte_terminal_is_word_char(terminal, *end)) {
			*end = '\0';
			end--;
			continue;
		}
		break;
	}
	char *start = end - 1;
	while (1) {
		if (start < state->corpus ||
		    *start == '\0' ||
		    !vte_terminal_is_word_char(terminal, *start)) {
			state->current = ++start;
			return start;
		}
		start--;
	}
	return NULL;		/* Unreachable */
}

/* Get the next word matching the current prefix */
static const char *
next_word_matching_prefix(VteTerminal *terminal, struct dabbrev_state *state) {
	const char *match, *first_match = next_word(terminal, state);
	match = first_match;
	if (match == NULL) return NULL;
	while (strncmp(match, state->prefix, strlen(state->prefix)) ||
	    !strcmp(match, state->prefix)) {
		/* Erase the current word from the corpus */
		memmove((char*)match, match + strlen(match) + 1,
		    state->corpus_len - (match - state->corpus) - (strlen(match) + 1));
		state->corpus_len -= strlen(match) + 1;

		/* Try another one */
		match = next_word(terminal, state);
		if (match == NULL) return NULL; /* Safety */
		if (match == first_match) return NULL;
	}
	return match;
}

void
dabbrev_expand(GtkWindow *window, VteTerminal *terminal)
{
	struct dabbrev_state *state = g_object_get_data(G_OBJECT(terminal), "dabbrev");
	if (state == NULL) {
		if ((state = calloc(1, sizeof(struct dabbrev_state))) == NULL)
			return;
		g_object_set_data_full(G_OBJECT(terminal), "dabbrev", state,
		    (GDestroyNotify)dabbrev_free);
	}
	if (state->prefix == NULL) {
		/* What prefix do we want to complete? */
		glong row, start_column, end_column;
		vte_terminal_get_cursor_position(terminal,
		    &end_column, &row);
		for (start_column = end_column - 1;
		     start_column >= 0;
		     start_column--) {
			char *newprefix = vte_terminal_get_text_range(terminal,
			    row, start_column,
			    row, end_column - 1,
			    NULL, NULL, NULL);
			if (!vte_terminal_is_word_char(terminal, newprefix[0])) {
				free(newprefix);
				break;
			}
			free(state->prefix);
			state->prefix = newprefix;
		}
		state->prefix[strlen(state->prefix) - 1] = '\0'; /* Remove newline */
		if (!state->prefix || strlen(state->prefix) < TERM_DABBREV_MIN_PREFIX) {
			free(state->prefix); state->prefix = NULL;
			return;
		}
	}
	update_corpus(window, terminal, state);

	const char *next_insert = next_word_matching_prefix(terminal, state);
	if (next_insert == NULL) return;
	next_insert += strlen(state->prefix);

	/* Prepare stream to be sent */
	if (state->last_insert != NULL) {
		if (!strcmp(state->last_insert, next_insert)) {
			/* Already inserted the same, do nothing */
			return;
		}
		/* Erase last insert */
		size_t len = strlen(state->last_insert);
		for (size_t i = 0; i < len; i++)
			vte_terminal_feed_child_binary(terminal, DEL, 1);
	}
	/* Send it */
	vte_terminal_feed_child(terminal, next_insert, strlen(next_insert));

	free(state->last_insert);
	state->last_insert = strdup(next_insert);
}

void
dabbrev_stop(VteTerminal *terminal)
{
	g_object_set_data_full(G_OBJECT(terminal), "dabbrev",
	    NULL, (GDestroyNotify)dabbrev_free);
}
