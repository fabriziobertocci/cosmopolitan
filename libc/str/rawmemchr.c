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
#include "libc/assert.h"
#include "libc/str/str.h"

static inline noasan uint64_t UncheckedAlignedRead64(unsigned char *p) {
  return (uint64_t)p[7] << 070 | (uint64_t)p[6] << 060 | (uint64_t)p[5] << 050 |
         (uint64_t)p[4] << 040 | (uint64_t)p[3] << 030 | (uint64_t)p[2] << 020 |
         (uint64_t)p[1] << 010 | (uint64_t)p[0] << 000;
}

/**
 * Returns pointer to first instance of character.
 *
 * @param m is memory to search
 * @param c is search byte which is masked with 255
 * @return is pointer to first instance of c
 */
void *rawmemchr(const void *m, int c) {
  uint64_t v, w;
  const unsigned char *s;
  s = m;
  c &= 255;
  v = 0x0101010101010101ul * c;
  for (; (uintptr_t)s & 7; ++s) {
    if (*s == c) return s;
  }
  for (;; s += 8) {
    w = UncheckedAlignedRead64(s);
    if ((w = ~(w ^ v) & ((w ^ v) - 0x0101010101010101) & 0x8080808080808080)) {
      s += (unsigned)__builtin_ctzll(w) >> 3;
      break;
    }
  }
  assert(*s == c);
  return s;
}
