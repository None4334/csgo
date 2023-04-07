#pragma once

class IVPanel
{
public:
	void SetKeyBoardInputEnabled(unsigned int nPanelIndex, bool bState)
	{
		return TUtil::CallVFunc<void>(this, 31, nPanelIndex, bState);
	}

	void SetMouseInputEnabled(unsigned int nPanelIndex, bool bState)
	{
		return TUtil::CallVFunc<void>(this, 32, nPanelIndex, bState);
	}

	const char* GetName(int nPanelIndex)
	{
		return TUtil::CallVFunc<const char*>(this, 36, nPanelIndex);
	}

	const char* GetNameClass(int nPanelIndex)
	{
		return TUtil::CallVFunc<const char*>(this, 37, nPanelIndex);
	}
};
