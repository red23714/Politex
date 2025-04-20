//52 3087 228 1953125 69 8505 4000 807831573486328125
#include <stdio.h>
#include <stdlib.h>

#define ll unsigned long long

typedef struct {
    ll *data;
    int size;
} dynarr;

void init_buffer(dynarr *buf)
{
    buf->size = 1;
    buf->data = malloc(sizeof(ll));
    buf->data[0] = 1;
}

void add_element(dynarr *buf, ll value) 
{
    buf->data = realloc(buf->data, (buf->size + 1) * sizeof(ll));
    buf->data[buf->size++] = value;
}

void compact_buffer(dynarr *buf, int new_start) 
{   
    int new_size = buf->size - new_start;
    for (int i = 0; i < new_size; i++) 
    {
        buf->data[i] = buf->data[new_start + i];
    }
    
    buf->size = new_size;
    buf->data = realloc(buf->data, buf->size * sizeof(ll));
}

ll give_n_sequence(int n) 
{
    dynarr buf;
    init_buffer(&buf);

    int i3 = 0, i5 = 0, i7 = 0;
    int last_compacted = 0;
    int prev_min_used = 0;

    for (int i = 1; i <= n; i++) 
    {
        ll next3 = buf.data[i3] * 3;
        ll next5 = buf.data[i5] * 5;
        ll next7 = buf.data[i7] * 7;

        ll next = next3;
        if (next5 < next) next = next5;
        if (next7 < next) next = next7;

        add_element(&buf, next);

        if (next == next3) i3++;
        if (next == next5) i5++;
        if (next == next7) i7++;

        int min_unused = (i3 < i5) ? i3 : i5;
        min_unused = (min_unused < i7) ? min_unused : i7;

        if (min_unused != prev_min_used) 
        {
            compact_buffer(&buf, min_unused);
            i3 -= min_unused;
            i5 -= min_unused;
            i7 -= min_unused;
            last_compacted += min_unused;
        }

        prev_min_used = min_unused;
    }

    ll result = buf.data[n - last_compacted];
    free(buf.data);
    return result;
}

int main() {
    int n;
    while (1) {
        if (scanf("%d", &n) != 1) break;
        printf("%llu\n", give_n_sequence(n));
    }
    return 0;
}