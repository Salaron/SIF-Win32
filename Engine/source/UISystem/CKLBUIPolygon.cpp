/* 
   Copyright 2013 KLab Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "CKLBUIPolygon.h"

enum {
	UI_POLYGON_NEWPATH,
	UI_POLYGON_PUSHPATH,
	UI_POLYGON_NEWHOLE,
	UI_POLYGON_ENDHOLE,
	UI_POLYGON_BUILD,
	UI_POLYGON_ADDPOINT,
	UI_POLYGON_SETTEXTURE
};

static IFactory::DEFCMD cmd[] = {
	{	"UI_POLYGON_NEWPATH",			UI_POLYGON_NEWPATH },
	{	"UI_POLYGON_PUSHPATH",			UI_POLYGON_PUSHPATH },
	{	"UI_POLYGON_NEWHOLE",			UI_POLYGON_NEWHOLE },
	{	"UI_POLYGON_ENDHOLE",			UI_POLYGON_ENDHOLE },
	{	"UI_POLYGON_BUILD",				UI_POLYGON_BUILD },
	{	"UI_POLYGON_ADDPOINT",			UI_POLYGON_ADDPOINT },
	{	"UI_POLYGON_SETTEXTURE",		UI_POLYGON_SETTEXTURE },
	{	0, 0 }
};

static CKLBTaskFactory<CKLBUIPolygon> factory("UI_Polygon", CLS_KLBUIPOLYGON, cmd);

// Allowed Property Keys
CKLBLuaPropTask::PROP_V2 CKLBUIPolygon::ms_propItems[] = {
	UI_BASE_PROP,
	{	"order",			R_UINTEGER,	NULL,									(getBoolT)&CKLBUIPolygon::getOrder,	0	},
	{	"maxpointcount",	UINTEGER,	(setBoolT)&CKLBUIPolygon::setMaxPoint,	(getBoolT)&CKLBUIPolygon::getMaxPoint,	0	}
};

// 引数のインデックス定義
enum {
	ARG_PARENT = 1,
	ARG_ORDER,

	ARG_MAXPOINTCNT,
	
	ARG_NUMS    = ARG_MAXPOINTCNT,
	ARG_REQUIRE = ARG_MAXPOINTCNT	// 最低限必要なパラメータ数
};

CKLBUIPolygon::CKLBUIPolygon()
: CKLBUITask	()
, m_pPolygon	(NULL) 
{
	setNotAlwaysActive();
	m_newScriptModel = true;
}

CKLBUIPolygon::~CKLBUIPolygon() 
{
}

u32
CKLBUIPolygon::getClassID()
{
	return CLS_KLBUIPOLYGON;
}

CKLBUIPolygon* 
CKLBUIPolygon::create(CKLBUITask* pParent, CKLBNode* pNode, u32 order, u32 maxPoint) 
{
	CKLBUIPolygon* pTask = KLBNEW(CKLBUIPolygon);
    if (!pTask) { return NULL; }
	if (!pTask->init(pParent, pNode, order, maxPoint)) {
		KLBDELETE(pTask);
		return NULL;
	}
	return pTask;
}

// TODO?
bool CKLBUIPolygon::init(CKLBUITask* pParent, CKLBNode* pNode, u32 order, u32 maxPoint) {
	return true;
}

bool 
CKLBUIPolygon::initCore(u32 order, u32 maxPoint)
{
	return true;
}

bool
CKLBUIPolygon::initUI(CLuaState& lua)
{
	return true;
}

int
CKLBUIPolygon::commandUI(CLuaState& lua, int argc, int cmd)
{
	int ret = 0;
	DEBUG_PRINT("UI_Polygon called: %d", cmd);
	lua.retString("123");
	lua.retBoolean(true);
	return 1;
}

void
CKLBUIPolygon::execute(u32 /*deltaT*/)
{
	// Should never be executed.
	klb_assertAlways("Task execution is not necessary");
}

void
CKLBUIPolygon::dieUI()
{
}
