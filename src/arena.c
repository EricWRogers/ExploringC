#include "arena.h"
#include "vec.h"

#include <stdio.h>
#include <stdlib.h>

Arena arena_init(size_t _size) {
    Arena arena;
    arena.size = _size;
    arena.data = malloc(_size);

    vec_init(&arena._freeList, 10, sizeof(BlockInfo));
    BlockInfo block = {};
    block.size = _size;
    block.data = arena.data;
    vec_add(&arena._freeList, &block);

    return arena;
}

Arena arena_free(Arena* _arena) {
    free(_arena->data);
    vec_free(&(_arena->_freeList));
}

void* arena_alloc(Arena* _arena, const size_t _size) {
    for(unsigned int i = 0; i < vec_count(&_arena->_freeList); i++) {
        if (_arena->_freeList[i].used == false) {
            if (_arena->_freeList[i].size >= _size) {
                // change to insert to improve deallocate
                BlockInfo block = {};
                block.size = _arena->_freeList[i].size - _size;
                block.data = (char*)(_arena->_freeList[i].data) + _size;
                block.used = false;
                vec_add(&(_arena->_freeList), &block);

                _arena->_freeList[i].size = _size;
                _arena->_freeList[i].used = true;
                return _arena->_freeList[i].data;
            }
        }
    }
}

void* arena_realloc(Arena* _arena, void* _target, size_t _size) {
    unsigned int count = vec_count(&(_arena->_freeList));
    int index = -1;

    // TODO : replace with BST bacause it is always sorted
    for(unsigned int i = 0; i < count; i++) {
        if (_arena->_freeList[i].data == _target) {
            index = i;
            continue;
        }
    }

    if (index == -1) {
        printf("arena_realloc error: could not find target\n");
        exit(2);
    }

    if (index + 1 < count) {
        if (_arena->_freeList[index].size + _arena->_freeList[(index+1)].size >= _size && _arena->_freeList[(index+1)].used == false) {
            if (_arena->_freeList[index].size + _arena->_freeList[(index+1)].size == _size) {
                vec_remove_at(&(_arena->_freeList), (index+1));
                _arena->_freeList[index].size = _size;
            } else {
                _arena->_freeList[(index+1)].size -= _size - _arena->_freeList[index].size;
                _arena->_freeList[index].size = _size;
            }
            return _target;
        }
    }

    for(unsigned int i = 0; i < count; i++) {
        if (_arena->_freeList[i].size >= _size && _arena->_freeList[i].used == false) {
            memcpy(_arena->_freeList[i].data, _arena->_freeList[index].data, _arena->_freeList[index].size);
            _arena->_freeList[i].used == true;
            _arena_dealloc(_arena, index);
            return _arena->_freeList[i].data;
        }
    }

    return NULL;
}

void arena_dealloc(Arena* _arena, void* _target) {
    unsigned int count = vec_count(&(_arena->_freeList));

    // TODO : replace with BST bacause it is always sorted
    for(unsigned int i = 0; i < count; i++) {
        if (_arena->_freeList[i].data == _target) {
            _arena_dealloc(_arena, i);
            continue;
        }
    }
}

void _arena_dealloc(Arena* _arena, unsigned int _index) {
    unsigned int count = vec_count(&(_arena->_freeList));

    _arena->_freeList[_index].used = false;

    if (_index < count - 1) // check to the right
    {
        if (_arena->_freeList[(_index + 1)].used == false) {
            _arena->_freeList[_index].size += _arena->_freeList[(_index + 1)].size;
            vec_remove_at(&(_arena->_freeList), (_index+1));
            printf("combined to the right\n");
        }
    }

    if (_index > 0) // check to the left
    {
        if (_arena->_freeList[(_index - 1)].used == false) {
            _arena->_freeList[(_index - 1)].size += _arena->_freeList[_index].size;
            vec_remove_at(&(_arena->_freeList), (_index));
            printf("combined to the left\n");
        }
    }
}