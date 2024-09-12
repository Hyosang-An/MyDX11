#pragma once

enum class CB_TYPE
{
	TRANSFORM,
	MATERIAL,
	SPRITE,
	GLOBAL,

	END
};


enum class ASSET_TYPE
{
	MESH,
	MESH_DATA,
	MATERIAL,
	PREFAB,
	TEXTURE,
	SOUND,
	GRAPHIC_SHADER,
	COMPUTE_SHADER,
	SPRITE,
	FLIPBOOK,

	END
};

extern const char* ASSET_TYPE_STRING[(UINT)ASSET_TYPE::END];
const char* ToString(ASSET_TYPE _Type);


enum class COMPONENT_TYPE
{
	TRANSFORM,
	COLLIDER2D,
	COLLIDER3D,
	LIGHT2D,
	LIGHT3D,
	FLIPBOOK_COMPONENT,
	ANIMATOR3D,
	STATE_MACHINE,
	RIGIDBODY,
	CAMERA,

	MESHRENDER,
	TILEMAP,
	PARTICLE_SYSTEM,
	DECAl,
	SKYBOX,
	LANDSCAPE,

	END,

	// Script 컴포넌트는 따로 관리
	SCRIPT
};

extern const char* COMPONENT_TYPE_STRING[(UINT)COMPONENT_TYPE::END];
const char* ToString(COMPONENT_TYPE _Type);

// Layer 이름을 변경하면 LAYER_STRING에서도 변경해야함
enum class LAYER
{
	DEFAULT,
	BACKGROUND,
	ROOM,
	TILEMAP,
	PLAYER,
	MONSTER,
	PLAYER_PROJECTILE,
	MONSTER_PROJECTILE,

	WALL_OR_GROUND,
	SPIKE,
	DREAMBLOCK,
	
	EXTRA_OBJ,

	UI,

	DEBUG,


	END
};

extern const char* LAYER_STRING[(UINT)LAYER::END];
const char* ToString(LAYER _Layer);



enum DIR
{
	RIGHT,	// 로컬 X방향
	UP,		// 로컬 Y방향
	FRONT,	// 로컬 Z방향
};

enum class RS_TYPE
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,
	END,
};

enum class DS_TYPE
{
	LESS,
	LESS_EQUAL,

	GREATER,

	NO_TEST,
	NO_WRITE,
	NO_TEST_NO_WRITE,

	END,
};

enum class BS_TYPE
{
	DEFAULT,
	ALPHABLEND_COVERAGE, //이 블렌드 상태는 알파 블렌딩을 통해 투명도 있는 오브젝트를 처리합니다. 특히 **알파 투 커버리지(Alpha-to-Coverage)**가 활성화되어 있어 경계가 뚜렷한 텍스처의 투명 부분을 처리할 때 유리합니다. 알파 값이 0.5 이하인 부분은 아예 그려지지 않기 때문에 투명한 부분과 불투명한 부분의 경계가 명확하게 나타납니다.
	ALPHABLEND,  	// 이 블렌드 상태는 알파 블렌딩만 활성화된 상태로, 알파 값을 기반으로 두 픽셀의 색상을 섞는 일반적인 투명 객체 처리 방식입니다. 이는 유리, 물, 또는 반투명한 게임 오브젝트를 렌더링할 때 주로 사용됩니다. 이 방식에서는 오브젝트의 투명도에 따라 배경이 자연스럽게 드러납니다.
	ONE_ONE, 	// One-One 블렌딩은 소스와 기존 렌더 타겟의 색상을 더하는(Additive) 방식으로 동작합니다. 이 방식은 주로 빛, 파티클, 불꽃, 레이저 등 밝은 색이 쌓이는 효과가 필요한 경우에 사용됩니다. 각 픽셀의 색상이 더해지면서 화면이 더 밝아집니다. 예를 들어, 여러 개의 빛 효과가 겹치는 장면에서는 이 블렌드 상태를 사용하면 색상들이 더해지면서 자연스럽게 밝아지는 효과를 얻을 수 있습니다.
	END,
};

enum SCALAR_PARAM
{
	INT_0, INT_1, INT_2, INT_3,
	FLOAT_0, FLOAT_1, FLOAT_2, FLOAT_3,
	VEC2_0, VEC2_1, VEC2_2, VEC2_3,
	VEC4_0, VEC4_1, VEC4_2, VEC4_3,
	MAT_0, MAT_1, MAT_2, MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEXCUBE_0,
	TEXCUBE_1,

	TEXARR_0,
	TEXARR_1,

	END,
};

enum SHADER_DOMAIN
{
	DOMAIN_OPAQUE,		// 불투명 (불투명 객체는 알파 블렌딩(투명도 처리)이 필요하지 않은 객체들입니다. 대부분의 3D 모델이 불투명 객체로 분류됩니다. 알파값은 무시)
	DOMAIN_MASKED,		// 투명, 불투명 (마스크 객체는 부분적으로 투명한 텍스처를 사용하여 일부 픽셀은 완전히 렌더링되고, 다른 일부는 완전히 투명해지는 객체들입니다. 예를 들어, 나뭇잎이나 울타리와 같은 텍스처에 구멍이 있는 오브젝트가 여기에 해당됩니다.)
	DOMAIN_TRANSPARENT, // 반투명
	DOMAIN_PARTICLE,	// 파티클
	DOMAIN_POSTPROCESS,	// 후처리
	DOMAIN_UI,			// UI

	DOMAIN_DEBUG,		// 디버그

	DOMAIN_NONE,		// 미설정
};

enum class DEBUG_SHAPE
{
	RECT,
	CIRCLE,
	LINE,
	CUBE,
	SPHERE,
};

enum TASK_TYPE
{
	// Param_0 : Layer, Param_1 : Object Adress
	SPAWN_OBJECT,

	// Param_0 : Delete Object Adress
	DELETE_OBJECT,

	// Param_0 : Next Level State
	CHANGE_LEVELSTATE,

	// Param_0 : Level Adress, Param_1 : Level State
	CHANGE_LEVEL,

	// Param_0 : AssetAdress
	DEL_ASSET,

	LEVEL_CHANGED,
	ASSET_CHANGED,

	ADD_CHILD,
	COMPONENT_ACTIVATE,
	COMPONENT_DEACTIVATE,
};

enum LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	SPOT,
};

enum class PARTICLE_MODULE
{
	SPAWN,
	SPAWN_BURST,
	ADD_VELOCITY,
	SCALE,
	DRAG,
	NOISE_FORCE,
	RENDER,

	END,
};