// rge256_drbg_cli.c
// Linux CLI for RGE256-DRBG
//
// - Seeds from getrandom() if available, else /dev/urandom
// - Streams bytes to stdout for dieharder / practrand / smokerand pipelines
// - --stream-id domain-separates streams via nonce mapping
// - --kat runs a Known Answer Test (CI-friendly)
//
// Build:
//   cc -O3 -std=c99 rge256_drbg.c rge256_drbg_cli.c -o rge256_drbg
//
// Usage:
//   ./rge256_drbg > out.bin
//   ./rge256_drbg --bytes 1000000 > out.bin
//   ./rge256_drbg --reseed-bytes 65536 > out.bin
//   ./rge256_drbg --stream-id 2 --bytes 64 | sha256sum
//   ./rge256_drbg --kat

#include "rge256_drbg.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#if defined(__linux__)
  #include <sys/random.h>
#endif

static void store32_le(uint8_t out[4], uint32_t x) {
    out[0] = (uint8_t)(x);
    out[1] = (uint8_t)(x >> 8);
    out[2] = (uint8_t)(x >> 16);
    out[3] = (uint8_t)(x >> 24);
}

static int os_random(void* buf, size_t n) {
#if defined(__linux__)
    size_t off = 0;
    while (off < n) {
        ssize_t r = getrandom((uint8_t*)buf + off, n - off, 0);
        if (r < 0) { if (errno == EINTR) continue; break; }
        off += (size_t)r;
    }
    if (off == n) return 0;
#endif
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    size_t off2 = 0;
    while (off2 < n) {
        ssize_t r = read(fd, (uint8_t*)buf + off2, n - off2);
        if (r < 0) { if (errno == EINTR) continue; close(fd); return -1; }
        if (r == 0) { close(fd); return -1; }
        off2 += (size_t)r;
    }
    close(fd);
    return 0;
}

static void print_hex(const uint8_t* b, size_t n) {
    for (size_t i = 0; i < n; i++) printf("%02x", (unsigned)b[i]);
    printf("\n");
}

static void usage(const char* argv0) {
    fprintf(stderr,
        "Usage: %s [--bytes N] [--reseed-bytes M] [--stream-id ID] [--kat]\n"
        "\n"
        "  --bytes N         output exactly N bytes then stop (default: infinite)\n"
        "  --reseed-bytes M  reseed from OS every M bytes (default: 0 = never)\n"
        "  --stream-id ID    domain-separate streams via nonce = (lo32, hi32, 'RGE1')\n"
        "  --kat             run Known Answer Test and exit\n",
        argv0
    );
}

static int parse_u64(const char* s, uint64_t* out) {
    if (!s || !*s) return -1;
    char* end = NULL;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 0);
    if (errno != 0 || end == s || *end != '\0') return -1;
    *out = (uint64_t)v;
    return 0;
}

static int run_kat(void) {
    // KAT for this DRBG design:
    // key = 00..1f, nonce = 00..0b, then generate 64 bytes (no additional input)
    uint8_t key[32];
    uint8_t nonce[12];
    for (int i = 0; i < 32; i++) key[i] = (uint8_t)i;
    for (int i = 0; i < 12; i++) nonce[i] = (uint8_t)i;

    static const uint8_t expected[64] = {
        0x2c,0x85,0x72,0x1c,0xf7,0x49,0xd3,0x58,0xcf,0x1f,0x5f,0xe1,0x10,0x78,0xb9,0xf6,
        0xba,0x84,0xd5,0x43,0x2c,0x17,0x67,0xeb,0x45,0xe9,0x0d,0x8a,0x77,0x1d,0xbb,0x2c,
        0x24,0xe9,0xe2,0xca,0xa0,0x1e,0xef,0x57,0xac,0xca,0x2f,0x03,0xdf,0x4e,0x2e,0x8f,
        0x6d,0xfe,0xf9,0xf5,0xc4,0x05,0x1f,0xb7,0x2c,0x32,0x70,0x29,0x42,0xa1,0xdd,0xd7
    };

    rge256_drbg_state st;
    rge256_drbg_init(&st, key, nonce);

    uint8_t out[64];
    rge256_drbg_generate(&st, out, sizeof(out), NULL, 0);

    int ok = (memcmp(out, expected, sizeof(out)) == 0);

    printf("RGE256-DRBG KAT: %s\n", ok ? "PASS" : "FAIL");
    printf("expected: ");
    print_hex(expected, sizeof(expected));
    printf("got     : ");
    print_hex(out, sizeof(out));

    return ok ? 0 : 1;
}

int main(int argc, char** argv) {
    uint64_t total = 0;
    uint64_t reseed_every = 0;
    uint64_t stream_id = 0;
    int have_stream_id = 0;
    int kat_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--bytes")) {
            if (i + 1 >= argc) { usage(argv[0]); return 2; }
            total = strtoull(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--reseed-bytes")) {
            if (i + 1 >= argc) { usage(argv[0]); return 2; }
            reseed_every = strtoull(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--stream-id")) {
            if (i + 1 >= argc) { usage(argv[0]); return 2; }
            if (parse_u64(argv[++i], &stream_id) != 0) {
                fprintf(stderr, "ERROR: invalid --stream-id\n");
                return 2;
            }
            have_stream_id = 1;
        } else if (!strcmp(argv[i], "--kat")) {
            kat_mode = 1;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown arg: %s\n", argv[i]);
            usage(argv[0]);
            return 2;
        }
    }

    if (kat_mode) return run_kat();

    uint8_t key[32];
    uint8_t nonce[12];

    if (os_random(key, sizeof(key)) != 0 || os_random(nonce, sizeof(nonce)) != 0) {
        fprintf(stderr, "ERROR: OS randomness failed\n");
        return 1;
    }

    // Stream separation: override nonce from stream-id
    // nonce = (lo32(stream_id), hi32(stream_id), 'RGE1')
    if (have_stream_id) {
        uint32_t lo = (uint32_t)(stream_id & 0xffffffffu);
        uint32_t hi = (uint32_t)(stream_id >> 32);
        store32_le(&nonce[0], lo);
        store32_le(&nonce[4], hi);
        store32_le(&nonce[8], 0x31454752u); // 'RGE1' in LE bytes
    }

    rge256_drbg_state st;
    rge256_drbg_init(&st, key, nonce);
    memset(key, 0, sizeof(key));
    memset(nonce, 0, sizeof(nonce));

    uint8_t buf[16384];
    uint64_t produced = 0;
    uint64_t until_reseed = reseed_every;

    for (;;) {
        size_t want = sizeof(buf);

        if (total) {
            uint64_t rem = total - produced;
            if (rem == 0) break;
            if (rem < want) want = (size_t)rem;
        }

        if (reseed_every && until_reseed == 0) {
            uint8_t seed[RGE256_DRBG_SEED_BYTES];
            if (os_random(seed, sizeof(seed)) != 0) {
                fprintf(stderr, "ERROR: reseed OS randomness failed\n");
                return 1;
            }
            rge256_drbg_reseed(&st, seed);
            memset(seed, 0, sizeof(seed));
            until_reseed = reseed_every;
        }

        rge256_drbg_generate(&st, buf, want, NULL, 0);
        if (fwrite(buf, 1, want, stdout) != want) return 0;

        produced += want;
        if (reseed_every) {
            if (until_reseed >= want) until_reseed -= want;
            else until_reseed = 0;
        }
    }

    return 0;
}
