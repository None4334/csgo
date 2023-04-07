
struct TEventLog {

	TEventLog()
	{


	}

	TEventLog(std::string String,int _iTickCount) {

		RtlZeroMemory(Characters, 500);

		Size = String.size();

		if (Size >= 500)
			return;

		RtlCopyMemory(Characters, String.data(), Size * sizeof(char));
		iTickCount = _iTickCount;
	}

	char Characters[500];
	size_t Size;
	int iTickCount = 0;
};


namespace TEventLogger {
	inline TDynamicStack<TEventLog,20> stkEventLogs;

	void PushString(std::string szText);
	void Routine();
}
