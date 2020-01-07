#ifndef CKLBUIShader_h
#define CKLBUIShader_h

#include "CKLBUITask.h"

/*!
* \class CKLBUIShader
* \brief Shader Task Class
*
* CKLBUIShader allows to create shaders.
*/
class CKLBUIShader : public CKLBUITask
{
	friend class CKLBTaskFactory<CKLBUIShader>;
private:
	CKLBUIShader();
	virtual ~CKLBUIShader();

public:
	virtual u32 getClassID();

	bool initUI(CLuaState& lua);
	int commandUI(CLuaState& lua, int argc, int cmd);

	void execute(u32 deltaT);
	void dieUI();


private:
	const char*				m_name;
	u32						m_startRenderPriority;
	u32						m_endRenderPriority;
	u32						m_sheaderDefinition;
	void*					m_sheaderInstance;

	static PROP_V2			ms_propItems[];
};

#endif // CKLBUIShader_h
