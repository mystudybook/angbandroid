/**
 * \file z-textblock.c
 * \brief Text output bugger (?NRM) code
 *
 * Copyright (c) 2010 Andi Sidwell
 * Copyright (c) 2011 Peter Denison
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Text blocks now use the internal representation of display characters, i.e.
 * wchar_t, since they are expected to display straight to screen. Conversion
 * from the incoming locale-encoded format is done in textblock_vappend_c().
 */
#include "z-color.h"
#include "z-textblock.h"
#include "z-util.h"
#include "z-virt.h"
#include "z-form.h"
#include "z-file.h"
#include "textblock-column-layout.h"
#include "zh-tw-file-text-wrap.h"

#define TEXTBLOCK_LEN_INITIAL		128
#define TEXTBLOCK_LEN_INCR(x)		((x) + 128)

struct textblock {
	wchar_t *text;
	uint8_t *attrs;

	size_t strlen;
	size_t size;
};



/**
 * Create a new textblock object and return it.
 */
textblock *textblock_new(void)
{
	textblock *tb = mem_zalloc(sizeof *tb);

	tb->size = TEXTBLOCK_LEN_INITIAL;
	tb->text = mem_zalloc(tb->size * sizeof *tb->text);
	tb->attrs = mem_zalloc(tb->size);

	return tb;
}

/**
 * Free a textblock object.
 */
void textblock_free(textblock *tb)
{
	mem_free(tb->text);
	mem_free(tb->attrs);
	mem_free(tb);
}

/**
 * Resize the internal textblock storage (if needed) to hold additional
 * characters.
 *
 * \param tb is the textblock we need to resize.
 * \param additional_size is how many characters we want to add.
 */
static void textblock_resize_if_needed(textblock *tb, size_t additional_size)
{
	size_t remaining = tb->size - tb->strlen;

	/* If we need more room, reallocate it */
	if (remaining < additional_size) {
		tb->size = TEXTBLOCK_LEN_INCR(tb->strlen + additional_size);
		tb->text = mem_realloc(tb->text, tb->size * sizeof *tb->text);
		tb->attrs = mem_realloc(tb->attrs, tb->size);
	}
}

static void textblock_vappend_c(textblock *tb, uint8_t attr, const char *fmt,
		va_list vp)
{
	size_t temp_len = TEXTBLOCK_LEN_INITIAL;
	char *temp_space;
	int new_length;

	/* The stock vstrnfmt() formats each %s into an internal 1024-byte
	 * temporary buffer. Resizing the outer buffer cannot recover a long
	 * Chinese lore description once that inner buffer has truncated it.
	 * A plain %s is the common monster/object lore path, so copy that
	 * argument directly without reformatting or truncating UTF-8 bytes. */
	if (strcmp(fmt, "%s") == 0) {
		const char *plain = va_arg(vp, const char *);
		if (!plain) plain = "";
		new_length = text_mbstowcs(NULL, plain, 0);
		assert(new_length >= 0);
		textblock_resize_if_needed(tb, (size_t)new_length + 1);
		text_mbstowcs(tb->text + tb->strlen, plain,
			tb->size - tb->strlen);
		memset(tb->attrs + tb->strlen, attr, (size_t)new_length);
		tb->strlen += (size_t)new_length;
		return;
	}

	temp_space = mem_zalloc(temp_len);

	/* We have to format the incoming string in native (external) format
	 * re-allocating the temporary space as necessary. Once it's been
	 * successfully formatted, we can then do the conversion to wide chars
	 */
	while (1) {
		va_list args;
		size_t len;

		va_copy(args, vp);
		len = vstrnfmt(temp_space, temp_len, fmt, args);
		va_end(args);
		if (len < temp_len - 1) {
			/* Not using all space, therefore it completed */
			break;
		}

		temp_len = TEXTBLOCK_LEN_INCR(temp_len);
		temp_space = mem_realloc(temp_space, temp_len * sizeof *temp_space);
	}

	/* Get extent of addition in wide chars */
	new_length = text_mbstowcs(NULL, temp_space, 0);
	assert(new_length >= 0); /* If this fails, the string was badly formed */
	textblock_resize_if_needed(tb, new_length + 1);

	/* Convert to wide chars, into the text block buffer */
	text_mbstowcs(tb->text + tb->strlen, temp_space, tb->size - tb->strlen);
	memset(tb->attrs + tb->strlen, attr, new_length);
	tb->strlen += new_length;
	mem_free(temp_space);
}

/**
 * Add a graphics tile to a text block.
 */
void textblock_append_pict(textblock *tb, uint8_t attr, int c)
{
	textblock_resize_if_needed(tb, 1);
	tb->text[tb->strlen] = (wchar_t)c;
	tb->attrs[tb->strlen] = attr;
	tb->strlen += 1;
}

/**
 * Append one textblock to another.
 *
 * \param tb is the textblock we are appending to.
 * \param tba is the textblock to append.
 */
void textblock_append_textblock(textblock *tb, const textblock *tba)
{
	textblock_resize_if_needed(tb, tba->strlen);
	(void) memcpy(tb->text + tb->strlen, tba->text,
		tba->strlen * sizeof(*tb->text));
	(void) memcpy(tb->attrs + tb->strlen, tba->attrs, tba->strlen);
	tb->strlen += tba->strlen;
}


/**
 * Add text to a text block, formatted.
 */
void textblock_append(textblock *tb, const char *fmt, ...)
{
	va_list vp;
	va_start(vp, fmt);

	textblock_vappend_c(tb, COLOUR_WHITE, fmt, vp);

	va_end(vp);
}

/**
 * Add coloured text to a text block, formatted.
 */
void textblock_append_c(textblock *tb, uint8_t attr, const char *fmt, ...)
{
	va_list vp;
	va_start(vp, fmt);

	textblock_vappend_c(tb, attr, fmt, vp);

	va_end(vp);

}

/**
 * Return a pointer to the text inputted thus far.
 */
const wchar_t *textblock_text(textblock *tb)
{
	return tb->text;
}

/**
 * Return a pointer to the text attrs.
 */
const uint8_t *textblock_attrs(textblock *tb)
{
	return tb->attrs;
}

static void new_line(size_t **line_starts, size_t **line_lengths,
		size_t *n_lines, size_t *cur_line,
		size_t start, size_t len)
{
	if (*cur_line == *n_lines) {
		/* this number is not arbitrary: it's the height of a "standard" term */
		(*n_lines) += 24;

		*line_starts = mem_realloc(*line_starts,
				*n_lines * sizeof **line_starts);
		*line_lengths = mem_realloc(*line_lengths,
				*n_lines * sizeof **line_lengths);
	}

	(*line_starts)[*cur_line] = start;
	(*line_lengths)[*cur_line] = len;

	(*cur_line)++;
}

/**
 * Given a certain width, split a textblock into wrapped lines of text. Trailing
 * empty lines are trimmed.
 *
 * \param tb The textblock to wrap.
 * \param line_starts On return, an array (indexed by line number) of character
 *		  indexes to the text of \c tb where each line begins.
 * \param line_lengths On return, an array (indexed by line number) of line
 *		  lengths.
 * \param width The maximum permitted width of each line.
 * \return Number of lines in output.
 */
size_t textblock_calculate_lines(textblock *tb, size_t **line_starts,
        size_t **line_lengths, size_t width)
{
    const wchar_t *text;
    const uint8_t *attrs;
    size_t alloc_lines = 0, total_lines = 0, offset = 0;

    if (!tb || !line_starts || !line_lengths || width == 0) return 0;
    text = textblock_text(tb);
    attrs = textblock_attrs(tb);
    if (!text || tb->strlen == 0) return 0;

    /* Width is in physical terminal cells, not wchar_t codepoints.  In
     * particular, do not treat Chinese lore paragraphs as one-cell glyphs.
     * The helper returns wchar_t offsets for the existing renderers. */
    while (offset < tb->strlen) {
        angdroid_textblock_line line = angdroid_textblock_next_line(
            text, attrs, tb->strlen, offset, width);
        new_line(line_starts, line_lengths, &alloc_lines, &total_lines,
            offset, line.length);
        if (line.next <= offset) break;
        offset = line.next;
    }
    return total_lines;
}

/**
 * Output a textblock to file.
 */
void textblock_to_file(textblock *tb, ang_file *f, int indent, int wrap_at)
{
	size_t *line_starts = NULL;
	size_t *line_lengths = NULL;

	size_t n_lines, i, j;
	char *mbbuf;

	int width = wrap_at - indent;
	assert(width > 0);

	n_lines = textblock_calculate_lines(tb, &line_starts, &line_lengths, width);
	mbbuf = mem_alloc(text_wcsz() + 1);

	for (i = 0; i < n_lines; i++) {
		if (indent > 0) {
			file_putf(f, "%*c", indent, ' ');
		}
		for (j = 0; j < line_lengths[i]; ++j) {
			int nc = text_wctomb(mbbuf,
				tb->text[line_starts[i] + j]);

			if (nc > 0) {
				mbbuf[nc] = 0;
			} else {
				mbbuf[0] = ' ';
				mbbuf[1] = 0;
			}
			file_putf(f, "%s", mbbuf);
		}
		file_putf(f, "\n");
	}

	mem_free(mbbuf);
	mem_free(line_starts);
	mem_free(line_lengths);
}




/**
 * ------------------------------------------------------------------------
 * text_out()
 * ------------------------------------------------------------------------ */


/**
 * Function hook to output (colored) text to the screen or to a file.
 */
void (*text_out_hook)(uint8_t a, const char *str);

/**
 * Hack -- Where to wrap the text when using text_out().  Use the default
 * value (for example the screen width) when 'text_out_wrap' is 0.
 */
int text_out_wrap = 0;

/**
 * Hack -- Indentation for the text when using text_out().
 */
int text_out_indent = 0;

/**
 * Hack -- Padding after wrapping
 */
int text_out_pad = 0;


/**
 * Hack - the destination file for text_out_to_file.
 */
ang_file *text_out_file = NULL;


/**
 * Write text to the given file and apply line-wrapping.
 *
 * Hook function for text_out(). Make sure that text_out_file points
 * to an open text-file.
 *
 * Long lines will be wrapped at text_out_wrap, or at column 75 if that
 * is not set; or at a newline character.  Note that punctuation can
 * sometimes be placed one column beyond the wrap limit.
 *
 * You must be careful to end all file output with a newline character
 * to "flush" the stored line position.
 */
/* File exports share the same UTF-8 renderer as other display-only prose.
 * Keep all file API calls in this TU; the column-aware helper is pure C99. */
static void angdroid_textblock_write_file_bytes(void *context,
        const char *text, size_t length)
{
    file_write((ang_file *)context, text, length);
}

static void angdroid_textblock_write_file_char(void *context, char character)
{
    /* Match the byte-based API used for the adjacent UTF-8 scalar writes. */
    file_write((ang_file *)context, &character, 1);
}

void text_out_to_file(uint8_t a, const char *str)
{
	const char *s;

	/* Current position on the line */
	static int pos = 0;

	/* A non-positive width would make the ASCII loop emit endless empty
	 * lines without consuming its input. Keep a usable display region. */
	int wrap = (text_out_wrap > 0) ? text_out_wrap : 75;
	int indent = text_out_indent > 0 ? text_out_indent : 0;

	if (indent >= wrap) indent = wrap - 1;

	/* Unused parameter */
	(void)a;

	/* The original byte-counting wrap splits a CJK character when a byte
	 * limit falls in the middle of its UTF-8 encoding. Use complete Unicode
	 * scalars and terminal columns only for non-ASCII export text; preserve
	 * all existing ASCII/English wrapping and formatting below. */
	if (angdroid_text_has_non_ascii(str)) {
		angdroid_utf8_text_file_wrap(str, &pos, wrap, indent,
			text_out_file, angdroid_textblock_write_file_bytes,
			angdroid_textblock_write_file_char);
		return;
	}

	/* The ASCII wrapping loop only reads the input; avoid copying
	 * it into a fixed 1024-byte buffer, which silently clipped long
	 * exported descriptions even after UTF-8 output was fixed. */
	s = str;

	/* Process the string */
	while (*s) {
		int n = 0;
		int len;
		int l_space = -1;

		/* If we are at the start of the line... */
		if (pos == 0) {
			int i;

			/* Output the indent */
			for (i = 0; i < indent; i++) {
				file_writec(text_out_file, ' ');
				pos++;
			}
		}

		/* Count indentation against the wrapping width, not on top
		 * of it. Previously the first ASCII row could exceed wrap. */
		len = wrap - pos;
		if (len < 0) len = 0;

		/* Find length of line up to next newline or end-of-string */
		while ((n < len) && !((s[n] == '\n') || (s[n] == '\0'))) {
			/* Mark the most recent space in the string */
			if (s[n] == ' ') l_space = n;

			/* Increment */
			n++;
		}

		/* If we have encountered no spaces */
		/* A fragment ending exactly at the remaining column limit
		 * is already a complete line segment: do not insert a newline
		 * before its final character on the next text_out_to_file()
		 * call. Only pre-wrap when more characters remain. */
		if ((l_space == -1) && (n == len) &&
			(s[n] != '\0') && (s[n] != '\n')) {
			/* If we are at the start of a new line */
			if (pos == indent) {
				len = n;
			} else if (s[0] == ' ' && pos >= wrap) {
				/* An isolated space in the next output chunk is a
				 * word separator, not one more display column after
				 * an already full line. Wrap once and consume the
				 * run of separators without exceeding the width. */
				file_writec(text_out_file, '\n');
				pos = 0;
				while (*s == ' ') s++;
				continue;
			} else if ((s[0] == ' ') || (s[0] == ',') || (s[0] == '.')) {
				/* HACK - Output punctuation at the end of the line */
				len = 1;
			} else {
				/* Begin a new line */
				file_writec(text_out_file, '\n');

				/* Reset */
				pos = 0;

				continue;
			}
		} else {
			/* Wrap at the newline */
			if ((s[n] == '\n') || (s[n] == '\0')) len = n;

			/* Wrap at the last space */
			else len = l_space;
		}

		/* Write that line to file */
		file_write(text_out_file, s, len);
		pos += len;

		/* Move 's' past the stuff we've written */
		s += len;

		/* If we are at the end of the string, end */
		if (*s == '\0') return;

		/* Skip newlines */
		if (*s == '\n') s++;

		/* Begin a new line */
		file_writec(text_out_file, '\n');

		/* Reset */
		pos = 0;

		/* Skip whitespace */
		while (*s == ' ') s++;
	}

	/* We are done */
	return;
}


/**
 * Output text to the screen or to a file depending on the selected
 * text_out hook.
 */
/* A compound format may contain a long %s field even when it is not
 * exactly "%s". Retry formatting until the output fits: the old 1024-byte
 * local buffer clipped those descriptions before they reached the hook. */
static void angdroid_text_out_vformatted(uint8_t attr,
		const char *fmt, va_list vp)
{
	size_t capacity = 1024;
	char *buf = mem_alloc(capacity);

	while (1) {
		va_list args;
		size_t length;

		va_copy(args, vp);
		length = vstrnfmt(buf, capacity, fmt, args);
		va_end(args);
		if (length < capacity - 1) break;

		/* Guard the doubling arithmetic before asking the allocator. */
		if (capacity > ((size_t)-1) / 2) break;
		capacity *= 2;
		buf = mem_realloc(buf, capacity);
	}

	text_out_hook(attr, buf);
	mem_free(buf);
}

void text_out(const char *fmt, ...)
{
	va_list vp;

	va_start(vp, fmt);
	if (strcmp(fmt, "%s") == 0) {
		const char *plain = va_arg(vp, const char *);
		text_out_hook(COLOUR_WHITE, plain ? plain : "");
	} else {
		angdroid_text_out_vformatted(COLOUR_WHITE, fmt, vp);
	}
	va_end(vp);
}

/**
 * Output text to the screen (in color) or to a file depending on the
 * selected hook.
 */
void text_out_c(uint8_t a, const char *fmt, ...)
{
	va_list vp;

	va_start(vp, fmt);
	if (strcmp(fmt, "%s") == 0) {
		const char *plain = va_arg(vp, const char *);
		text_out_hook(a, plain ? plain : "");
	} else {
		angdroid_text_out_vformatted(a, fmt, vp);
	}
	va_end(vp);
}


/**
 * Given a "formatted" chunk of text (i.e. one including tags like {red}{/})
 * in 'source', with starting point 'init', this finds the next section of
 * text and any tag that goes with it, return true if it finds something to 
 * print.
 * 
 * If it returns true, then it also fills 'text' with a pointer to the start
 * of the next printable section of text, and 'len' with the length of that 
 * text, and 'end' with a pointer to the start of the next section.  This
 * may differ from "text + len" because of the presence of tags.  If a tag
 * applies to the section of text, it returns a pointer to the start of that
 * tag in 'tag' and the length in 'taglen'.  Otherwise, 'tag' is filled with
 * NULL.
 *
 * See text_out_e for an example of its use.
 */
static bool next_section(const char *source, size_t init, const char **text,
						 size_t *len, const char **tag, size_t *taglen,
						 const char **end)
{
	const char *next;	

	*tag = NULL;
	*text = source + init;
	if (*text[0] == '\0') return false;

	next = strchr(*text, '{');
	while (next)
	{
		const char *s = next + 1;

		while (*s && (isalpha((unsigned char) *s) ||
					  isspace((unsigned char) *s)))
			s++;

		/* Woo!  valid opening tag thing */
		if (*s == '}') {
			const char *close = strstr(s, "{/}");

			/* There's a closing thing, so it's valid. */
			if (close) {
				/* If this tag is at the start of the fragment */
				if (next == *text) {
					*tag = *text + 1;
					*taglen = s - *text - 1;
					*text = s + 1;
					*len = close - *text;
					*end = close + 3;
					return true;
				} else {
					/* Otherwise return the chunk up to this */
					*len = next - *text;
					*end = *text + *len;
					return true;
				}
			} else {
				/* No closing thing, therefore all one lump of text. */
				*len = strlen(*text);
				*end = *text + *len;
				return true;
			}
		} else if (*s == '\0') {
			/* End of the string, that's fine. */
			*len = strlen(*text);
			*end = *text + *len;
			return true;
		} else {
			/* An invalid tag, skip it. */
			next = next + 1;
		}

		next = strchr(next, '{');
	}

	/* Default to the rest of the string */
	*len = strlen(*text);
	*end = *text + *len;

	return true;
}

/**
 * Output text to the screen or to a file depending on the
 * selected hook.  Takes strings with "embedded formatting",
 * such that something within {red}{/} will be printed in red.
 *
 * Note that such formatting will be treated as a "breakpoint"
 * for the printing, so if used within words may lead to part of the
 * word being moved to the next line.
 */
void text_out_e(const char *fmt, ...)
{
	char buf[1024];
	char smallbuf[1024];
	va_list vp;

	const char *start, *next, *text, *tag;
	size_t textlen, taglen = 0;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Do the va_arg fmt to the buffer */
	(void)vstrnfmt(buf, sizeof(buf), fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	start = buf;
	while (next_section(start, 0, &text, &textlen, &tag, &taglen, &next)) {
		int a = -1;

		memcpy(smallbuf, text, textlen);
		smallbuf[textlen] = 0;

		if (tag) {
			char tagbuffer[16];

			/* Colour names are less than 16 characters long. */
			assert(taglen < 16);

			memcpy(tagbuffer, tag, taglen);
			tagbuffer[taglen] = '\0';

			a = color_text_to_attr(tagbuffer);
		}
		
		if (a == -1) 
			a = COLOUR_WHITE;

		/* Output now */
		text_out_hook(a, smallbuf);

		start = next;
	}
}


/**
 * Write a text file from given input.
 *
 * \param path the path to write to
 * \param writer the text-writing function
 */
errr text_lines_to_file(const char *path, text_writer writer)
{
	char new_fname[1024];
	char old_fname[1024];

	ang_file *new_file;

	/* Format filenames */
	file_get_tempfile(new_fname, sizeof(new_fname), path, "new");
	file_get_tempfile(old_fname, sizeof(old_fname), path, "old");

	/* Write new file */
	new_file = file_open(new_fname, MODE_WRITE, FTYPE_TEXT);
	if (!new_file) {
		return -1;
	}

	text_out_file = new_file;
	writer(new_file);
	text_out_file = NULL;

	file_close(new_file);

	/* Move files around */
	if (!file_exists(path)) {
		file_move(new_fname, path);
	} else if (file_move(path, old_fname)) {
		file_move(new_fname, path);
		file_delete(old_fname);
	} else {
		file_delete(new_fname);
	}

	return 0;
}

