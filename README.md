![Cosmopolitan Honeybadger](usr/share/img/honeybadger.png)

# Cosmopolitan

[Cosmopolitan Libc](https://justine.lol/cosmopolitan/index.html) makes C
a build-once run-anywhere language, like Java, except it doesn't need an
interpreter or virtual machine. Instead, it reconfigures stock GCC and
Clang to output a POSIX-approved polyglot format that runs natively on
Linux + Mac + Windows + FreeBSD + OpenBSD + NetBSD + BIOS with the best
possible performance and the tiniest footprint imaginable.

## Background

For an introduction to this project, please read the [αcτµαlly pδrταblε
εxεcµταblε](https://justine.lol/ape.html) blog post and [cosmopolitan
libc](https://justine.lol/cosmopolitan/index.html) website. We also have
[API documentation](https://justine.lol/cosmopolitan/documentation.html).

## Getting Started

If you're doing your development work on Linux or BSD then you need just
five files to get started. Here's what you do on Linux:

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan-amalgamation-0.3.zip
unzip cosmopolitan-amalgamation-0.3.zip
printf 'main() { printf("hello world\\n"); }\n' >hello.c
gcc -g -Os -static -nostdlib -nostdinc -fno-pie -no-pie -mno-red-zone \
  -fno-omit-frame-pointer -pg -mnop-mcount \
  -o hello.com.dbg hello.c -fuse-ld=bfd -Wl,-T,ape.lds \
  -include cosmopolitan.h crt.o ape.o cosmopolitan.a
objcopy -S -O binary hello.com.dbg hello.com
```

You now have a portable program. Please note that your APE binary will
assimilate itself as a conventional resident of your platform after the
first run, so it can be fast and efficient for subsequent executions.

```sh
./hello.com
bash -c './hello.com'  # zsh/fish workaround (we upstreamed a patch)
```

So if you intend to copy the binary to Windows or Mac then please do
that before you run it, not after.

If you're developing on Windows or MacOS then you need to download an
x86_64-pc-linux-gnu toolchain beforehand. See the [Compiling on
Windows](https://justine.lol/cosmopolitan/windows-compiling.html)
tutorial. It's needed because the ELF object format is what makes
universal binaries possible.

Cosmopolitan can also be compiled from source on any Linux distro.

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan-0.3.tar.gz
tar xf cosmopolitan-0.3.tar.gz  # see releases page
cd cosmopolitan
make -j16
o//examples/hello.com
find o -name \*.com | xargs ls -rShal | less
```

## Support Vector

| Platform        | Min Version | Circa |
| :---            | ---:        | ---:  |
| AMD             | K8 Venus    | 2005  |
| Intel           | Core        | 2006  |
| New Technology  | Vista       | 2006  |
| GNU/Systemd     | 2.6.18      | 2007  |
| XNU's Not UNIX! | 15.6        | 2018  |
| FreeBSD         | 12          | 2018  |
| OpenBSD         | 6.4         | 2018  |
| NetBSD          | 9.1         | 2020  |
