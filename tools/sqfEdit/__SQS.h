#pragma once
//SJCLib
#include <SJCLib.h>

namespace BISLibrary
{
	static const SJCLib::CString g_SQSCommands[]=
	{
		"abs",
		"accTime",
		"acos",
		"action",
		"addAction",
		"addEventHandler",
		"addMagazine",
		"addMagazineCargo",
		"addMagazinePool",
		"addRating",
		"addScore",
		"addWeapon",
		"addWeaponCargo",
		"addWeaponPool",
		"alive",
		"allowDammage",
		"allowFleeing",
		"allowGetIn",
		"ammo",
		"and",
		"animate",
		"animationPhase",
		"asin",
		"assignAsCargo",
		"assignAsCommander",
		"assignAsDriver",
		"assignAsGunner",
		"atan",
		"atan2",
		"atg",
		"behaviour",
		"benchmark",
		"buildingPos",
		"buttonAction",
		"buttonSetAction",
		"cadetMode",
		"call",
		"camCommand",
		"camCommit",
		"camCommitted",
		"camCreate",
		"camDestroy",
		"cameraEffect",
		"camSetBank",
		"camSetDir",
		"camSetDive",
		"camSetFov",
		"camSetFovRange",
		"camSetPos",
		"camSetRelPos",
		"camSetTarget",
		"canFire",
		"canMove",
		"canStand",
		"captive",
		"cheatsEnabled",
		"civilian",
		"clearMagazineCargo",
		"clearMagazinePool",
		"clearWeaponCargo",
		"clearWeaponPool",
		"closeDialog",
		"combatMode",
		"commander",
		"commandFire",
		"commandFollow",
		"commandMove",
		"commandStop",
		"commandTarget",
		"commandWatch",
		"comment",
		"cos",
		"count",
		"countEnemy",
		"countFriendly",
		"countSide",
		"countType",
		"countUnknown",
		"createDialog",
		"createUnit",
		"createVehicle",
		"crew",
		"ctrlEnable",
		"ctrlEnabled",
		"ctrlSetText",
		"ctrlShow",
		"ctrlText",
		"ctrlVisible",
		"cutObj",
		"cutRsc",
		"cutText",
		"damage",
		"daytime",
		"debugLog",
		"deg",
		"deleteIdentity",
		"deleteStatus",
		"deleteVehicle",
		"dialog",
		"direction",
		"disableAI",
		"disableUserInput",
		"distance",
		"do",
		"doFire",
		"doFollow",
		"doMove",
		"doStop",
		"doTarget",
		"doWatch",
		"driver",
		"drop",
		"east",
		"else",
		"enableEndDialog",
		"enableRadio",
		"enemy",
		"engineOn",
		"estimatedTimeLeft",
		"exec",
		"exit",
		"exp",
		"fadeMusic",
		"fadeSound",
		"false",
		"fillWeaponsFromPool",
		"fire",
		"flag",
		"flagOwner",
		"fleeing",
		"flyInHeight",
		"forceEnd",
		"forceMap",
		"forEach",
		"format",
		"formation",
		"formLeader",
		"friendly",
		"fuel",
		"getDammage",
		"getDir",
		"getMarkerColor",
		"getMarkerPos",
		"getMarkerSize",
		"getMarkerType",
		"getPos",
		"getWPPos",
		"globalChat",
		"globalRadio",
		"goto",
		"group",
		"groupChat",
		"groupRadio",
		"grpNull",
		"gunner",
		"handsHit",
		"hasWeapon",
		"hint",
		"hintC",
		"hintCadet",
		"if",
		"in",
		"inflame",
		"inflamed",
		"isEngineOn",
		"isNull",
		"join",
		"knowsAbout",
		"land",
		"lbAdd",
		"lbClear",
		"lbColor",
		"lbCurSel",
		"lbData",
		"lbDelete",
		"lbPicture",
		"lbSetColor",
		"lbSetCurSel",
		"lbSetData",
		"lbSetPicture",
		"lbSetValue",
		"lbSize",
		"lbText",
		"lbValue",
		"leader",
		"leaveVehicle",
		"lightIsOn",
		"list",
		"ln",
		"loadFile",
		"loadIdentity",
		"loadStatus",
		"local",
		"localize",
		"lock",
		"locked",
		"lockWP",
		"log",
		"magazines",
		"mapAnimAdd",
		"mapAnimClear",
		"mapAnimCommit",
		"mapAnimDone",
		"markerColor",
		"markerPos",
		"markerSize",
		"markerType",
		"missionName",
		"missionStart",
		"mod",
		"move",
		"moveInCargo",
		"moveInCommander",
		"moveInDriver",
		"moveInGunner",
		"musicVolume",
		"name",
		"nearestBuilding",
		"nearestObject",
		"nil",
		"not",
		"object",
		"objNull",
		"objStatus",
		"onBriefingGear",
		"onBriefingGroup",
		"onBriefingNotes",
		"onBriefingPlan",
		"onMapSingleClick",
		"or",
		"orderGetIn",
		"pi",
		"pickWeaponPool",
		"player",
		"playersNumber",
		"playMove",
		"playMusic",
		"playSound",
		"position",
		"preprocessFile",
		"primaryWeapon",
		"private",
		"publicVariable",
		"putWeaponPool",
		"queryMagazinePool",
		"queryWeaponPool",
		"rad",
		"random",
		"rating",
		"removeAction",
		"removeAllEventHandlers",
		"removeAllWeapons",
		"removeEventHandler",
		"removeMagazine",
		"removeMagazines",
		"removeWeapon",
		"requiredVersion",
		"resistance",
		"resize",
		"reveal",
		"saveGame",
		"saveIdentity",
		"saveStatus",
		"saveVar",
		"say",
		"score",
		"scudState",
		"secondaryWeapon",
		"select",
		"selectWeapon",
		"set",
		"setAccTime",
		"setAmmoCargo",
		"setBehaviour",
		"setCaptive",
		"setCombatMode",
		"setCurrentTask",
		"setDamage",
		"setDammage",
		"setDir",
		"setFace",
		"setFaceAnimation",
		"setFlagOwner",
		"setFlagSide",
		"setFlagTexture",
		"setFog",
		"setFormation",
		"setFormDir",
		"setFuel",
		"setFuelCargo",
		"setGroupId",
		"setIdentity",
		"setMarkerColor",
		"setMarkerPos",
		"setMarkerSize",
		"setMarkerType",
		"setMimic",
		"setObjectTexture",
		"setOvercast",
		"setPos",
		"setRadioMsg",
		"setRain",
		"setRepairCargo",
		"setSkill",
		"setSpeedMode",
		"setTerrainGrid",
		"setUnitPos",
		"setVelocity",
		"setViewDistance",
		"setWPPos",
		"showCinemaBorder",
		"showCompass",
		"showGps",
		"showMap",
		"shownCompass",
		"shownGps",
		"shownMap",
		"shownPad",
		"shownRadio",
		"shownWarrant",
		"shownWatch",
		"showPad",
		"showRadio",
		"showWarrant",
		"showWatch",
		"side",
		"sideChat",
		"sideEnemy",
		"sideFriendly",
		"sideLogic",
		"sideRadio",
		"sin",
		"skill",
		"skipTime",
		"sliderPosition",
		"sliderRange",
		"sliderSetPosition",
		"sliderSetRange",
		"sliderSetSpeed",
		"sliderSpeed",
		"someAmmo",
		"soundVolume",
		"speed",
		"speedMode",
		"sqrt",
		"stop",
		"stopped",
		"switchCamera",
		"switchLight",
		"switchMove",
		"tan",
		"textLog",
		"tg",
		"then",
		"time",
		"titleCut",
		"titleObj",
		"titleRsc",
		"titleText",
		"true",
		"typeOf",
		"unassignVehicle",
		"unitReady",
		"units",
		"vehicle",
		"vehicleChat",
		"vehicleRadio",
		"velocity",
		"waypointPosition",
		"weapons",
		"west",
		"while"
	};
}