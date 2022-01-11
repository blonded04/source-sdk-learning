#include "cbase.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "gamestats.h"
#include "npcevent.h"

#include "tier0/memdbgon.h"

class CMyWeapon : public CBaseCombatWeapon {
public:
	DECLARE_CLASS(CMyWeapon, CBaseCombatWeapon);

	CMyWeapon();
	
	DECLARE_SERVERCLASS();

	void Precache();

	void ItemPreFrame();
	void ItemBusyFrame();
	void ItemPostFrame();

	void PrimaryAttack();
	void AddViewKick();
	void DryFire();

	bool Reload(void) override;

	virtual const Vector& GetBulletSpread();

	DECLARE_DATADESC();
	DECLARE_ACTTABLE();

private:
	float m_flRateOfFire;
};

IMPLEMENT_SERVERCLASS_ST(CMyWeapon, DT_weapon_tutorial)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_tutorial, CMyWeapon);
PRECACHE_WEAPON_REGISTER(weapon_tutorial);

BEGIN_DATADESC(CMyWeapon)
	DEFINE_FIELD(m_flRateOfFire, FIELD_FLOAT),
END_DATADESC()

acttable_t CMyWeapon::m_acttable[] = {
	{ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SMG1, true},
	{ACT_RELOAD, ACT_RELOAD_SMG1, true},
	{ACT_IDLE, ACT_IDLE_SMG1, true },
	{ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_SMG1, true },
	{ACT_WALK, ACT_WALK_RIFLE, true },
	{ACT_WALK_AIM, ACT_WALK_AIM_RIFLE, true },
};

CMyWeapon::CMyWeapon() {
	m_flRateOfFire = 0.1f;

	m_fMinRange1 = 24.0f;
	m_fMaxRange1 = 3000.0f;
	m_bFiresUnderwater = true;
}

#define MYWEAPON_MODEL_V "models/weapons/v_smg1.mdl"
#define MYWEAPON_MODEL_W "models/weapons/w_smg1.mdl"

void CMyWeapon::Precache() {
	PrecacheModel(MYWEAPON_MODEL_V, true);
	PrecacheModel(MYWEAPON_MODEL_W, true);
	BaseClass::Precache();
}

void CMyWeapon::ItemPreFrame() {
	BaseClass::ItemPreFrame();
}

void CMyWeapon::ItemBusyFrame() {
	BaseClass::ItemBusyFrame();
}

void CMyWeapon::ItemPostFrame() {
	BaseClass::ItemBusyFrame();
}

void CMyWeapon::PrimaryAttack() {
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	pPlayer->FireBullets(3, pPlayer->Weapon_ShootPosition(), pPlayer->GetAutoaimVector(0.0f), GetBulletSpread(), MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1, entindex(), 0, 30, GetOwner(), true, true);

	WeaponSound(SINGLE);
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	m_flNextPrimaryAttack = gpGlobals->curtime + m_flRateOfFire;
	AddViewKick();
}

void CMyWeapon::AddViewKick() {
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	pPlayer->ViewPunch(QAngle(-0.2f, 0.0f, 0.0f));
}

void CMyWeapon::DryFire() {
	WeaponSound(EMPTY);
	SendWeaponAnim(ACT_VM_DRYFIRE);

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

bool CMyWeapon::Reload(void) {
	bool bRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (bRet) {
		WeaponSound(RELOAD);
	}
	return bRet;
}

const Vector& CMyWeapon::GetBulletSpread() {
	static Vector plrCone = VECTOR_CONE_1DEGREES;
	static Vector npcCone = VECTOR_CONE_1DEGREES;
	if (GetOwner() != nullptr && GetOwner()->IsNPC()) {
		return npcCone;
	} else {
		return plrCone;
	}
}
