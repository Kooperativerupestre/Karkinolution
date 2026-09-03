#pragma once

#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class LetterPool {
	public:

		explicit LetterPool(std::string_view letters);

		std::vector<std::string> to_list() const;

		std::string pick() const;

	private:

		std::u32string                                m_letters_u32;
		mutable std::uniform_int_distribution<size_t> m_dist;
};

namespace Pools {
inline extern const LetterPool vowels;
inline extern const LetterPool long_vowels;
inline extern const LetterPool high_vowels;
inline extern const LetterPool general_vowels;

inline extern const LetterPool plosives;
inline extern const LetterPool fricatives;
inline extern const LetterPool liquids;
inline extern const LetterPool nasals;
inline extern const LetterPool approximants;

inline extern const LetterPool consonants;
} // namespace Pools

class Pool {
	public:

		using CommandFunc = std::string                                (*)();
		static const std::unordered_map<std::string_view, CommandFunc> commands;

		static std::string interpret(std::string_view command);

		static std::string
		weight_pick(const std::vector<std::pair<std::string, double>> &commands_weights);
};

std::string gen_name();