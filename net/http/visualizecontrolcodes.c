/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpenc.h"
#include "net/http/http.h"

/**
 * Filters out control codes from string.
 *
 * This is useful for logging data like HTTP messages, where we don't
 * want full blown C string literal escaping, but we don't want things
 * like raw ANSI control codes from untrusted devices in our terminals.
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length on success
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *VisualizeControlCodes(const char *data, size_t size, size_t *out_size) {
  uint64_t w;
  char *r, *q;
  unsigned i, n;
  wint_t x, a, b;
  const char *p, *e;
  if (size == -1) size = strlen(data);
  if ((r = malloc(size * 6 + 1))) {
    q = r;
    p = data;
    e = p + size;
    while (p < e) {
      x = *p++ & 0xff;
      if (x >= 0300) {
        a = ThomPikeByte(x);
        n = ThomPikeLen(x) - 1;
        if (p + n <= e) {
          for (i = 0;;) {
            b = p[i] & 0xff;
            if (!ThomPikeCont(b)) break;
            a = ThomPikeMerge(a, b);
            if (++i == n) {
              x = a;
              p += i;
              break;
            }
          }
        }
      }
      if (0x80 <= x && x < 0xA0) {
        q[0] = '\\';
        q[1] = 'u';
        q[2] = '0';
        q[3] = '0';
        q[4] = "0123456789abcdef"[(x & 0xF0) >> 4];
        q[5] = "0123456789abcdef"[(x & 0x0F) >> 0];
        q += 6;
      } else {
        if (0x00 <= x && x < 0x20) {
          if (x != '\t' && x != '\r' && x != '\n') {
            x += 0x2400; /* Control Pictures */
          }
        } else if (x == 0x7F) {
          x = 0x2421;
        }
        w = tpenc(x);
        do {
          *q++ = w;
        } while ((w >>= 8));
      }
    }
    if (out_size) *out_size = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
