
#include <gtest/gtest.h>
#include <karkinolution/core/id.hpp>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/terrain/octree/aabb.hpp>
#include <karkinolution/terrain/octree/octree.hpp>

using Karkinolution::Core::Id::gen_id;
using Karkinolution::Math::Units::Size;
using Karkinolution::Math::Vec::Vec3;
using Karkinolution::Organisms::Entities::create_creature_id;
using Karkinolution::Organisms::Entities::EntityId;
using Karkinolution::Terrain::Trees::Octree::AABB;
using Karkinolution::Terrain::Trees::Octree::MAX_ENTRIES;
using Karkinolution::Terrain::Trees::Octree::MAX_OCTREE_CHILDREN;
using Karkinolution::Terrain::Trees::Octree::Octree;
using Karkinolution::Terrain::Trees::Octree::OctreeEntry;
using Karkinolution::Terrain::Trees::Octree::OctreeNode;

/*
Invariants


remove() == true if it exists
remove() == false it doesn't exist

exists() == true if it exists
exists() == false it it doesn't exist

entries.size() never > MAX_ENTRIES
children.size() neve > MAX_CHILDREN_SIZE

find() == vector of entries that intersect
*/

namespace {
	AABB generate_random_aabb() {
		return AABB{.max = Karkinolution::Functional::KRandom::RandomGenerators::generate<Vec3>(),
					.min = Karkinolution::Functional::KRandom::RandomGenerators::generate<Vec3>()};
	}

	void fill_entries(std::vector<OctreeEntry> &entries, Octree &octree) {
		for (size_t i = 0; i < 100; i++) {
			const AABB aabb{
				.max = Karkinolution::Functional::KRandom::RandomGenerators::generate<Vec3>(),
				.min = Karkinolution::Functional::KRandom::RandomGenerators::generate<Vec3>()};
			const auto     id_value = gen_id();
			const EntityId id       = create_creature_id(id_value);
			entries.push_back(OctreeEntry{.entity_id = id, .bound = aabb});

			octree.root().insert(entries.back());
		}
	}

	void fill_entries(Octree &octree) {
		for (size_t i = 0; i < 100; i++) {
			const auto     aabb     = generate_random_aabb();
			const auto     id_value = gen_id();
			const EntityId id       = create_creature_id(id_value);

			octree.root().insert(OctreeEntry{.entity_id = id, .bound = aabb});
		}
	}

	void fill_ids(std::vector<EntityId> &ids) {
		for (size_t i = 0; i < 100; i++) {
			const auto     id_value = gen_id();
			const EntityId id       = create_creature_id(id_value);
			ids.push_back(id);
		}
	}

	void verify_children(OctreeNode &root) {
		ASSERT_TRUE(root.children_view().size() <= MAX_OCTREE_CHILDREN);

		for (const auto &child : root.children_view()) {
			if (child == nullptr) {
				continue;
			}
			verify_children(*child);
		}
	}

	Octree make_big_octree() {
		return Octree{Size{1000.0, 1000.0, 10000.0}};
	}

} // namespace

TEST(OctreeTest, RemoveIsEqualTrueIfItExists) {
	Octree octree = make_big_octree();

	std::vector<OctreeEntry> entries;
	fill_entries(entries, octree);

	for (size_t i = entries.size(); i-- > 0;) {
		const auto was_removed = octree.root().remove(entries[i].entity_id, entries[i].bound);
		ASSERT_TRUE(was_removed);
		entries.erase(entries.begin() + i);
	}

	fill_entries(entries, octree);

	for (size_t i = entries.size(); i-- > 0;) {
		const auto was_removed = octree.root().remove(entries[i].entity_id);
		ASSERT_TRUE(was_removed);
		entries.erase(entries.begin() + i);
	}
}

TEST(OctreeTest, RemoveIsEqualFalseItDoesntExist) {
	Octree octree = make_big_octree();

	std::vector<EntityId> ids;
	fill_ids(ids);

	for (size_t i = ids.size(); i-- > 0;) {
		const auto was_removed = octree.root().remove(ids[i]);
		ASSERT_FALSE(was_removed);
		ids.erase(ids.begin() + i);
	}

	fill_ids(ids);

	for (size_t i = ids.size(); i-- > 0;) {
		const auto was_removed = octree.root().remove(ids[i]);
		ASSERT_FALSE(was_removed);
	}
}

TEST(OctreeTest, AddEntryMakeEntryExists) {
	Octree octree = make_big_octree();

	std::vector<OctreeEntry> entries;

	fill_entries(entries, octree);
	for (size_t i = 0; i < entries.size(); i++) {
		ASSERT_TRUE(octree.root().exists(entries[i].entity_id));
	}
	for (size_t i = 0; i < entries.size(); i++) {
		ASSERT_TRUE(octree.root().exists(entries[i].entity_id, entries[i].bound));
	}
}

TEST(OctreeTest, DeleteEntryMakeEntryDoesntExist) {
	Octree                   octree = make_big_octree();
	std::vector<OctreeEntry> entries;

	fill_entries(entries, octree);
	for (size_t i = 0; i < entries.size(); i++) {
		octree.root().remove(entries[i].entity_id, entries[i].bound);
	}

	for (size_t i = 0; i < entries.size(); i++) {
		ASSERT_FALSE(octree.root().exists(entries[i].entity_id, entries[i].bound));
	}

	entries.clear();

	fill_entries(entries, octree);

	for (size_t i = 0; i < entries.size(); i++) {
		octree.root().remove(entries[i].entity_id);
	}
	for (size_t i = 0; i < entries.size(); i++) {
		ASSERT_FALSE(octree.root().exists(entries[i].entity_id, entries[i].bound));
	}
}

TEST(OctreeTest, AddChildrenNeverIsOverMax) {
	Octree octree = make_big_octree();

	std::vector<OctreeEntry> entries;
	fill_entries(entries, octree);

	for (size_t i = 0; i < entries.size(); i++) {
		verify_children(octree.root());
	}
}

TEST(OctreeTest, UpdateChildrenNeverIsOverMax) {
	Octree octree = make_big_octree();

	std::vector<OctreeEntry> entries;

	fill_entries(entries, octree);

	for (size_t i = 0; i < entries.size(); i++) {
		verify_children(octree.root());
	}
}