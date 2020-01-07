#include "CKLBUIShader.h"
#include "CKLBLuaEnv.h"
#include "CKLBScriptEnv.h"
#include "CKLBUISystem.h"

enum {
	SHADER_ANIM,
	PSHADER_VALUE,
	VSHADER_VALUE,
};

static IFactory::DEFCMD cmd[] = {
	{ "SHADER_ANIM",		SHADER_ANIM   },
	{ "PSHADER_VALUE",		PSHADER_VALUE },
	{ "VSHADER_VALUE",		VSHADER_VALUE },
	{0, 0}
};

static CKLBTaskFactory<CKLBUIShader> factory("UI_Shader", CLS_KLBUISHADER, cmd);

CKLBLuaPropTask::PROP_V2 CKLBUIShader::ms_propItems[] = {
	UI_BASE_PROP
};

CKLBUIShader::CKLBUIShader()
	: CKLBUITask(CKLBTask::P_UIAFTER)
	, m_clipHandle(NULL)
{
	m_newScriptModel = true;
}

CKLBUIShader::~CKLBUIShader()
{
}

u32
CKLBUIShader::getClassID()
{
	return CLS_KLBUISHADER;
}

CKLBUIShader*
CKLBUIShader::create(const char* name, u32 startPrio, u32 endPrio)
{
	CKLBUIShader* pTask = KLBNEW(CKLBUIShader);
	if (!pTask) { return NULL; }
	return pTask;
}

bool
CKLBUIShader::initUI(CLuaState& lua)
{
	int argc = lua.numArgs();
	lua.print_stack();
	// calls twice
	// arg 3 is number
	// first call: start priority
	// second call: end priority

	return true;
}

void
CKLBUIShader::execute(u32 /*deltaT*/)
{
}

void
CKLBUIShader::dieUI()
{
}

int
CKLBUIShader::commandUI(CLuaState& lua, int argc, int cmd)
{
	lua.print_stack();
	return 1;
}
