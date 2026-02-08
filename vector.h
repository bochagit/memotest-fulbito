

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <stddef.h>

typedef struct {
    void *data;
    size_t elemSize;
    size_t size;
    size_t capacity;
} tVector;


tVector* vector_create(size_t elemSize);

void vector_destroy(tVector *v);

int vector_push_back(tVector *v, const void *elem);

void* vector_get(tVector *v, size_t index);

int vector_set(tVector *v, size_t index, const void *elem);

size_t vector_size(tVector *v);

size_t vector_capacity(tVector *v);

int vector_remove_at(tVector *v, size_t index);

void vector_clear(tVector *v);

int vector_reserve(tVector *v, size_t newCapacity);

#endif // VECTOR_H_INCLUDED
