#include "cbase.h"
#include "basehlcombatweapon.h"
#include "npcevent.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "game.h"
#include "in_buttons.h"
#include "ai_memory.h"
#include "soundent.h"
#include "rumble_shared.h"
#include "gamestats.h"

#include "memdbgon.h"  // Always has to be last include.

//-----------------------------------------------------------------------------
// CMyWeaponMP5
//-----------------------------------------------------------------------------
class CMyWeaponMP5 : public CHLSelectFireMachineGun {
	DECLARE_CLASS(CMyWeaponMP5, CHLSelectFireMachineGun);
public:
	DECLARE_DATADESC();

	CMyWeaponMP5();

	DECLARE_SERVERCLASS();

	void AddViewKick();  // Camera kick.

	int GetMinBurst() { return 1; }  // Amount of bullets in burst-fire mode? Recoil degree? IDK.
	int GetMaxBurst() { return 5; }

	bool Reload();

	float GetFireRate() { return 0.033f; }  // Fast-shooting boi.
	int CapabilitiesGet() { return bits_CAP_WEAPON_RANGE_ATTACK1; }  // AFAIK we are allowing this class to actually be a gun, by performing RANGE_ATTACK1.
	Activity GetPrimaryAttackActivity();  // Adjust viewmodel recoil depending on amount of shots fired in a row.

	virtual const Vector& GetBulletSpread() {  // Bullet spread depending on owner and its position;
		static Vector npcCone = VECTOR_CONE_4DEGREES;
		static Vector plrDuckCone = VECTOR_CONE_1DEGREES;  // Actually dumbass lasergun.
		static Vector plrStandCone = VECTOR_CONE_2DEGREES;
		static Vector plrMoveCone = VECTOR_CONE_3DEGREES;
		static Vector plrRunCone = VECTOR_CONE_4DEGREES;
		static Vector plrJumpCone = VECTOR_CONE_5DEGREES;

		if (GetOwner() == nullptr || GetOwner()->IsNPC()) {
			return npcCone;
		}

		CBasePlayer* pPlayer = ToBasePlayer(GetOwnerEntity());  // pPlayer->m_nButtons are buttons that pPlayer (player) is pressing.
		if (pPlayer->m_nButtons & IN_JUMP) {
			return plrJumpCone;
		}
		if (pPlayer->m_nButtons & (IN_SPEED | IN_RUN)) {
			return plrRunCone;
		}
		if (pPlayer->m_nButtons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT)) {
			return plrMoveCone;
		}
		if (pPlayer->m_nButtons & IN_DUCK) {
			return plrDuckCone;
		}
		return plrStandCone;
	}

	const WeaponProficiencyInfo_t* GetProficiencyValues() {  // Returns array of recoil-hardness levels depending on NPC skill (The higher skill the lower recoil).
		static WeaponProficiencyInfo_t proficiencyTable[] =
		{
			{ 7.0,		0.75	},
			{ 5.00,		0.75	},
			{ 10.0 / 3.0, 0.75	},
			{ 5.0 / 3.0,	0.75	},
			{ 1.00,		1.0		},
		};

		COMPILE_TIME_ASSERT(ARRAYSIZE(proficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

		return proficiencyTable;
	}

	void FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector &vecShootOrigin, Vector &vecShootDir);  // Some NPC fire logic I don't really get.
	void Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary);  // Something that forces NPC to fire (maybe on death/jumpscare). IDK.
	void Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator);  // Literally have no idea.

	DECLARE_ACTTABLE();  // Animations for our class table.
};

// Here, if we would write MP code, we would like our server to check, if client has such class -- if no, connection is not established.
IMPLEMENT_SERVERCLASS_ST(CMyWeaponMP5, DT_MyWeaponMP5)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_my_mp5, CMyWeaponMP5);
PRECACHE_WEAPON_REGISTER(weapon_my_mp5);

BEGIN_DATADESC(CMyWeaponMP5)
END_DATADESC()

acttable_t CMyWeaponMP5::m_acttable[] =  // We will use SMG animations because MP5 is sort of SMG.
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_SMG1,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,				true },
	{ ACT_IDLE,						ACT_IDLE_SMG1,					true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			true },

	{ ACT_WALK,						ACT_WALK_RIFLE,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true  },

	// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG1_RELAXED,			false },  // Never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG1_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG1,			false },  // Always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_RIFLE_RELAXED,			false },  // Never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_RIFLE_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_RIFLE,				false },  // Always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_RIFLE_RELAXED,			false },  // Never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_RIFLE_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_RIFLE,				false },  // Always aims

	// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG1_RELAXED,			false },  // Never aims
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_RIFLE_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG1,			false },  // Always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_RIFLE_RELAXED,			false },  // Never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_RIFLE_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_RIFLE,				false },  // Always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_RIFLE_RELAXED,			false },  // Never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_RIFLE_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_RIFLE,				false },  // Always aims
	// End readiness activities

	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_RIFLE,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SMG1,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG1_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_SMG1_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_SMG1_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,		true },
};

IMPLEMENT_ACTTABLE(CMyWeaponMP5);

CMyWeaponMP5::CMyWeaponMP5() {
	m_fMinRange1 = 0.0f;
	m_fMaxRange1 = 3000.0f;  // Max distance bullet can fly to.
}

// BEGIN FUNCTIONS THAT I DONT UNDERSTAND:
void CMyWeaponMP5::FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir)
{
	// FIXME: Use the returned number of bullets to account for >10hz firerate
	WeaponSoundRealtime(SINGLE_NPC);

	CSoundEnt::InsertSound(SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy());
	pOperator->FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2, entindex(), 0);

	pOperator->DoMuzzleFlash();
	m_iClip1--;
}

void CMyWeaponMP5::Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary)
{
	// Ensure we have enough rounds in the magazine
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle angShootDir;
	GetAttachment(LookupAttachment("muzzle"), vecShootOrigin, angShootDir);
	AngleVectors(angShootDir, &vecShootDir);
	FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
}

void CMyWeaponMP5::Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator)
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_SMG1:
	{
		Vector vecShootOrigin, vecShootDir;
		QAngle angDiscard;

		// Support old style attachment point firing
		if ((pEvent->options == NULL) || (pEvent->options[0] == '\0') || (!pOperator->GetAttachment(pEvent->options, vecShootOrigin, angDiscard)))
			vecShootOrigin = pOperator->Weapon_ShootPosition();

		CAI_BaseNPC* npc = pOperator->MyNPCPointer();
		ASSERT(npc != NULL);
		vecShootDir = npc->GetActualShootTrajectory(vecShootOrigin);

		FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
	}
	break;

	default:
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
		break;
	}
}
// END FUNCTIONS THAT I DON'T UNDERSTAND.

Activity CMyWeaponMP5::GetPrimaryAttackActivity() {
	if (m_nShotsFired <= 5) {
		return ACT_VM_PRIMARYATTACK;
	}
	if (m_nShotsFired <= 10) {
		return ACT_VM_RECOIL1;
	}
	return ACT_VM_RECOIL2;
}

bool CMyWeaponMP5::Reload() {
	bool bRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (bRet) {
		WeaponSound(RELOAD);
	}
	return bRet;
}

void CMyWeaponMP5::AddViewKick() {
#define EASY_DAMPEN			0.5f  // Slightly shaking.
#define MAX_VERTICAL_KICK	3.0f  // Max angle in degrees to kick.
#define SLIDE_LIMIT			0.5f  // Seconds we spend to perform kick.

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (pPlayer != nullptr) {
		DoMachineGunKick(pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT);
	}
}
