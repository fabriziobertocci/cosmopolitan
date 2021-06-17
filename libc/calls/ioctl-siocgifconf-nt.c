/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/iff.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/nt/winsock.h"

// TODO: Remove me
#include "libc/stdio/stdio.h"
#define PRINTF (weaken(printf))

#define MAX_INTERFACES  32

/* Reference:
 *  - Description of ioctls: https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
 *  - Structure INTERFACE_INFO: https://docs.microsoft.com/en-us/windows/win32/api/ws2ipdef/ns-ws2ipdef-interface_info
 *  - WSAIoctl man page: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaioctl
 *  - Using SIOCGIFCONF in Win32: https://docs.microsoft.com/en-us/windows/win32/winsock/using-unix-ioctls-in-winsock
 */

textwindows int ioctl_siocgifconf_nt(int fd, struct ifconf *ifc) {
  struct NtInterfaceInfo iflist[MAX_INTERFACES];
  uint32_t dwBytes;
  int ret;
  int i, count;

  PRINTF(">>> ioctl_siocgifconf_nt starting\n");

  if (g_fds.p[fd].kind != kFdSocket) {
    return ebadf();
  }

  PRINTF(">>> WSAIoctl=%p, WSAGetLastError=%p\n", weaken(WSAIoctl), weaken(WSAGetLastError));
  ret = weaken(WSAIoctl)(g_fds.p[fd].handle, kNtSioGetInterfaceList, NULL, 0, &iflist, sizeof(iflist), &dwBytes, NULL, NULL);
  if (ret == -1) {
    PRINTF(">>> WSAIoctl failed with error: %d\n", weaken(__winsockerr)());
    return weaken(__winsockerr)();
  }

  count = dwBytes / sizeof(struct NtInterfaceInfo);

  PRINTF("CI> SIO_GET_INTERFACE_LIST success:\n");
  for (i = 0; i < count; ++i) {
    PRINTF("CI>\t #i address: %08x\n", i, iflist[i].iiAddress.sin_addr.s_addr);
    PRINTF("CI>\t #i bcast  : %08x\n", i, iflist[i].iiBroadcastAddress.sin_addr.s_addr);
    PRINTF("CI>\t #i netmask: %08x\n", i, iflist[i].iiNetmask.sin_addr.s_addr);
    PRINTF("CI>\t #i flags  : %08x - ", i, iflist[i].iiFlags);
    if (iflist[i].iiFlags & IFF_UP) PRINTF("IFF_UP ");
    if (iflist[i].iiFlags & IFF_BROADCAST) PRINTF("IFF_BROADCAST ");
    if (iflist[i].iiFlags & IFF_LOOPBACK) PRINTF("IFF_LOOPBACK ");
    if (iflist[i].iiFlags & IFF_POINTOPOINT) PRINTF("IFF_POINTOPOINT ");
    if (iflist[i].iiFlags & IFF_MULTICAST) PRINTF("IFF_MULTICAST ");
    PRINTF("\n");
  }

  return ret;
}

