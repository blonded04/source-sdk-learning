#include "cbase.h"

class CMyLogicalEntity : public CLogicalEntity {
	DECLARE_CLASS(CMyLogicalEntity, CLogicalEntity);
public:
	void InputTick(inputdata_t& inputData);

	DECLARE_DATADESC();

private:
	int m_iThreshold;
	int m_iCounter{ 0 };

	COutputEvent m_onThreshold;
};

LINK_ENTITY_TO_CLASS(my_logical_entity, CMyLogicalEntity);

BEGIN_DATADESC(CMyLogicalEntity)
	DEFINE_FIELD(m_iCounter, FIELD_INTEGER),
	
	DEFINE_KEYFIELD(m_iThreshold, FIELD_INTEGER, "Threshold"),
	
	DEFINE_INPUTFUNC(FIELD_VOID, "Tick", InputTick),

	DEFINE_OUTPUT(m_onThreshold, "OnThreshold"),
END_DATADESC()

void CMyLogicalEntity::InputTick(inputdata_t& inputData) {
	m_iCounter++;
	if (m_iThreshold <= m_iCounter) {
		m_onThreshold.FireOutput(inputData.pActivator, this);
		m_iCounter = 0;
	}
}
