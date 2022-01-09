#include "cbase.h"
#include "triggers.h"

class CMyBrushEntity : public CBaseTrigger {
	DECLARE_CLASS(CMyBrushEntity, CBaseTrigger);
public:
	void Spawn();

	void BrushTouch(CBaseEntity *pOther);

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(my_brush_entity, CMyBrushEntity);

BEGIN_DATADESC(CMyBrushEntity)
	DEFINE_ENTITYFUNC(BrushTouch),
END_DATADESC()

void CMyBrushEntity::Spawn() {
	BaseClass::Spawn();

	SetTouch(&CMyBrushEntity::BrushTouch);

	SetSolid(SOLID_VPHYSICS);
	SetModel(STRING(GetModelName()));

	SetMoveType(MOVETYPE_PUSH);
	
	VPhysicsInitShadow(false, false);
}

void CMyBrushEntity::BrushTouch(CBaseEntity *pOther) {
	const trace_t& tr = GetTouchTrace();
	DevMsg("%s (%s) touch plane's normal: [%f %f]\n", GetClassname(), GetDebugName(), tr.plane.normal.x, tr.plane.normal.y);
	
	Vector vecPushDir = tr.plane.normal;
	Vector otherDir;
	AngleVectors(pOther->GetLocalAngles(), &otherDir);
	if (DotProduct(otherDir, vecPushDir) < 0) {
		vecPushDir.Negate();
	}
	vecPushDir.z = 0.0f;

	LinearMove(GetAbsOrigin() + vecPushDir * 64.0f, 32.0f);
}
