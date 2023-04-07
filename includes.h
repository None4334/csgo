#include <Windows.h>
#include <cstddef>
#include <minwindef.h>
#include <array>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <format>
#include <optional>
#include <functional>
#include <intrin.h>
#include <DirectXMath.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <ranges>
#include <numbers>
#include <shared_mutex>
#include <deque>
#include <set>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "dependencies/mh/MinHook.h"
#include "dependencies/hooklibs/detour.h"
#include "dependencies/classes/pe32.h"
#include "dependencies/classes/vector.h"
#include "dependencies/classes/color.h"
#include "dependencies/classes/datamap.h"
#include "dependencies/classes/datatable.h"
#include "dependencies/classes/utlmemory.h"
#include "dependencies/classes/utlstack.h"
#include "dependencies/classes/utlvector.h"
#include "dependencies/classes/usercmd.h"
#include "dependencies/classes/matrix.h"
#include "dependencies/classes/bitbuf.h"
#include "dependencies/classes/bspflags.h"
#include "dependencies/classes/definitions.h"
#include "dependencies/classes/studio.h"
#include "dependencies/classes/fnva.h"
#include "dependencies/classes/inputsys.h"
#include "dependencies/classes/address.h"
#include "dependencies/classes/stack.h"
#include "dependencies/classes/autobaseptr.h"
#include "dependencies/classes/classvalues.h"
#include "dependencies/classes/clipraytrace.h"
#define CURL_STATICLIB
#include "dependencies/curl/curl.h"
#pragma comment(lib,"dependencies/curl/libcurl_a.lib")

#define FMT_HEADER_ONLY
#include "dependencies/fmt/format.h"
#include "dependencies/fmt/chrono.h"

#define RAPIDJSON_NOMEMBERITERATORCLASS 
#define CURL_STATICLIB
#include "dependencies/json/json.hpp"
using nlohmann::json;

#include "others/util.h"
#include "others/wapi.h"

#include "main/interfaces.h"

#include "main/handlers/netvarmanager.h"

#include "selfprotect/strencrypt.h"