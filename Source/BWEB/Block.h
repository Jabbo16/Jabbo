#pragma once
#include "BWEB.h"

namespace BWEB
{
	using namespace BWAPI;
	using namespace std;

	class Block
	{
		int w, h;
		TilePosition t;
		set <TilePosition> smallBlock, mediumBlock, largeBlock;
	public:
		Block() { };
		Block(int width, int height, TilePosition tile) { w = width, h = height, t = tile; }
		int width() { return w; }
		int height() { return h; }

		// Returns the top left tile position of this block
		const TilePosition Location() { return t; }

		// Returns the const set of tilepositions that belong to 2x2 (small) buildings
		const set<TilePosition>& SmallTiles() { return smallBlock; }

		// Returns the const set of tilepositions that belong to 3x2 (medium) buildings
		const set<TilePosition>& MediumTiles() { return mediumBlock; }

		// Returns the const set of tilepositions that belong to 4x3 (large) buildings
		const set<TilePosition>& LargeTiles() { return largeBlock; }

		void insertSmall(TilePosition here) { smallBlock.insert(here); }
		void insertMedium(TilePosition here) { mediumBlock.insert(here); }
		void insertLarge(TilePosition here) { largeBlock.insert(here); }
	};
}
