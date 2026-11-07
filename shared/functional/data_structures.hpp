#pragma once

#include <unordered_map>
#include <utility>

namespace KShared {
	namespace Functional {

		template <typename K, typename V> [[nodiscard]] V* try_at(std::unordered_map<K, V> &map, const K &key) {
			auto it = map.find(key);
			return it != map.end() ? &it->second : nullptr;
		}

		template <typename K, typename V>
		[[nodiscard]] const V* try_at(const std::unordered_map<K, V> &map, const K &key) {
			auto it = map.find(key);
			return it != map.end() ? &it->second : nullptr;
		}

		template <typename K, typename V>
		[[nodiscard]] bool try_add(std::unordered_map<K, V> &map, const K &key, V &&value) {
			return map.emplace(key, std::move(value)).second;
		}

		template <typename K, typename V>
		[[nodiscard]] bool try_add(std::unordered_map<K, V> &map, const K &key, const V &value) {
			return map.emplace(key, value).second;
		}

		template <typename K, typename V>
		[[nodiscard]] bool try_add(std::unordered_map<K, V> &map, K &&key, V &&value) {
			return map.emplace(std::move(key), std::move(value)).second;
		}

		template <typename K, typename V>
		[[nodiscard]] bool try_add(std::unordered_map<K, V> &map, K &&key, const V &value) {
			return map.emplace(std::move(key), value).second;
		}

		template <typename K, typename V> [[nodiscard]] bool try_remove(std::unordered_map<K, V> &map, const K &key) {
			return map.erase(key) > 0;
		}
	} // namespace Functional
} // namespace KShared