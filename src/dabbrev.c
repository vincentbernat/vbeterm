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

/* TODO: be UTF-8 compliant */

/* Global state */
static char *corpus = NULL;	/* Terminal content */
static size_t corpus_len = 0;
static char *current = NULL;	/* Current position in content */
static char *prefix = NULL;	/* Prefix to complete */
static const char *last_insert = NULL;	/* Last word inserted */

#define DEL "\x7f"

/* Get next word from current position */
static const char *
next_word(VteTerminal *terminal) {
	char *end = current - 1;
	while (1) {
		if (end < corpus)
			end = corpus + corpus_len;
		if (end == current)
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
		if (start < corpus ||
		    *start == '\0' ||
		    !vte_terminal_is_word_char(terminal, *start)) {
			current = ++start;
			return start;
		}
		start--;
	}
	return NULL;		/* Unreachable */
}

/* Get the next word matching the current prefix */
static const char *
next_word_matching_prefix(VteTerminal *terminal) {
	const char *match, *first_match = next_word(terminal);
	match = first_match;
	if (match == NULL) return NULL;
	while (strncmp(match, prefix, strlen(prefix)) || !strcmp(match, prefix)) {
		match = next_word(terminal);
		if (match == NULL) return NULL; /* Safety */
		if (match == first_match) return NULL;
	}
	return match;
}

void
dabbrev_expand(VteTerminal *terminal)
{
	if (prefix == NULL) {
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
			free(prefix);
			prefix = newprefix;
		}
		if (!prefix || strlen(prefix) < TERM_DABBREV_MIN_PREFIX) {
			free(prefix); prefix = NULL;
			return;
		}
		prefix[strlen(prefix) - 1] = '\0'; /* Remove newline */
	}
	if (corpus == NULL) {
		/* Retrieve the corpus */
		corpus = vte_terminal_get_text(terminal, NULL, NULL, NULL);
		corpus_len = strlen(corpus);
		current = corpus + corpus_len;
	}

	const char *next_insert = next_word_matching_prefix(terminal);
	if (next_insert == NULL) return;
	next_insert += strlen(prefix);

	/* Prepare stream to be sent */
	if (last_insert != NULL) {
		if (!strcmp(last_insert, next_insert)) {
			/* Already inserted the same, do nothing */
			return;
		}
		/* Erase last insert */
		size_t len = strlen(last_insert);
		for (size_t i = 0; i < len; i++)
			vte_terminal_feed_child_binary(terminal, DEL, 1);
	}
	/* Send it */
	vte_terminal_feed_child(terminal, next_insert, strlen(next_insert));

	last_insert = next_insert;
}

void dabbrev_stop()
{
	free(corpus); corpus = NULL;
	free(prefix); prefix = NULL;
	last_insert = NULL;
}
