AM_CPPFLAGS = $(MORE_CPPFLAGS)

bin_PROGRAMS = term

term_SOURCES  = term.h term.c color.c dabbrev.c
term_CFLAGS   = @GTK_CFLAGS@ @X11_CFLAGS@ @VTE_CFLAGS@ $(MORE_CFLAGS)
term_LDFLAGS  = @GTK_LIBS@   @X11_LIBS@   @VTE_LIBS@   $(MORE_LDFLAGS) -lm
