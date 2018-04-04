#pragma once

#include <BWAPI.h>
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class ResourceManager
	{
	public:
		map<Unit, int> minerals{};
		map<Unit, int> gas{};
		ResourceManager();
		static void onMineralDestroy(Unit);
		static void initBaseResources();
		static ResourceManager & instance();
	};
}