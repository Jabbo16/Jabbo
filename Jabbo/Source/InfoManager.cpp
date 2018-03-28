#include "InfoManager.hpp"

using namespace BWAPI;

namespace Jabbo {
	InfoManager::InfoManager()
		= default;

	InfoManager & InfoManager::instance()
	{
		static InfoManager instance;
		return instance;
	}
}