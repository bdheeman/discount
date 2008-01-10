#ifndef _MARKDOWN_D
#define _MARKDOWN_D

#include "cstring.h"

/* reference-style links (and images) are stored in an array
 * of footnotes.
 */
typedef struct footnote {
    Cstring tag;		/* the tag for the reference link */
    Cstring link;		/* what this footnote points to */
    Cstring title;		/* what it's called (TITLE= attribute) */
    int height, width;		/* dimensions (for image link) */
} Footnote;

/* each input line is read into a Line, which contains the line,
 * the offset of the first non-space character [this assumes 
 * that all tabs will be expanded to spaces!], and a pointer to
 * the next line.
 */
typedef struct line {
    Cstring text;
    struct line *next;
    int dle;
} Line;


/* a paragraph is a collection of Lines, with links to the next paragraph
 * and (if it's a QUOTE, UL, or OL) to the reparsed contents of this
 * paragraph.
 */
typedef struct paragraph {
    struct paragraph *next;	/* next paragraph */
    struct paragraph *down;	/* recompiled contents of this paragraph */
    struct line *text;		/* all the text in this paragraph */
    enum { WHITESPACE=0, CODE, QUOTE, MARKUP, HTML, DL, UL, OL, LISTITEM, HDR, HR } typ;
    enum { IMPLICIT=0, PARA, CENTER} align;
    int hnumber;		/* <Hn> for typ == HDR */
} Paragraph;

enum { ETX, SETEXT };	/* header types */


/*
 * the mkdio text input functions return a document structure,
 * which contains a header (retrieved from the document if
 * markdown was configured * with the * --enable-pandoc-header
 * and the document begins with a pandoc-style header) and the
 * root of the linked list of Lines.
 */
typedef struct document {
    Line *headers;		/* title -> author(s) -> date */
    ANCHOR(Line) content;	/* uncompiled text, not valid after compile() */
    Paragraph *code;		/* intermediate code generated by compile() */
    int compiled;		/* set after mkd_compile() */
} Document;


/* a magic markdown io thing holds all the data structures needed to
 * do the backend processing of a markdown document
 */
typedef struct mmiot {
    FILE *out;
    Cstring in;
    int isp;
    STRING(Footnote) footnotes;
    int flags;
#define DENY_A		0x01
#define DENY_IMG	0x02
#define DENY_SMARTY	0x04
#define EXPAND_QUOTE	0x10
} MMIOT;

extern int    mkd_firstnonblank(Line *);
extern int    mkd_compile(Document *, FILE *, int, MMIOT *);
extern void   mkd_generatehtml(Document *, MMIOT *);
extern void   mkd_cleanup(Document *, MMIOT *);
extern int    mkd_text(char *, int, FILE*, int);

extern Document *mkd_in(FILE *);
extern Document *mkd_string(char*,int);


#endif/*_MARKDOWN_D*/
