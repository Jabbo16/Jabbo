#pragma once
#include <BWAPI.h>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class InfoManager
	{
	public:
		InfoManager();
		static InfoManager & instance();
	};
}