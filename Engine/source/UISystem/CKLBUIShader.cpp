
#include "CKLBUIShader.h"
#include "CKLBUtility.h"
#include "CKLBLuaEnv.h"
#include "CKLBScriptEnv.h"

enum {
	SHADER_ANIM = 0,
	PSHADER_VALUE = 1,
	VSHADER_VALUE = 2
};

static IFactory::DEFCMD cmd[] = {
	{"SHADER_ANIM",		SHADER_ANIM		},
	{"PSHADER_VALUE",	PSHADER_VALUE	},
	{"VSHADER_VALUE",	VSHADER_VALUE	},
	{0, 0}
};

static CKLBTaskFactory<CKLBUIShader> factory("UI_Shader", CLS_KLBUISHADER, cmd);

CKLBLuaPropTask::PROP_V2 CKLBUIShader::ms_propItems[] = {
	UI_BASE_PROP
};


CKLBUIShader::CKLBUIShader() : CKLBUITask()
, m_name(NULL)
, m_startRenderPriority(NULL)
, m_endRenderPriority(NULL)
, m_sheaderDefinition(NULL)
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

bool
CKLBUIShader::initUI(CLuaState& lua)
{
	int argc = lua.numArgs();
	lua.print_stack();

	if (lua.isString(2) && lua.isNum(3)) {
		m_name = lua.getString(2);
		m_startRenderPriority = lua.getInt(3);
	}
	else if (lua.isNil(2) && lua.isNum(3)) {
		CKLBUIShader* task = (CKLBUIShader*)lua.getPointer(1);
		CKLBRenderingManager& mgr = CKLBRenderingManager::getInstance();

		task->m_endRenderPriority = lua.getInt(3);
		task->m_sheaderDefinition = mgr.getShaderDefinition(task->m_name);
		task->m_sheaderInstance	  = mgr.instanceShader(task->m_sheaderDefinition, task->m_startRenderPriority, task->m_endRenderPriority);
	}
	return true;
}


int
CKLBUIShader::commandUI(CLuaState& lua, int argc, int cmd)
{
	// TODO
	switch (cmd) {
	case SHADER_ANIM: 
	{
		break;
	}
	case PSHADER_VALUE: 
	{
		break;
	}
	case VSHADER_VALUE:
	{
		break;
	}
	}
	lua.retBoolean(true);
	return 1;
}

void
CKLBUIShader::execute(u32 /* deltaT */)
{
}

void
CKLBUIShader::dieUI()
{
	if (m_name != NULL) {
		CKLBRenderingManager& mgr = CKLBRenderingManager::getInstance();
		mgr.removeShader(m_sheaderInstance);
	}
}
