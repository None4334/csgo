

class CNetworkStringTable {
public:
	int addString(bool isServer, const char* value, int length = -1, const void* userdata = nullptr) {
		return TUtil::CallVFunc<int>(this, 8, isServer, value, length, userdata);
	}
};

class CNetworkStringTableContainer {
public:
	CNetworkStringTable* findTable(const char* name) {
		return TUtil::CallVFunc<CNetworkStringTable*>(this, 3, name);
	}
};