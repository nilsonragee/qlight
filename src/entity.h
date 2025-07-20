#ifndef QLIGHT_ENTITY_H
#define QLIGHT_ENTITY_H

#include "common.h"
#include "transform.h"
#include "model.h"
#include "math.h"

enum Entity_Type : u16 {
	EntityType_Player = 0,
	EntityType_Camera,
	EntityType_StaticObject,
	EntityType_DynamicObject,

	EntityType_COUNT,

	EntityType_Unknown = U16_MAX,
};

enum EEntity_Bits : u32 {
	EntityBit_NoDraw = ( 1 << 0 )
};
typedef u16 Entity_Bits;

typedef u16 Entity_ID;
constexpr Entity_ID INVALID_ENTITY_ID = U16_MAX;

struct Entity {
	Entity_Type type;
	Entity_ID parent;
	Entity_Bits bits;
	Transform transform;
};

struct Entity_Player : Entity {
	String_ASCII name;
	Model_ID model;
};

struct Entity_Static_Object : Entity {
	Model_ID model;
};

#endif /* QLIGHT_ENTITY_H */