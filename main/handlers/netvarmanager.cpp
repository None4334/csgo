#include "../../includes.h"

bool TNetvarManager::Setup()
{
	mapProps.clear();

	for (auto pClass = TInterfaces::BaseClient->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
	{
		if (pClass->pRecvTable == nullptr)
			continue;

		StoreProps(pClass->szNetworkName, pClass->pRecvTable, 0U, 0);
	}

	return !mapProps.empty();
}

void TNetvarManager::StoreProps(const char* szClassName, RecvTable_t* pRecvTable, const std::uintptr_t uOffset, const int iDepth)
{

	const FNV1A_t uClassHash = FNV1A::Hash(szClassName);
	const FNV1A_t uDelimiterHash = FNV1A::Hash("->", uClassHash);
	const FNV1A_t uBaseClassHash = FNV1A::Hash("baseclass", uDelimiterHash);

	for (int i = 0; i < pRecvTable->nProps; ++i)
	{
		const auto pCurrentProp = &pRecvTable->pProps[i];

		if (pCurrentProp == nullptr || isdigit(pCurrentProp->szVarName[0]))
			continue;

		const FNV1A_t uTotalHash = FNV1A::Hash(pCurrentProp->szVarName, uDelimiterHash);

		if (uTotalHash == uBaseClassHash)
			continue;

		if (const auto pChildTable = pCurrentProp->pDataTable; pChildTable != nullptr &&
			pChildTable->nProps > 0 &&
			pChildTable->szNetTableName[0] == 'D' &&
			pCurrentProp->iRecvType == DPT_DATATABLE)
			StoreProps(szClassName, pChildTable, static_cast<std::uintptr_t>(pCurrentProp->iOffset) + uOffset, iDepth + 1);

		const std::uintptr_t uTotalOffset = static_cast<std::uintptr_t>(pCurrentProp->iOffset) + uOffset;

		if (!mapProps[uTotalHash].uOffset)
			mapProps[uTotalHash] = { pCurrentProp, uTotalOffset };
		
	}
}

std::string TNetvarManager::GetPropertyType(const RecvProp_t* pRecvProp) 
{
	static CStandartRecvProxies* pStandartRecvProxies = TInterfaces::BaseClient->GetStandardRecvProxies();

	if (pRecvProp == nullptr)
		return "";

	RecvVarProxyFn pProxyFn = pRecvProp->oProxyFn;

	switch (pRecvProp->iRecvType)
	{
	case DPT_INT:

		if (pProxyFn == pStandartRecvProxies->pInt32ToInt8)
			return TSTRING("byte");
		else if (pProxyFn == pStandartRecvProxies->pInt32ToInt16)
			return TSTRING("short");

		return TSTRING("int");
	case DPT_FLOAT:
		return TSTRING("float");
	case DPT_VECTOR:
		return TSTRING("Vector");
	case DPT_VECTOR2D:
		return TSTRING("Vector2D");
	case DPT_STRING:
		return std::vformat(TSTRING("char[{:d}]"), std::make_format_args(pRecvProp->nStringBufferSize));
	case DPT_ARRAY:
		return std::vformat(TSTRING("array[{:d}]"), std::make_format_args(pRecvProp->iElements));
	case DPT_DATATABLE:
		return TSTRING("void*");
	case DPT_INT64:
		return TSTRING("std::int64_t");
	default:
		break;
	}

	return "";
}

std::uintptr_t TNetvarManager::FindInDataMap(DataMap_t* pMap, const FNV1A_t uFieldHash)
{
	while (pMap != nullptr)
	{
		for (int i = 0; i < pMap->nDataFields; i++)
		{
			if (pMap->pDataDesc[i].szFieldName == nullptr)
				continue;

			if (FNV1A::Hash(pMap->pDataDesc[i].szFieldName) == uFieldHash)
				return pMap->pDataDesc[i].iFieldOffset[TD_OFFSET_NORMAL];

			if (pMap->pDataDesc[i].iFieldType == FIELD_EMBEDDED)
			{
				if (pMap->pDataDesc[i].pTypeDescription != nullptr)
				{
					if (const auto uOffset = FindInDataMap(pMap->pDataDesc[i].pTypeDescription, uFieldHash); uOffset != 0U)
						return uOffset;
				}
			}
		}

		pMap = pMap->pBaseMap;
	}

	return 0U;
}
