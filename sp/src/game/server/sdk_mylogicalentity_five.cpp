// Actually, `My Stuff/sdk_mylogicalentity_five.cpp` is a subcase of `My Stuff/sdk_mylogicalentity.cpp`

#include "cbase.h"
#include "memdbgon.h"

class CMyLogicFive : public CLogicalEntity {
	DECLARE_CLASS(CMyLogicFive, CLogicalEntity);
public:
	void InputTick(inputdata_t& inputData);

	DECLARE_DATADESC();

private:
	int m_iTicks{ 0 };
};

LINK_ENTITY_TO_CLASS(my_logic_5, CMyLogicFive);

BEGIN_DATADESC(CMyLogicFive)
	DEFINE_FIELD(m_iTicks, FIELD_INTEGER),

	DEFINE_INPUTFUNC(FIELD_VOID, "Tick", InputTick),
END_DATADESC()

void CMyLogicFive::InputTick(inputdata_t& inputData) {
	m_iTicks++;
	if (5 <= m_iTicks) {
		ConColorMsg(Color(255, 0, 0), "\'sdk_mylogicalentity_five\' was triggered!\n");
		m_iTicks = 0;
	}
}
