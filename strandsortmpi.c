#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <mpi.h>

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

    for (int i = 0; i < len; i++)
        all[i].v = ar[i], all[i].next = i < len - 1 ? all + i + 1 : 0;

    slist list = {all, all + len - 1}, rem, strand = {0},  res = {0};

    for (node e = 0; list.head; list = rem) {
        rem.head = rem.tail = 0;
        while ((e = removehead(&list)))
            push((!strand.head || e->v >= strand.tail->v) ? &strand : &rem, e);

        merge(&res, &strand);
    }

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


double getCurrentTime() {
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return (double)currentTime.tv_sec * 1000.0 + (double)currentTime.tv_nsec / 1000000.0;
}


int main(int argc, char *argv[]) {
	clock_t starttime, endtime; 

	double totaltime; 
		 
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Assuming the maximum number of integers is 100000
    int x[100000];
    int len = 0;

    // Assuming a file named "T10I4D100K.dat.txt" is present
    FILE *file = fopen("T10I4D100K.dat.txt", "r");
    if (file == NULL) {
        printf("Failed to open the input file.\n");
        MPI_Finalize();
        return 1;
    }

    while (fscanf(file, "%d", &x[len]) != EOF) {
        len++;
        if (len >= 100000) {
            //printf("Too many integers in the file. Increase the array size if necessary.\n");
            break;
        }
    }

    fclose(file);
    // Distribute data among different MPI processes
    int local_size = len / size;
    int *local_data = (int *)malloc(local_size * sizeof(int));

	starttime = clock();
    MPI_Scatter(x, local_size, MPI_INT, local_data, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform the sorting operation
    sort(local_data, local_size);

    // Gather sorted data to the root process
    int *sorted_data = NULL;
    if (rank == 0) {
        sorted_data = (int *)malloc(len * sizeof(int));
    }

    MPI_Gather(local_data, local_size, MPI_INT, sorted_data, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int *final_sorted_data = (int *)malloc(len * sizeof(int));
        int local_index[size];
        for (int i = 0; i < size; i++) {
            local_index[i] = i * local_size;
        }
        for (int i = 0; i < len; i++) {
            int min_index = -1;
            int min_value;
            for (int j = 0; j < size; j++) {
                if (local_index[j] < (j + 1) * local_size && (min_index == -1 || sorted_data[local_index[j]] < min_value)) {
                    min_index = j;
                    min_value = sorted_data[local_index[j]];
                }
            }
            final_sorted_data[i] = min_value;
            local_index[min_index]++;
        }
        endtime = clock();

        // Display or use the final sorted dataset
        //show("After sort: ", final_sorted_data, len);
        

    totaltime = ((double)(endtime - starttime)) / CLOCKS_PER_SEC;
    printf("\nTotal time of execution = %f", totaltime);
    }
    free(local_data);
    MPI_Finalize();
    



    return 0;
}

