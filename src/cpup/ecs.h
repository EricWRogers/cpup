#pragma once
#include "vec.h"
#include "math.h"
#include <stdlib.h>

typedef struct {
    u32 entity;
    u32 index;
} EntityAndIndex;

typedef struct {
    u64*                mask;
    EntityAndIndex**     componentMap;
    void**              denseComponents;
    u64                 nextId;
    u64                 version;
} ECS;

typedef void (*EntityIterFunc)(ECS* _ecs, u32 _entityId, void* _userData);

typedef struct {
    u64 requiredMask;
    u64 lastVersion;
    u32* entities;
} ECSView;

ECS InitECS(i32 _entityCapacity) {
    ECS ecs;
    ecs.mask = vec_init(_entityCapacity, sizeof(u64));
    ecs.componentMap = vec_init(32, sizeof(void*));
    ecs.denseComponents = vec_init(32, sizeof(void*));
    ecs.nextId = 1;
    ecs.version = 1;
    return ecs;
}

u64 RegisterComponent(ECS* _ecs, size_t _size) {
    u64 id = _ecs->nextId;
    _ecs->nextId <<= 1;
    if (_ecs->nextId == 0)
        _ecs->nextId = 1;

    u32 index = vec_count(&_ecs->componentMap);
    vec_resize(&_ecs->componentMap, index + 1);
    vec_resize(&_ecs->denseComponents, index + 1);

    u32 entityCount = vec_count(&_ecs->mask);

    EntityAndIndex* sparse = vec_init(entityCount ? entityCount : 1, sizeof(EntityAndIndex));
    vec_resize(&sparse, entityCount);
    for (u32 i = 0; i < entityCount; i++) {
        sparse[i].entity = i;
        sparse[i].index = u32_max;
    }

    void* dense = vec_init(entityCount ? entityCount : 1, _size);

    _ecs->componentMap[index] = sparse;
    _ecs->denseComponents[index] = dense;
    return id;
}

u32 CreateEntity(ECS* _ecs) {
    u32 id = vec_count(&_ecs->mask);
    u64 value = 0;
    vec_add(&_ecs->mask, &value);

    u32 componentCount = vec_count(&_ecs->componentMap);
    for (u32 i = 0; i < componentCount; i++) {
        EntityAndIndex* sparse = _ecs->componentMap[i];
        if (sparse == NULL)
            continue;

        vec_resize(&sparse, id + 1);
        sparse[id].entity = id;
        sparse[id].index = u32_max;
        _ecs->componentMap[i] = sparse;
    }

    _ecs->version++;
    return id;
}

static i32 GetComponentIndex(u32 _componentId) {
    switch (_componentId) {
        case (1u << 0): return 0;
        case (1u << 1): return 1;
        case (1u << 2): return 2;
        case (1u << 3): return 3;
        case (1u << 4): return 4;
        case (1u << 5): return 5;
        case (1u << 6): return 6;
        case (1u << 7): return 7;
        case (1u << 8): return 8;
        case (1u << 9): return 9;
        case (1u << 10): return 10;
        case (1u << 11): return 11;
        case (1u << 12): return 12;
        case (1u << 13): return 13;
        case (1u << 14): return 14;
        case (1u << 15): return 15;
        case (1u << 16): return 16;
        case (1u << 17): return 17;
        case (1u << 18): return 18;
        case (1u << 19): return 19;
        case (1u << 20): return 20;
        case (1u << 21): return 21;
        case (1u << 22): return 22;
        case (1u << 23): return 23;
        case (1u << 24): return 24;
        case (1u << 25): return 25;
        case (1u << 26): return 26;
        case (1u << 27): return 27;
        case (1u << 28): return 28;
        case (1u << 29): return 29;
        case (1u << 30): return 30;
        case (1u << 31): return 31;
        default: return -1;
    }
}

void* AddComponent(ECS* _ecs, u32 _entityId, u32 _componentId) {
    if (_componentId == 0 || _entityId >= vec_count(&_ecs->mask))
        return NULL;

    i32 componentIndex = GetComponentIndex(_componentId);
    if (componentIndex < 0)
        return NULL;

    if ((u32)componentIndex >= vec_count(&_ecs->componentMap))
        return NULL;

    EntityAndIndex* componentMap = _ecs->componentMap[componentIndex];
    void* dense = _ecs->denseComponents[componentIndex];
    if (componentMap == NULL || dense == NULL)
        return NULL;

    if (_entityId >= vec_count(&componentMap)) {
        vec_resize(&componentMap, _entityId + 1);
        componentMap[_entityId].entity = _entityId;
        componentMap[_entityId].index = u32_max;
        _ecs->componentMap[componentIndex] = componentMap;
    }

    if ((_ecs->mask[_entityId] & (u64)_componentId) != 0) {
        u32 existingIndex = componentMap[_entityId].index;
        return (u8*)dense + (existingIndex * vec_element_size(&dense));
    }

    size_t elementSize = vec_element_size(&dense);
    u8* zeroValue = calloc(1, elementSize);
    if (zeroValue == NULL)
        return NULL;

    vec_add(&dense, zeroValue);
    free(zeroValue);

    _ecs->denseComponents[componentIndex] = dense;
    u32 denseIndex = vec_count(&dense) - 1;
    componentMap[_entityId] = (EntityAndIndex){ .entity = _entityId, .index = denseIndex };
    _ecs->mask[_entityId] |= (u64)_componentId;
    _ecs->version++;

    return (u8*)dense + (denseIndex * elementSize);
}

void* GetComponent(ECS* _ecs, u32 _entityId, u32 _componentId) {
    if (_componentId == 0 || _entityId >= vec_count(&_ecs->mask))
        return NULL;

    i32 componentIndex = GetComponentIndex(_componentId);
    if (componentIndex < 0)
        return NULL;

    if ((u32)componentIndex >= vec_count(&_ecs->componentMap))
        return NULL;

    if ((_ecs->mask[_entityId] & (u64)_componentId) == 0)
        return NULL;

    EntityAndIndex* componentMap = _ecs->componentMap[componentIndex];
    void* dense = _ecs->denseComponents[componentIndex];
    if (componentMap == NULL || dense == NULL)
        return NULL;

    u32 denseIndex = componentMap[_entityId].index;
    if (denseIndex >= vec_count(&dense))
        return NULL;

    return (u8*)dense + (denseIndex * vec_element_size(&dense));
}

void RemoveComponent(ECS* _ecs, u32 _entityId, u32 _componentId) {
    if (_ecs == NULL || _componentId == 0 || _entityId >= vec_count(&_ecs->mask))
        return;

    i32 componentIndex = GetComponentIndex(_componentId);
    if (componentIndex < 0 || (u32)componentIndex >= vec_count(&_ecs->componentMap))
        return;

    if ((_ecs->mask[_entityId] & (u64)_componentId) == 0)
        return;

    EntityAndIndex* componentMap = _ecs->componentMap[componentIndex];
    void* dense = _ecs->denseComponents[componentIndex];
    if (componentMap == NULL || dense == NULL)
        return;

    u32 removedIndex = componentMap[_entityId].index;
    u32 denseCount = vec_count(&dense);
    if (removedIndex >= denseCount)
        return;

    vec_remove_at(&dense, removedIndex);
    _ecs->denseComponents[componentIndex] = dense;

    componentMap[_entityId].index = u32_max;
    _ecs->mask[_entityId] &= ~((u64)_componentId);

    u32 entityCount = vec_count(&_ecs->mask);
    for (u32 entity = 0; entity < entityCount; entity++) {
        if ((_ecs->mask[entity] & (u64)_componentId) == 0)
            continue;

        if (componentMap[entity].index > removedIndex)
            componentMap[entity].index--;
    }

    _ecs->version++;
}

void DestroyEntity(ECS* _ecs, u32 _entityId) {
    if (_ecs == NULL || _entityId >= vec_count(&_ecs->mask))
        return;

    u64 mask = _ecs->mask[_entityId];
    if (mask == 0)
        return;

    for (u32 bit = 0; bit < 32; bit++) {
        u64 componentId = 1ull << bit;
        if ((mask & componentId) != 0) {
            RemoveComponent(_ecs, _entityId, (u32)componentId);
        }
    }
}

u64 GetSmallestComponentIdInMask(ECS* _ecs, u64 _requiredMask) {
    if (_ecs == NULL || _requiredMask == 0)
        return 0;

    u64 bestComponentId = 0;
    u32 bestCount = u32_max;

    for (u32 bit = 0; bit < 32; bit++) {
        u64 componentId = 1ull << bit;
        if ((_requiredMask & componentId) == 0)
            continue;

        i32 componentIndex = GetComponentIndex((u32)componentId);
        if (componentIndex < 0 || (u32)componentIndex >= vec_count(&_ecs->denseComponents))
            continue;

        void* dense = _ecs->denseComponents[componentIndex];
        if (dense == NULL)
            continue;

        u32 count = vec_count(&dense);
        if (count < bestCount) {
            bestCount = count;
            bestComponentId = componentId;
        }
    }

    return bestComponentId;
}

void ForEachEntityWithComponents(
    ECS* _ecs,
    u64 _requiredMask,
    EntityIterFunc _iterFunc,
    void* _userData
) {
    if (_ecs == NULL || _iterFunc == NULL)
        return;

    u32 entityCount = vec_count(&_ecs->mask);
    for (u32 entityId = 0; entityId < entityCount; entityId++) {
        u64 entityMask = _ecs->mask[entityId];
        if ((entityMask & _requiredMask) == _requiredMask) {
            _iterFunc(_ecs, entityId, _userData);
        }
    }
}

void QueryEntitiesWithComponents(ECS* _ecs, u64 _requiredMask, u32** _outEntities) {
    if (_ecs == NULL || _outEntities == NULL || *_outEntities == NULL)
        return;

    vec_clear(_outEntities);

    u32 entityCount = vec_count(&_ecs->mask);
    for (u32 entityId = 0; entityId < entityCount; entityId++) {
        u64 entityMask = _ecs->mask[entityId];
        if ((entityMask & _requiredMask) == _requiredMask) {
            vec_add(_outEntities, &entityId);
        }
    }
}

ECSView InitECSView(u64 _requiredMask, u32 _capacity) {
    ECSView view;
    view.requiredMask = _requiredMask;
    view.lastVersion = 0;
    view.entities = vec_init(_capacity ? _capacity : 1, sizeof(u32));
    return view;
}

void UpdateECSView(ECS* _ecs, ECSView* _view) {
    if (_ecs == NULL || _view == NULL)
        return;

    if (_view->lastVersion == _ecs->version)
        return;

    QueryEntitiesWithComponents(_ecs, _view->requiredMask, &_view->entities);
    _view->lastVersion = _ecs->version;
}

void FreeECSView(ECSView* _view) {
    if (_view == NULL || _view->entities == NULL)
        return;

    vec_free(&_view->entities);
    _view->lastVersion = 0;
}
