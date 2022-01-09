#include "cbase.h"

class CMyModelEntity : public CBaseAnimating {
	DECLARE_CLASS(CMyModelEntity, CBaseAnimating);

public:
	void Spawn(void);
	void Precache(void);

	void MoveThink(void);

	void InputToggle(inputdata_t& inputData);

	DECLARE_DATADESC();

private:
	bool m_bActive{ false };
	float m_flNextChangeTime;
};

LINK_ENTITY_TO_CLASS(my_model_entity, CMyModelEntity);

BEGIN_DATADESC(CMyModelEntity)
	DEFINE_FIELD(m_bActive, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flNextChangeTime, FIELD_FLOAT),

	DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

	DEFINE_THINKFUNC(MoveThink),
END_DATADESC()

#define ENTITY_MODEL "models/combine_scanner.mdl"

void CMyModelEntity::Precache(void) {
	PrecacheModel(ENTITY_MODEL);

	BaseClass::Precache();
}

void CMyModelEntity::Spawn(void) {
	Precache();

	SetModel(ENTITY_MODEL);
	SetSolid(SOLID_BBOX);
	UTIL_SetSize(this, -Vector(20, 20, 20), Vector(20, 20, 20));

	SetSequence(LookupSequence("idle"));
	SetPlaybackRate(1.0f);
	UseClientSideAnimation();
}

void CMyModelEntity::MoveThink(void) {
	if (m_flNextChangeTime < gpGlobals->curtime) {
		SetAbsVelocity(RandomVector(-64.0f, 64.0f));

		m_flNextChangeTime = gpGlobals->curtime + random->RandomFloat(1.0f, 3.0f);
	}

	QAngle angFacing;
	VectorAngles(GetAbsVelocity(), angFacing);
	SetAbsAngles(angFacing);

	SetNextThink(gpGlobals->curtime + 0.05f);
}

void CMyModelEntity::InputToggle(inputdata_t& inputData) {
	if (!m_bActive) {
		SetThink(&CMyModelEntity::MoveThink);

		SetMoveType(MOVETYPE_FLY);

		SetNextThink(gpGlobals->curtime + 0.05f);
		m_flNextChangeTime = gpGlobals->curtime;

		m_bActive = true;
	}
	else {
		SetThink(nullptr);

		SetAbsVelocity(vec3_origin);
		SetMoveType(MOVETYPE_NONE);

		m_bActive = false;
	}
}

CON_COMMAND(create_sdk_mymodelentity, "Creates an instance of My Model Entity in front of the player.") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (pPlayer == nullptr) {
		Warning("Couldn't determine who called command!\n");
		return;
	}

	CBaseEntity* pEnt = CreateEntityByName("my_model_entity");
	if (pEnt != nullptr) {
		Vector vecForward;
		AngleVectors(pPlayer->EyeAngles(), &vecForward);
		Vector vecOrigin = pPlayer->GetAbsOrigin() + vecForward * 256 + Vector(0, 0, 64);
		QAngle vecAngles(0, pPlayer->GetAbsAngles().y - 90, 0);

		pEnt->SetAbsOrigin(vecOrigin);
		pEnt->SetAbsAngles(vecAngles);
		DispatchSpawn(pEnt);
	}
}
