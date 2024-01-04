#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct node_t *node, node_t;
struct node_t { int v; node next; };
typedef struct { node head, tail; } slist;

void push(slist *l, node e) {
    if (!l->head) l->head = e;
    if (l->tail)  l->tail->next = e;
    l->tail = e;
}

node removehead(slist *l) {
    node e = l->head;
    if (e) {
        l->head = e->next;
        e->next = 0;
    }
    return e;
}

void join(slist *a, slist *b) {
    push(a, b->head);
    a->tail = b->tail;
}

void merge(slist *a, slist *b) {
    slist r = {0};
    while (a->head && b->head)
        push(&r, removehead(a->head->v <= b->head->v ? a : b));

    join(&r, a->head ? a : b);
    *a = r;
    b->head = b->tail = 0;
}

void sort(int *ar, int len)
{
    node_t all[len];

    // array to list
    for (int i = 0; i < len; i++)
        all[i].v = ar[i], all[i].next = i < len - 1 ? all + i + 1 : 0;

    slist list = {all, all + len - 1}, rem, strand = {0},  res = {0};

    for (node e = 0; list.head; list = rem) {
        rem.head = rem.tail = 0;
        while ((e = removehead(&list)))
            push((!strand.head || e->v >= strand.tail->v) ? &strand : &rem, e);

        merge(&res, &strand);
    }

    // list to array
    for (int i = 0; res.head; i++, res.head = res.head->next)
        ar[i] = res.head->v;
}

void show(const char *title, int *x, int len)
{
    printf("%s ", title);
    for (int i = 0; i < len; i++)
        printf("%3d ", x[i]);
    putchar('\n');
}

double getCurrentTime()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return (double)currentTime.tv_sec * 1000.0 + (double)currentTime.tv_nsec / 1000000.0;
}

int main(void)
{
clock_t starttime, endtime; 

	double totaltime; 
    FILE *file = fopen("T10I4D100K.dat.txt", "r");
    if (file == NULL) {
        printf("Failed to open the input file.\n");
        return 1;
    }

int x[100000]; // Assuming a maximum of 100 integers
    int len = 0;

	starttime = clock(); 
    // Read integers from the file
    while (fscanf(file, "%d", &x[len]) != EOF) {
        len++;
        if (len >= 100000) {
            //printf("Too many integers in the file. Increase the array size if necessary.\n");
            break;
        }
    }

    // Close the file
    fclose(file);

    //show("Before sort:", x, len);
    sort(x, len);
endtime = clock();

	totaltime = ((double)(endtime - starttime)) / CLOCKS_PER_SEC;
printf("\n\nTotal time of execution = %f seconds\n", totaltime); 
    //show("After sort: ", x, len);




    return 0;
}

