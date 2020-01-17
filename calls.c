#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <fcntl.h>
#include <linux/limits.h>

enum
{
    DIGITS = 10,
    BUFFER_ENDING = 3
};

void
die(int condition) {
    if (condition) {
        exit(EXIT_FAILURE);
    }
}

enum
{
    BUFFER_SIZE = 16
};

void
die(int condition) {
    if (condition) {
        exit(EXIT_FAILURE);
    }
}

struct Reader {
    char buf[BUFFER_SIZE];
    ssize_t pos, len;
};

int
fast_c(struct Reader *r) {
    if (r->pos >= r->len) {
        r->len = read(STDIN_FILENO, r->buf, BUFFER_SIZE);
        die(r->len < 0);
        r->pos = 0;
    }
    if (r->pos == r->len) {
        return EOF;
    }
    (r->pos)++;
    return r->buf[r->pos - 1];
}

long long unsigned int
fast_llu(struct Reader *r) {
    int c;
    while ((c = fast_c(r)) != EOF && isspace((unsigned char)c)) {
        ;
    }
    errno = 0;
    if (c == EOF) {
        errno = 1;
        return 0;
    }

    int sign = 0;
    long long unsigned int res = 0;
    if (c == '-') {
        sign = 1;
        c = fast_c(r);
    } else if (c == '+') {
        c = fast_c(r);
    }

    do {
        res *= 10;
        if (sign) {
            res -= c - '0';
        } else {
            res += c - '0';
        }
    } while ((c = fast_c(r)) != EOF && !isspace(c));

    return res;
}

int
main(void) {
    int w;
    int32_t n;
    scanf("%" SCNo32 "%o", &n, &w);
    int32_t h = 1 << (n - 1);
    n = 1 << n;
    for (int32_t i = 0; i < n; i++) {
        int32_t j = i;
        if (i >= h) {
            j = h - i;
        }
        printf("|%*" PRIo32 "|%*" PRIu32 "|%*" PRId32 "|\n", w, i, w, i, w, j);
    }
    return 0;
}

void normalize_path(char *buf) {
    char *last = buf;
    while (*buf != '\0') {
        while (*buf != '\0' && *buf != '/') {
            *last = *buf;
            last++;
            buf++;
        }
        if (*buf == '/' ) {
            last++;
            while (*buf == '/') {
                buf++;
            }
        }
    }
    *last = *buf;
}

int
main(int argc, char **argv) {
    int inf = open(argv[1], O_RDWR);
    die(inf < 0);
    
    int ret = 0;
    int num_inf = 1;
    size_t pos = 0;
    long long int mn = LLONG_MAX;
    long long int buf;

    size_t i = 0;
    while ((ret = read(inf, &buf, sizeof(buf))) == sizeof(buf)) {
        die(ret < 0);
        if (buf < mn || num_inf) {
            num_inf = 0;
            mn = buf;
            pos = i;
        }
        i++;
    }
    
    if (!num_inf) {
        long long unsigned out_buf = (long long unsigned)mn;
        out_buf = -out_buf;
        die(lseek(inf, pos * sizeof(buf), SEEK_SET) == -1);
        die(write(inf, &out_buf, sizeof(out_buf)) < 0);
    }
    die(close(inf) == -1);
    
    return 0;
}

int
main(void) {
    char buffer[PATH_MAX + BUFFER_ENDING];

    char *ret = fgets(buffer, sizeof(buffer), stdin);
    die(ret == NULL);
    FILE *inf = fopen(buffer, "r");
    die(inf == NULL);
    r = fread(buffer, sizeof(*buffer), sizeof(buffer), inf);
    int r = atoi(argv[i]);
    memset(buffer, 0, size);
    return 0;
}

int
readdate(struct tm *t, FILE *fd) {
    int32_t timestamp[6];
    int ret = fscanf(fd, "%d/%d/%d %d:%d:%d",
            &timestamp[0],
            &timestamp[1],
            &timestamp[2],
            &timestamp[3],
            &timestamp[4],
            &timestamp[5]);
    if (ret != 6) {
        return 1;
    }

    t->tm_year = timestamp[0] - 1900;
    t->tm_mon = timestamp[1] - 1;
    t->tm_mday = timestamp[2];

    t->tm_hour = timestamp[3];
    t->tm_min = timestamp[4];
    t->tm_sec = timestamp[5];
    t->tm_isdst = -1;
    return 0;
}

int
main(int argc, char **argv) {
    FILE *inf = fopen(argv[1], "r");

    struct tm t = {0};
    die(readdate(&t, inf) != 0);
    time_t t_prev = mktime(&t);

    while (readdate(&t, inf) == 0) {
        time_t t_cur = mktime(&t);
        printf("%ld\n", t_cur - t_prev);
        t_prev = t_cur;
    }

    fclose(inf);

    return 0;
}

#include <string.h>

const char perm[] = "rwxrwxrwx";

int
parse_rwx_permissions(const char *str) {
    if (str == NULL) {
        return -1;
    }
    int len = sizeof(perm) - 1;
    if (strlen(str) != len) {
        return -1;
    }
    int res = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == perm[i]) {
            res |= 1 << (len - i - 1);
        } else if (str[i] != '-') {
            return -1;
        }
    }
    return res;
}

int
main(int argc, char **argv) {
    DIR *dir = opendir(argv[1]);
    die(dir == NULL);
    struct dirent *entry = readdir(dir);
    long long unsigned s = 0;
    while (entry) {
        char path[PATH_MAX] = {0};
        strcat(strcat(strcat(path, argv[1]), "/"), entry->d_name);
        struct stat stb;
        int res = stat(path, &stb);
        if (res == 0 &&
            S_ISREG(stb.st_mode) && stb.st_uid == getuid() &&
            'A' <= entry->d_name[0] && entry->d_name[0] <= 'Z') {
            s += stb.st_size;
        }

        entry = readdir(dir);
    }

    printf("%llu\n", s);
    die(closedir(dir) == -1);
    /*
        struct stat buf;
        int res = lstat(argv[i], &buf);
        if (res == 0 &&
            S_ISREG(buf.st_mode) && buf.st_nlink <= 1) {
            s += buf.st_size;
        }
    */
    return 0;
}

int
main(int argc, char **argv) {
    struct tm begin_t = {0};
    begin_t.tm_year = ROYAL_YEAR - TM_YEAR;
    begin_t.tm_mon = ROYAL_MONTH - 1;
    begin_t.tm_mday = ROYAL_DAY;
    long long orig = mktime(&begin_t);
    
    long long year, month, day;
    while (scanf("%lld%lld%lld", &year, &month, &day) == 3) {
        struct tm t = {0};
        t.tm_year = year - TM_YEAR;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        long long cur = ((long long) mktime(&t) - orig + t.tm_gmtoff) / SECS_DAY;

        printf("%lld %lld %lld\n", cur / MONTHS / VIRTUAL_MONTH + 1,
            cur / VIRTUAL_MONTH % MONTHS + 1, cur % VIRTUAL_MONTH + 1);
    }

    return 0;
}