/**
 * \file z-form.c
 * \brief Low-level text formatting (snprintf() replacement)
 *
 * Copyright (c) 1997 Ben Harrison
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */
#include "z-form.h"
#include "zh-tw-format-utf8.h"
#include "z-type.h"
#include "z-util.h"
#include "z-virt.h"


/**
 * Here is some information about the routines in this file.
 *
 * In general, the following routines take a "buffer", a "max length",
 * a "format string", and some "arguments", and use the format string
 * and the arguments to create a (terminated) string in the buffer
 * (using only the first "max length" bytes), and return the "length"
 * of the resulting string, not including the (mandatory) terminator.
 *
 * The format strings allow the basic "sprintf()" format sequences, though
 * some of them are processed slightly more carefully or portably, as well
 * as a few "special" sequences, including the "capilitization" sequences of
 * "%C" and "%S".
 *
 * Note that some "limitations" are enforced by the current implementation,
 * for example, no "format sequence" can exceed 100 characters, including any
 * "length" restrictions, and the result of combining and "format sequence"
 * with the relevent "arguments" must not exceed 1000 characters.
 *
 * These limitations could be fixed by stealing some of the code from,
 * say, "vsprintf()" and placing it into my "vstrnfmt()" function.
 *
 * Legal format characters: %,b,n,p,c,s,d,i,o,u,X,x,E,e,F,f,G,g,r,v.
 *
 * Format("%%")
 *   Append the literal "%".
 *   No legal modifiers.
 *
 * Format("%n", size_t *np)
 *   Save the current length into (*np).
 *   No legal modifiers.
 *
 * Format("%p", void *v)
 *   Append the pointer "v" (implementation varies).
 *   No legal modifiers.
 *
 * Format("%E", double r)
 * Format("%F", double r)
 * Format("%G", double r)
 * Format("%e", double r)
 * Format("%f", double r)
 * Format("%g", double r)
 *   Append the double "r", in various formats.
 *
 * Format("%ld", long int i)
 *   Append the long integer "i".
 *
 * Format("%d", int i)
 *   Append the integer "i".
 *
 * Format("%lu", unsigned long int i)
 *   Append the unsigned long integer "i".
 *
 * Format("%u", unsigned int i)
 *   Append the unsigned integer "i".
 *
 * Format("%lo", unsigned long int i)
 *   Append the unsigned long integer "i", in octal.
 *
 * Format("%o", unsigned int i)
 *   Append the unsigned integer "i", in octal.
 *
 * Format("%lX", unsigned long int i)
 *   Note -- use all capital letters
 * Format("%lx", unsigned long int i)
 *   Append the unsigned long integer "i", in hexidecimal.
 *
 * Format("%X", unsigned int i)
 *   Note -- use all capital letters
 * Format("%x", unsigned int i)
 *   Append the unsigned integer "i", in hexidecimal.
 *
 * Format("%c", char c)
 *   Append the character "c".
 *   Do not use the "+" or "0" flags.
 *
 * Format("%s", const char *s)
 *   Append the string "s".
 *   Do not use the "+" or "0" flags.
 *   Note that a "NULL" value of "s" is converted to the empty string.
 *
 * Format("%y", type_union *y). Use any of the above patterns; 
 * z is interpreted as one of c, d, f, or s in the patterns above,
 * as appropriate for the type of the corresponding argument.
 * (There is currently no way to render a typeunion in octal or hex.)
 * 
 * For examples below, assume "int n = 0; int m = 100; char buf[100];",
 * plus "char *s = NULL;", and unknown values "char *txt; int i;".
 *
 * For example: "n = strnfmt(buf, -1, "(Max %d)", i);" will have a
 * similar effect as "sprintf(buf, "(Max %d)", i); n = strlen(buf);".
 *
 * For example: "(void)strnfmt(buf, 16, "%s", txt);" will have a similar
 * effect as "strncpy(buf, txt, 16); buf[15] = '\0';".
 *
 * For example: "if (strnfmt(buf, 16, "%s", txt) < 16) ..." will have
 * a similar effect as "strcpy(buf, txt)" but with bounds checking.
 *
 * For example: "s = buf; s += vstrnfmt(s, -1, ...); ..." will allow
 * multiple "appends" to "buf" (at the cost of losing the max-length info).
 *
 * For example: "s = buf; n = vstrnfmt(s+n, 100-n, ...); ..." will allow
 * multiple bounded "appends" to "buf", with constant access to "strlen(buf)".
 *
 * For example: "format("%-.*s", i, txt)" will produce a string containing
 * the first "i" characters of "txt", left justified.
 */


/**
 * Basic "vararg" format function.
 *
 * This function takes a buffer, a max byte count, a format string, and
 * a va_list of arguments to the format string, and uses the format string
 * and the arguments to create a string to the buffer.  The string is
 * derived from the format string and the arguments in the manner of the
 * "sprintf()" function, but with some extra "format" commands.  Note that
 * this function will never use more than the given number of bytes in the
 * buffer, preventing messy invalid memory references.  This function then
 * returns the total number of non-null bytes written into the buffer.
 *
 * Method: Let "str" be the (unlimited) created string, and let "len" be the
 * smaller of "max-1" and "strlen(str)".  We copy the first "len" chars of
 * "str" into "buf", place "\0" into buf[len], and return "len".
 *
 * In English, we do a sprintf() into "buf", a buffer with size "max",
 * and we return the resulting value of "strlen(buf)", but we allow some
 * special format commands, and we are more careful than "sprintf()".
 *
 * Typically, "max" is in fact the "size" of "buf", and thus represents
 * the "number" of chars in "buf" which are ALLOWED to be used.  An
 * alternative definition would have required "buf" to hold at least
 * "max+1" characters, and would have used that extra character only
 * in the case where "buf" was too short for the result.  This would
 * give an easy test for "overflow", but a less "obvious" semantics.
 *
 * Note that if the buffer was "too short" to hold the result, we will
 * always return "max-1", but we also return "max-1" if the buffer was
 * "just long enough".  We could have returned "max" if the buffer was
 * too short, not written a null, and forced the programmer to deal with
 * this special case, but I felt that it is better to at least give a
 * "usable" result when the buffer was too long instead of either giving
 * a memory overwrite like "sprintf()" or a non-terminted string like
 * "strncpy()".  Note that "strncpy()" also "null-pads" the result.
 *
 * Note that in most cases "just long enough" is probably "too short".
 *
 * As of 4.0, we use snprintf (for safety, and to quieten picky compilers)
 *
 * We should also consider extracting and processing the "width" and other
 * "flags" by hand, it might be more "accurate", and it would allow us to
 * remove the limit (1000 chars) on the result of format sequences.
 *
 * Also, some sequences, such as "%+d" by hand, do not work on all machines,
 * and could thus be correctly handled here.
 *
 * Error detection in this routine is not very graceful, in particular,
 * if an error is detected in the format string, we simply "pre-terminate"
 * the given buffer to a length of zero, and return a "length" of zero.
 * The contents of "buf", except for "buf[0]", may then be undefined.
 */
/* Before writing any prefix or earlier format field, inspect the original
 * narrow %s arguments.  An argument may point inside 'buf', and writing
 * even one literal byte can destroy it before the %s branch sees it.
 *
 * Match vstrnfmt's argument consumption, without formatting text or
 * dereferencing string contents except when a potential overlap needs
 * checking. Ordinary non-overlapping calls do not allocate memory. */
static bool angdroid_vstrnfmt_has_aliased_source(
	const char *fmt, const char *buf, size_t max, va_list vp)
{
	const char *cursor = fmt;
	va_list args;
	bool aliased = false;

	/* Formatting with fmt itself inside buf also requires protection. */
	if (angdroid_format_string_overlaps_output(fmt, buf, max))
		return true;
	if (!strchr(fmt, 's')) return false;

	va_copy(args, vp);
	while (*cursor) {
		bool long_arg = false;
		bool last_is_dot = false;
		size_t directive_size = 1;
		char conversion;

		if (*cursor++ != '%') continue;
		if (*cursor == '%') {
			++cursor;
			continue;
		}
		if (*cursor == 'n') {
			(void)va_arg(args, size_t *);
			++cursor;
			continue;
		}

		/* Width and precision '*' arguments are consumed before the
		 * conversion, including when their values are negative. */
		while (*cursor) {
			/* Match the real formatter's 100-character directive
			 * limit. Otherwise this lookahead may read varargs after
			 * the formatter would have stopped on an invalid format. */
			if (directive_size > 100) goto finished;
			if (*cursor == '*') {
				int width = va_arg(args, int);
				if (width < 0 && last_is_dot) {
					/* Negative dynamic precision removes its dot. */
					--directive_size;
				} else {
					int digits = snprintf(NULL, 0, "%d", width);
					if (digits < 0) goto finished;
					directive_size += (size_t)digits;
				}
				last_is_dot = false;
				++cursor;
			} else if (*cursor == 'l') {
				long_arg = true;
				last_is_dot = false;
				++directive_size;
				++cursor;
			} else if (isalpha((unsigned char)*cursor)) {
				break;
			} else {
				last_is_dot = (*cursor == '.');
				++directive_size;
				++cursor;
			}
		}
		if (!*cursor) break;
		conversion = *cursor++;
		switch (conversion) {
		case 's':
			if (long_arg) {
				(void)va_arg(args, const wchar_t *);
			} else {
				const char *value = va_arg(args, const char *);
				if (angdroid_format_string_overlaps_output(
						value, buf, max)) {
					aliased = true;
				}
			}
			break;
		case 'c':
		case 'd': case 'i':
			if (long_arg && conversion != 'c')
				(void)va_arg(args, long);
			else
				(void)va_arg(args, int);
			break;
		case 'u': case 'o': case 'x': case 'X':
			if (long_arg) (void)va_arg(args, unsigned long);
			else (void)va_arg(args, unsigned int);
			break;
		case 'f': case 'e': case 'E': case 'g': case 'G':
			(void)va_arg(args, double);
			break;
		case 'p':
			(void)va_arg(args, void *);
			break;
		default:
			/* An invalid format makes the real formatter return an
			 * error; no later argument can be formatted. */
			cursor += strlen(cursor);
			break;
		}
		/* No need to inspect later varargs once staging is required. */
		if (aliased) break;
	}
	finished:
	va_end(args);
	return aliased;
}

/* The existing parser/formatter writes directly into its destination.
 * Its caller supplies a non-aliasing scratch buffer when necessary. */
static size_t angdroid_vstrnfmt_unaliased(char *buf, size_t max,
	const char *fmt, va_list vp)
{
	const char *s;

	/* The argument is "long" */
	bool do_long;

	/* Bytes used in buffer */
	size_t n;

	/* Bytes used in format sequence */
	size_t q;

	/* Format sequence */
	char aux[128];

	/* Bytes produced by the current non-string format field. */
	int formatted;

	assert(max);
	assert(fmt);

	/* Begin the buffer */
	n = 0;

	/* Begin the format string */
	s = fmt;

	/* Scan the format string */
	while (true) {
		/* All done */
		if (!*s) break;

		/* Normal character */
		if (*s != '%') {
			/* Check total length */
			if (n == max-1) break;

			/* Save the character */
			buf[n++] = *s++;

			/* Continue */
			continue;
		}

		/* Skip the "percent" */
		s++;

		/* Pre-process "%%" */
		if (*s == '%') {
			/* Check total length */
			if (n == max-1) break;

			/* Save the percent */
			buf[n++] = '%';

			/* Skip the "%" */
			s++;

			/* Continue */
			continue;
		}

		/* Pre-process "%n" */
		if (*s == 'n') {
			size_t *arg;

			/* Get the next argument */
			arg = va_arg(vp, size_t *);

			/* Save the current length */
			(*arg) = n;

			/* Skip the "n" */
			s++;

			/* Continue */
			continue;
		}


		/* Begin the "aux" string */
		q = 0;

		/* Save the "percent" */
		aux[q++] = '%';

		do_long = false;

		/* Build the "aux" string */
		while (true) {
			/* Error -- format sequence is not terminated */
			if (!*s) {
				/* Terminate the buffer */
				buf[0] = '\0';

				/* Return "error" */
				return (0);
			}

			/* Error -- format sequence may be too long */
			if (q > 100) {
				/* Terminate the buffer */
				buf[0] = '\0';

				/* Return "error" */
				return (0);
			}

			/* Handle "alphabetic" or "non-alphabetic"chars */
			if (isalpha((unsigned char)*s)) {
				/* Hack -- handle "long" request */
				if (*s == 'l') {
					/* Save the character */
					aux[q++] = *s++;

					/* Note the "long" flag */
					do_long = true;
				} else {
					/* Save the character */
					aux[q++] = *s++;

					/* Stop processing the format sequence */
					break;
				}
			} else {
				/* Hack -- Handle 'star' (for "variable length" argument) */
				if (*s == '*') {
					int arg;

					/* Get the next argument */
					arg = va_arg(vp, int);

					/* C99 treats a negative dynamic precision as if
					 * there were no precision at all.  Previously
					 * "%.*ls" with -1 became "%.-1ls", which is
					 * not a valid printf conversion and could lose
					 * the entire translated string.  Negative field
					 * widths still retain their '-' alignment flag. */
					if (arg < 0 && q > 0 && aux[q - 1] == '.') {
						aux[--q] = '\0';
					} else {
						snprintf(aux + q, sizeof(aux) - q, "%d", arg);
						while (aux[q]) q++;
					}

					/* Skip the "*" */
					s++;
				} else {
					/* Save the normal character (digits, "-", "+", ".", etc) */
					aux[q++] = *s++;
				}
			}
		}

		/* Terminate "aux" */
		aux[q] = '\0';

		/* Process the "format" symbol */
		switch (aux[q - 1]) {
			/* Simple Character -- standard format */
			case 'c':
			{
				int arg;

				/* Get the next argument */
				arg = va_arg(vp, int);

				/* Format the argument */
				formatted = snprintf(buf + n, max - n, aux, arg);

				/* Done */
				break;
			}

			/* Signed Integers -- standard format */
			case 'd': case 'i':
			{
				if (do_long) {
					long arg;

					/* Get the next argument */
					arg = va_arg(vp, long);

					/* Format the argument */
					formatted = snprintf(buf + n, max - n, aux, arg);
				} else {
					int arg;

					/* Get the next argument */
					arg = va_arg(vp, int);

					/* Format the argument */
					formatted = snprintf(buf + n, max - n, aux, arg);
				}

				/* Done */
				break;
			}

			/* Unsigned Integers -- various formats */
			case 'u': case 'o': case 'x': case 'X':
			{
				if (do_long) {
					unsigned long arg;

					/* Get the next argument */
					arg = va_arg(vp, unsigned long);

					/* Format the argument */
					formatted = snprintf(buf + n, max - n, aux, arg);
				} else {
					unsigned int arg;

					/* Get the next argument */
					arg = va_arg(vp, unsigned int);

					/* Format the argument */
					formatted = snprintf(buf + n, max - n, aux, arg);
				}

				/* Done */
				break;
			}

			/* Floating Point -- various formats */
			case 'f':
			case 'e': case 'E':
			case 'g': case 'G':
			{
				double arg;

				/* Get the next argument */
				arg = va_arg(vp, double);

				/* Format the argument */
				formatted = snprintf(buf + n, max - n, aux, arg);

				/* Done */
				break;
			}

			/* Pointer -- implementation varies */
			case 'p':
			{
				void *arg;

				/* Get the next argument */
				arg = va_arg(vp, void*);

				/* Format the argument */
				formatted = snprintf(buf + n, max - n, aux, arg);

				/* Done */
				break;
			}

			/* String */
			case 's':
			{
				if (do_long) {
					const wchar_t *arg = va_arg(vp, const wchar_t *);
					const char *dot;
					size_t wide_len, max_chars, capacity, max_bytes;
					int max_encoded_bytes;
					size_t used = 0, index, precision = (size_t)-1;
					char *utf8;
					int bytes, converted, written;
					size_t remaining;

					if (!arg) arg = L"";

					/* The Android Unicode hook supplies a real UTF-8
					 * encoder. Casting wchar_t to char discarded every
					 * non-ASCII character, and the old fixed buffer
					 * silently clipped any name longer than 767 chars. */
					max_encoded_bytes = text_wcsz();
					if (max_encoded_bytes <= 0) {
						buf[0] = '\0';
						return 0;
					}
					max_bytes = (size_t)max_encoded_bytes;

					/* printf precision on a string is measured in
					 * bytes. Never emit only part of a UTF-8 scalar,
					 * including when the format contains a '*' width
					 * or precision already expanded into aux. */
					dot = strchr(aux, '.');
					if (dot) {
						unsigned digit;
						precision = 0;
						for (++dot; *dot >= '0' && *dot <= '9'; ++dot) {
							digit = (unsigned)(*dot - '0');
							if (precision > ((size_t)-1 - digit) / 10) {
								precision = (size_t)-1;
								break;
							}
							precision = precision * 10 + digit;
						}
					}

					wide_len = text_wcslen(arg);
					max_chars = wide_len;
					if (max_chars > precision) max_chars = precision;
					if (max_chars > ((size_t)-1 - 1) / max_bytes) {
						buf[0] = '\0';
						return 0;
					}
					capacity = max_chars * max_bytes + 1;
					utf8 = mem_alloc(capacity);
					for (index = 0; index < max_chars; ++index) {
						converted = text_wctomb(utf8 + used, arg[index]);
						if (converted <= 0) {
							utf8[used++] = '?';
							continue;
						}
						bytes = converted;
						if ((size_t)bytes > capacity - used - 1 ||
							(size_t)bytes > precision - used)
							break;
						used += (size_t)bytes;
					}
					utf8[used] = '\0';

					/* Keep width/alignment/precision semantics from the
					 * original narrow %s formatting route. */
					aux[q-2] = 's';
					aux[q-1] = '\0';
					remaining = max - n;
					written = snprintf(buf + n, remaining, aux, utf8);
					mem_free(utf8);
					if (written < 0) {
						buf[0] = '\0';
						return 0;
					}
					if ((size_t)written >= remaining) {
						n = max - 1;
						buf[n] = '\0';
						angdroid_utf8_repair_truncated_tail(buf, n);
						return n;
					}
					n += (size_t)written;
					continue;
				} else {
					const char *arg;
					const char *formatted;
					const char *dot;
					char *safe_copy = NULL;
					char *overlap_copy = NULL;
					size_t precision = (size_t)-1;
					size_t remaining;
					int written;

					/* Never split a multibyte glyph when printf applies
					 * a byte-based %.Ns precision to translated prose.
					 * The ordinary printf flags and column padding
					 * remain in charge after selecting a safe prefix. */
					arg = va_arg(vp, const char *);
					if (!arg) arg = "";
					formatted = arg;
					dot = strchr(aux, '.');
					if (dot) {
						const char *digit = dot + 1;
						precision = 0;
						while (*digit >= '0' && *digit <= '9') {
							unsigned value = (unsigned)(*digit - '0');
							if (precision > ((size_t)-1 - value) / 10) {
								precision = (size_t)-1;
								break;
							}
							precision = precision * 10 + value;
							++digit;
						}
						if (precision != (size_t)-1) {
							size_t safe_bytes =
								angdroid_utf8_precision_prefix_bytes(
									arg, precision);
							if ((safe_bytes < precision && arg[safe_bytes]) ||
								angdroid_utf8_prefix_contains_invalid(
									arg, safe_bytes)) {
								safe_copy = mem_alloc(safe_bytes + 1);
								angdroid_utf8_copy_sanitized_prefix(
									safe_copy, arg, safe_bytes);
								formatted = safe_copy;
							}
						}
					}

					/* Write directly to the caller's output buffer,
					 * rather than its former fixed 1024-byte staging
					 * buffer. The caller can retry with more space. */
					remaining = max - n;
					if (angdroid_format_string_overlaps_output(
							formatted, buf + n, remaining)) {
						/* A direct snprintf() from buf (or from its
						 * existing prefix in strnfcat()) is undefined.
						 * Preserve the source before overwriting it. */
						size_t copy_length = strlen(formatted) + 1;
						overlap_copy = mem_alloc(copy_length);
						memcpy(overlap_copy, formatted, copy_length);
						formatted = overlap_copy;
					}
					written = snprintf(buf + n, remaining, aux, formatted);
					mem_free(overlap_copy);
					mem_free(safe_copy);
					if (written < 0) {
						buf[0] = '\0';
						return 0;
					}
					if ((size_t)written >= remaining) {
						n = max - 1;
						buf[n] = '\0';
						angdroid_utf8_repair_truncated_tail(buf, n);
						return n;
					}
					n += (size_t)written;
					continue;
				}
			}

			/* Oops */
			default:
			{
				/* Error -- illegal format char */
				buf[0] = '\0';

				/* Return "error" */
				return (0);
			}
		}

		/* Numeric, character, float and pointer conversions must also
		 * use the caller's buffer. A fixed 1024-byte staging area silently
		 * truncated wide numeric fields and defeated vformat() retries. */
		if (formatted < 0) {
			buf[0] = '\0';
			return 0;
		}
		if ((size_t)formatted >= max - n) {
			n = max - 1;
			buf[n] = '\0';
			angdroid_utf8_repair_truncated_tail(buf, n);
			return n;
		}
		n += (size_t)formatted;
	}


	/* Keep fixed-size translated text valid even when the final
	 * UTF-8 scalar crosses the caller's byte limit. The byte count
	 * remains unchanged so dynamic-buffer callers still retry. */
	buf[n] = '\0';
	if (n == max - 1)
		angdroid_utf8_repair_truncated_tail(buf, n);

	/* Return length */
	return (n);
}


/* Keep the legacy bounded length contract.  Allocate only when an input
 * string or the format itself aliases the output buffer: %s may be preceded
 * by literal text, padding, or numeric fields that would destroy its source
 * before the conversion is reached. */
size_t vstrnfmt(char *buf, size_t max, const char *fmt, va_list vp)
{
	char *scratch;
	size_t result;
	va_list args;

	assert(max);
	assert(fmt);
	if (!angdroid_vstrnfmt_has_aliased_source(fmt, buf, max, vp))
		return angdroid_vstrnfmt_unaliased(buf, max, fmt, vp);

	scratch = mem_alloc(max);
	va_copy(args, vp);
	result = angdroid_vstrnfmt_unaliased(scratch, max, fmt, args);
	va_end(args);
	memcpy(buf, scratch, result + 1);
	mem_free(scratch);
	return result;
}

/**
 * Add a formatted string to the end of a string
 */
void strnfcat(char *str, size_t max, size_t *end, const char *fmt, ...)
{
	size_t len;

	va_list vp;

	/* Paranoia */
	if (*end >= max) return;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Build the string */
	len = vstrnfmt(&str[*end], max - *end, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Change the end value */
	*end += len;
}


static char *format_buf = NULL;
static size_t format_len = 0;


/**
 * Do a vstrnfmt (see above) into a (growable) static buffer.
 * This buffer is usable for very short term formatting of results.
 */
char *vformat(const char *fmt, va_list vp)
{
	/* Initial allocation */
	if (!format_buf) {
		format_len = 1024;
		format_buf = mem_zalloc(format_len);
		format_buf[0] = 0;
	}

	/* Null format yields last result */
	if (!fmt) return (format_buf);

	/* Keep going until successful */
	while (1) {
		va_list	args;
		size_t len;

		/* Build the string */
		va_copy(args, vp);
		len = vstrnfmt(format_buf, format_len, fmt, args);
		va_end(args);

		/* Success */
		if (len < format_len-1) break;

		/* Grow the buffer */
		format_len = format_len * 2;
		format_buf = mem_realloc(format_buf, format_len);
	}

	/* Return the new buffer */
	return (format_buf);
}

void vformat_kill(void)
{
	mem_free(format_buf);
}


/**
 * Do a vstrnfmt (see above) into a buffer of a given size.
 */
size_t strnfmt(char *buf, size_t max, const char *fmt, ...)
{
	size_t len;

	va_list vp;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Do the va_arg fmt to the buffer */
	len = vstrnfmt(buf, max, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Return the number of bytes written */
	return (len);
}


/**
 * Do a vstrnfmt() into (see above) into a (growable) static buffer.
 * This buffer is usable for very short term formatting of results.
 * Note that the buffer is (technically) writable, but only up to
 * the length of the string contained inside it.
 */
char *format(const char *fmt, ...)
{
	char *res;
	va_list vp;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args */
	res = vformat(fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Return the result */
	return (res);
}




/**
 * Vararg interface to plog()
 */
void plog_fmt(const char *fmt, ...)
{
	char *res;
	va_list vp;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args */
	res = vformat(fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Call plog */
	plog(res);
}



/**
 * Vararg interface to quit()
 */
void quit_fmt(const char *fmt, ...)
{
	char *res;
	va_list vp;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format */
	res = vformat(fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Call quit() */
	quit(res);
}
