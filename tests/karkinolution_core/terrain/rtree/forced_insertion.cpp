#include "karkinolution/terrain/rtree/box.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <memory>
#include <optional>
#include <shared/math/physic/vec/model.hpp>
#include <string>
#include <variant>
#include <vector>

// =====================================================================================================================
// Regression test for the R*-tree "stale reinsertion depth" bug.
// Created: 2026-11-15
//
// HOW IT SHOWED UP
//   CI test #117 (WorldOwnership.ConcurrentAcquisitionCannotStealWorldDuringCreateAndConnect) aborted with
//   "terminate called after throwing an instance of 'std::bad_variant_access' / std::get: wrong index for variant".
//   It failed only on GitHub Actions. 1537 consecutive local runs never failed, and ThreadSanitizer, AddressSanitizer
//   and UBSan found nothing, with libstdc++, libc++ and several compilers. It looked like an impossible data race.
//
// WHY IT WAS NOT A DATA RACE
//   Only the creator thread builds the World; the other threads just wait on WorldStorage/Simulation mutexes. The
//   exception comes from a single-threaded logic error inside RStarTree, which sanitizers cannot see (it is not
//   undefined behavior). The concurrent test was only the place where the bug happened to surface.
//
// WHY IT WAS SO RARE AND SO HARD TO REPRODUCE
//   World::World() calls gen_terrain(), which creates one soil piece per grid cell (21 x 21 for a 100 x 100 world with
//   epsilon 5) with radius = KRandom::RandomGenerators::generate(10, 20). KRandom::gen is seeded with
//   std::random_device, so WorldPreset::seed = 1 only fixes the noise: the radii, and therefore the exact shape of the
//   R*-tree, change on every run. The bug needs a specific sequence of events to line up:
//     1. a leaf overflows and forced reinsertion removes some of its entries (recording their depth from the root);
//     2. reinserting the first one makes the root overflow, and the root split adds one level on top of the tree;
//     3. at least one more removed entry is still waiting, and is reinserted using the old, now stale, depth.
//   That entry (an id) lands in an INTERNAL node, which then holds ids and child nodes at the same time. A later
//   insert descends into it and RStarTree::choose_leaf does std::get<unique_ptr<Node>> on an id, throwing
//   std::bad_variant_access (rtree.hpp, choose_leaf).
//
//   Measured with a standalone harness that replays the 441 World insertions against the real rtree.hpp with fresh
//   random radii each time:
//     - ~0.05% to ~0.08% of world generations threw std::bad_variant_access (101 of 200000 and 45 of 60000 runs);
//     - ~1.2% of world generations silently left the tree corrupted without throwing (about 725 of 60000), so every
//       test or real world creation was exposed, not only test #117;
//     - 1537 local runs at that rate have roughly a 40% chance of seeing zero failures, while CI, which runs on every
//       push and over a whole build matrix, eventually hits it.
//   With the depth corrected, 60000 generations produced zero exceptions and zero corrupted trees.
//
// WHAT THE FIX MUST GUARANTEE
//   The depth used to reinsert an orphan must be derived from the current height of the tree, not captured before
//   the reinsertion loop. A root split during the loop shifts every level down by one.
//
// WHY THE TEST LOOKS LIKE THIS
//   - It validates the whole tree after every insert (using only the public API), so it points at the insert that
//     corrupts the tree instead of at a later, unrelated crash.
//   - The input mimics World generation (grid of soil pieces, radii in [10, 20], pieces leaving the world rejected),
//     the only place where the problem was observed.
//   - Seeds are fixed and the generator is our own (SplitMix64). std::uniform_real_distribution gives different
//     numbers on libstdc++ and libc++, and CI uses both, so the cases must not depend on the standard library.
//     The seeds below therefore belong to this generator and are unrelated to the seeds of the original harness.
//   - Because the bug appears in ~1.2% of terrain generations, a single ordinary example would almost never catch it.
//     Two complementary tests are used: pinned seeds that are known to reproduce it, and a deterministic fuzz over
//     1000 seeds (12 of them, about 1.2%, broke the tree before the fix: 44, 190, 277, 419, 457, 635, 714, 769, 796,
//     866, 944 and 980).
//
// EXPECTED RESULT
//   Before the fix: both tests fail (the fuzz reports 12 of 1000 seeds with "INTERNAL node holds an id").
//   After the fix:  both tests pass. In an optimized build they take roughly 0.4 s together.
// =====================================================================================================================

using Karkinolution::Terrain::Soil::SoilPieceId;
using Karkinolution::Terrain::Trees::RTree::Box2D;
using Karkinolution::Terrain::Trees::RTree::NodeType;
using Karkinolution::Terrain::Trees::RTree::RStarTree;
using Karkinolution::Terrain::Trees::RTree::RtreeEntry;
using Karkinolution::Terrain::Trees::RTree::RtreeNode;
using KShared::Math::Vec::Vec2;
namespace Box2DMotor = Karkinolution::Terrain::Trees::RTree::Box2DMotor;

using TRStarTree  = RStarTree<SoilPieceId>;
using TRTreeNode  = RtreeNode<SoilPieceId>;
using TRTreeEntry = RtreeEntry<SoilPieceId>;

namespace {
	constexpr double world_size = 100.0;
	constexpr double grid_step  = 5.0;
	constexpr double min_radius = 10.0;
	constexpr double max_radius = 20.0;

	class SplitMix64 {
		private:

			uint64_t state_;

		public:

			explicit SplitMix64(uint64_t seed)
				: state_(seed) {}

			uint64_t next() {
				uint64_t z = (state_ += 0x9E3779B97F4A7C15ULL);
				z          = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
				z          = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
				return z ^ (z >> 31);
			}

			// Uniform in [0, 1) with 53 bits of precision; exact in IEEE-754 doubles on every platform.
			double unit() {
				return static_cast<double>(next() >> 11) * 0x1.0p-53;
			}
	};

	// Same shape as TerrainFactory::gen_terrain + RSTService::add: one piece per grid cell, boxes that leave the
	// world are rejected.
	std::vector<Box2D> make_terrain_like_boxes(uint64_t seed) {
		SplitMix64         rng{seed};
		std::vector<Box2D> boxes;

		for (double x = 0.0; x <= world_size; x += grid_step) {
			for (double y = 0.0; y <= world_size; y += grid_step) {
				const double radius = min_radius + (max_radius - min_radius) * rng.unit();
				const Box2D  box{.max = Vec2{x + radius, y + radius}, .min = Vec2{x - radius, y - radius}};

				if (box.min.x < 0.0 || box.min.y < 0.0 || box.max.x > world_size || box.max.y > world_size) {
					continue;
				}
				boxes.push_back(box);
			}
		}
		return boxes;
	}

	std::optional<std::string>
	find_violation(const TRTreeNode &node, size_t depth, std::optional<size_t> &leaf_depth, bool is_root) {
		if (node.entries.size() > TRStarTree::MAX_ENTRIES) {
			return "node has more than MAX_ENTRIES entries";
		}
		if (!is_root && node.entries.size() < TRStarTree::MIN_ENTRIES) {
			return "non-root node has fewer than MIN_ENTRIES entries";
		}

		for (const TRTreeEntry &entry : node.entries) {
			const bool holds_id = std::holds_alternative<SoilPieceId>(entry.content);

			if (node.type == NodeType::LEAF && !holds_id) {
				return "LEAF node holds a child node";
			}
			if (node.type == NodeType::INTERNAL && holds_id) {
				return "INTERNAL node holds an id (entry landed above the leaf level)";
			}
		}

		if (node.type == NodeType::LEAF) {
			if (!leaf_depth.has_value()) {
				leaf_depth = depth;
			} else if (*leaf_depth != depth) {
				return "leaves are not all at the same depth";
			}
			return std::nullopt;
		}

		for (const TRTreeEntry &entry : node.entries) {
			const auto &child = std::get<std::unique_ptr<TRTreeNode>>(entry.content);

			if (child == nullptr) {
				return "INTERNAL node holds a null child";
			}
			if (auto violation = find_violation(*child, depth + 1, leaf_depth, false)) {
				return violation;
			}
		}
		return std::nullopt;
	}

	std::optional<std::string> check_tree(const TRStarTree &tree) {
		std::optional<size_t> leaf_depth;
		return find_violation(tree.root(), 0, leaf_depth, true);
	}

	// Inserts the terrain-like boxes of `seed` one by one, validating the whole tree after every insert.
	// Returns a description of the first problem, or std::nullopt when the tree stayed sound.
	std::optional<std::string> run_seed(uint64_t seed) {
		const auto               boxes = make_terrain_like_boxes(seed);
		TRStarTree               tree;
		std::vector<SoilPieceId> inserted;

		for (size_t i = 0; i < boxes.size(); ++i) {
			const auto id = static_cast<SoilPieceId>(i + 1);

			try {
				tree.insert(id, boxes[i]);
			} catch (const std::bad_variant_access &) {
				return "seed " + std::to_string(seed) + ": std::bad_variant_access while inserting entry #"
					+ std::to_string(i + 1);
			}
			inserted.push_back(id);

			if (auto violation = check_tree(tree)) {
				return "seed " + std::to_string(seed) + ": " + *violation + " (after insert #" + std::to_string(i + 1)
					+ ")";
			}
		}

		auto stored = tree.get_all_ids();
		std::ranges::sort(stored);
		if (stored != inserted) {
			return "seed " + std::to_string(seed) + ": stored ids differ from the inserted ids";
		}
		return std::nullopt;
	}

	// Seeds that corrupted the tree before the fix: the first group breaks when the tree first grows to three levels,
	// the second one later, when the root of a taller tree overflows during a reinsertion.
	constexpr std::array<uint64_t, 6> pinned_seeds{44, 190, 277, 635, 866, 944};
	constexpr uint64_t                fuzz_seed_count = 1000;
} // namespace

TEST(RStarTreeForcedReinsertion, RootSplitDuringReinsertionKeepsOrphansAtLeafLevel) {
	for (const uint64_t seed : pinned_seeds) {
		const auto problem = run_seed(seed);
		EXPECT_FALSE(problem.has_value()) << problem.value_or("");
	}
}

TEST(RStarTreeForcedReinsertion, TerrainLikeInsertionsKeepTreeStructurallySound) {
	std::vector<std::string> problems;

	for (uint64_t seed = 0; seed < fuzz_seed_count; ++seed) {
		if (auto problem = run_seed(seed)) {
			problems.push_back(std::move(*problem));
		}
	}

	std::string report;
	for (size_t i = 0; i < std::min<size_t>(problems.size(), 5); ++i) {
		report += "\n  " + problems[i];
	}
	EXPECT_TRUE(problems.empty()) << problems.size() << " of " << fuzz_seed_count << " seeds broke the tree:" << report;
}