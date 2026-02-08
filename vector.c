/**
 * @file vector.c
 * @brief Implementación del TDA Vector genérico.
 */

#include "vector.h"
#include <stdlib.h>
#include <string.h>

#define VECTOR_INITIAL_CAP 8

tVector* vector_create(size_t elemSize)
{
    if (elemSize == 0) return NULL;
    tVector *v = (tVector*)malloc(sizeof(tVector));
    if (!v) return NULL;
    v->elemSize = elemSize;
    v->size = 0;
    v->capacity = VECTOR_INITIAL_CAP;
    v->data = malloc(v->capacity * v->elemSize);
    if (!v->data) {
        free(v);
        return NULL;
    }
    return v;
}

void vector_destroy(tVector *v)
{
    if (!v) return;
    free(v->data);
    free(v);
}

int vector_reserve(tVector *v, size_t newCapacity)
{
    if (!v) return -1;
    if (newCapacity <= v->capacity) return 0;
    void *p = realloc(v->data, newCapacity * v->elemSize);
    if (!p) return -1;
    v->data = p;
    v->capacity = newCapacity;
    return 0;
}

static int _ensure_capacity(tVector *v)
{
    if (v->size < v->capacity) return 0;
    size_t nueva = v->capacity ? v->capacity * 2 : VECTOR_INITIAL_CAP;
    return vector_reserve(v, nueva);
}

int vector_push_back(tVector *v, const void *elem)
{
    if (!v || !elem) return -1;
    if (_ensure_capacity(v) != 0) return -1;
    void *dst = (char*)v->data + (v->size * v->elemSize);
    memcpy(dst, elem, v->elemSize);
    v->size++;
    return 0;
}

void* vector_get(tVector *v, size_t index)
{
    if (!v) return NULL;
    if (index >= v->size) return NULL;
    return (char*)v->data + (index * v->elemSize);
}

int vector_set(tVector *v, size_t index, const void *elem)
{
    if (!v || !elem) return -1;
    if (index >= v->size) return -1;
    void *dst = (char*)v->data + (index * v->elemSize);
    memcpy(dst, elem, v->elemSize);
    return 0;
}

size_t vector_size(tVector *v)
{
    if (!v) return 0;
    return v->size;
}

size_t vector_capacity(tVector *v)
{
    if (!v) return 0;
    return v->capacity;
}

int vector_remove_at(tVector *v, size_t index)
{
    if (!v) return -1;
    if (index >= v->size) return -1;
    if (index + 1 < v->size) {
        void *dst = (char*)v->data + (index * v->elemSize);
        void *src = (char*)v->data + ((index + 1) * v->elemSize);
        size_t bytes = (v->size - index - 1) * v->elemSize;
        memmove(dst, src, bytes);
    }
    v->size--;
    return 0;
}

void vector_clear(tVector *v)
{
    if (!v) return;
    v->size = 0;
}
