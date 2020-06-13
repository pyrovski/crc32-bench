#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

void crc32_init(void);
void crc32_shutdown(void);

uint32_t crc32(uint32_t, uint8_t *, size_t);

#define BUFLEN (64*1024*1024)
#define ROUNDS (10)

int main(int argc, char **argv) {
    int buflen = 0;
    if (argc > 1)
        buflen = atoi(argv[1]);
    if (buflen <= 0)
        buflen = BUFLEN;

    if (buflen / sizeof(long int) * sizeof(long int) != buflen) {
        printf("buflen must be multiple of %d\n", sizeof(long int));
        return 1;
    }

    int rounds = 0;
    if (argc > 2)
        rounds = atoi(argv[2]);
    if (rounds <= 0)
        rounds = ROUNDS;

    printf("%d bytes, %d rounds\n", buflen, rounds);

    long int *buf = malloc(buflen);
    srand48(0);
    for (int i = 0; i < buflen/sizeof(long int); i++)
        buf[i] = mrand48();

    crc32_init();

    struct timeval start, end;
    unsigned long crc = 0;

    gettimeofday(&start, 0);
    for (int i = rounds; i > 0; i--)
        crc = crc32(0, (unsigned char *) buf, buflen);
    gettimeofday(&end, 0);

    struct timeval res;
    timersub(&end, &start, &res);

    double run = ((double) res.tv_sec*1000000 + res.tv_usec) / 1000000;
    double avg = run/rounds;
    double throughput = buflen / avg;

    printf("crc 0x%lx, runtime %.2f (avg %.5f, %f Bps)\n", crc, run, avg,
	   throughput);

    crc32_shutdown();

    return 0;
}
