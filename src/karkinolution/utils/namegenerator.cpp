#include "namegenerator.hpp"

#include "karkinolution/utils/k_random.hpp"

#include <cassert>
#include <cstdint>

namespace {
	std::string utf32_to_utf8(const std::u32string &u32) {
		std::string utf8;
		utf8.reserve(u32.size() * 4);

		for (char32_t cp : u32) {
			if (cp <= 0x7F) {
				utf8.push_back(static_cast<char>(cp));
			} else if (cp <= 0x7FF) {
				utf8.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
				utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
			} else if (cp <= 0xFFFF) {
				utf8.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
				utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
				utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
			} else if (cp <= 0x10FFFF) {
				utf8.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
				utf8.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
				utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
				utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
			}
		}
		return utf8;
	}

	std::u32string utf8_to_utf32(std::string_view utf8) {
		std::u32string u32;
		u32.reserve(utf8.size());

		size_t i = 0;
		while (i < utf8.size()) {
			uint8_t  byte          = static_cast<uint8_t>(utf8[i]);
			char32_t cp            = 0;
			size_t   bytes_to_read = 0;

			if (byte <= 0x7F) {
				cp            = byte;
				bytes_to_read = 0;
			} else if ((byte & 0xE0) == 0xC0) {
				cp            = byte & 0x1F;
				bytes_to_read = 1;
			} else if ((byte & 0xF0) == 0xE0) {
				cp            = byte & 0x0F;
				bytes_to_read = 2;
			} else if ((byte & 0xF8) == 0xF0) {
				cp            = byte & 0x07;
				bytes_to_read = 3;
			} else {
				i++;
				continue;
			}

			if (i + bytes_to_read >= utf8.size()) {
				break;
			}

			for (size_t j = 0; j < bytes_to_read; ++j) {
				i++;
				uint8_t next_byte = static_cast<uint8_t>(utf8[i]);
				cp                = (cp << 6) | (next_byte & 0x3F);
			}
			u32.push_back(cp);
			i++;
		}
		return u32;
	}

	char32_t custom_towupper(char32_t cp) {
		if (cp >= U'a' && cp <= U'z') {
			return cp - (U'a' - U'A');
		}

		static const std::unordered_map<char32_t, char32_t> lowercase_to_uppercase = {{U'á', U'Á'},
																					  {U'é', U'É'},
																					  {U'í', U'Í'},
																					  {U'ó', U'Ó'},
																					  {U'ú', U'Ú'},
																					  {U'ý', U'Ý'},
																					  {U'ā', U'Ā'},
																					  {U'ē', U'Ē'},
																					  {U'ī', U'Ī'},
																					  {U'ō', U'Ō'},
																					  {U'ū', U'Ū'},
																					  {U'ȳ', U'Ȳ'}};

		if (auto it = lowercase_to_uppercase.find(cp); it != lowercase_to_uppercase.end()) {
			return it->second;
		}
		return cp;
	}
} // namespace

LetterPool::LetterPool(std::string_view letters)
	: m_letters_u32(utf8_to_utf32(letters))
	, m_dist(0, m_letters_u32.empty() ? 0 : m_letters_u32.size() - 1) {}

std::vector<std::string> LetterPool::to_list() const {
	std::vector<std::string> list;
	list.reserve(m_letters_u32.size());
	for (char32_t c : m_letters_u32) {
		list.push_back(utf32_to_utf8(std::u32string(1, c)));
	}
	return list;
}

std::string LetterPool::pick() const {
	if (m_letters_u32.empty()) {
		return "";
	}
	char32_t chosen = m_letters_u32[m_dist(gen)];
	return utf32_to_utf8(std::u32string(1, chosen));
}

namespace Pools {
	inline const LetterPool vowels{"aeiouy"};
	inline const LetterPool long_vowels{"āēīōūȳ"};
	inline const LetterPool high_vowels{"áéíóúý"};
	inline const LetterPool general_vowels{"aeiouyāēīōūȳáéíóúý"};

	inline const LetterPool plosives{"pbtdkg"};
	inline const LetterPool fricatives{"fvszh"};
	inline const LetterPool liquids{"rl"};
	inline const LetterPool nasals{"mn"};
	inline const LetterPool approximants{"jw"};

	inline const LetterPool consonants{"pbtdkgfvszhrlmnjw"};
} // namespace Pools

const std::unordered_map<std::string_view, Pool::CommandFunc> Pool::commands = {
	{"v",
	 []() {
		 return Pools::vowels.pick();
	 }},
	{"lv",
	 []() {
		 return Pools::long_vowels.pick();
	 }},
	{"hv",
	 []() {
		 return Pools::high_vowels.pick();
	 }},
	{"gv",
	 []() {
		 return Pools::general_vowels.pick();
	 }},
	{"p",
	 []() {
		 return Pools::plosives.pick();
	 }},
	{"f",
	 []() {
		 return Pools::fricatives.pick();
	 }},
	{"l",
	 []() {
		 return Pools::liquids.pick();
	 }},
	{"n",
	 []() {
		 return Pools::nasals.pick();
	 }},
	{"a",
	 []() {
		 return Pools::approximants.pick();
	 }},
	{"c", []() {
		 return Pools::consonants.pick();
	 }}};

std::string Pool::interpret(std::string_view command) {
	std::string result;
	result.reserve(command.size() * 2);

	std::string token;
	for (char c : command) {
		if (c == '/') {
			if (!token.empty()) {
				if (auto it = commands.find(token); it != commands.end()) {
					result += it->second();
				} else {
					assert(false && "Pool::interpret: unknown token, falling back to literal");
					result += token;
				}
				token.clear();
			}
		} else {
			token += c;
		}
	}

	if (!token.empty()) {
		if (auto it = commands.find(token); it != commands.end()) {
			result += it->second();
		} else {
			assert(false && "Pool::interpret: unknown token, falling back to literal");
			result += token;
		}
	}

	return result;
}

std::string Pool::weight_pick(const std::vector<std::pair<std::string, double>> &commands_weights) {
	if (commands_weights.empty()) {
		return "";
	}

	std::vector<std::string> items;
	std::vector<double>      weights;
	items.reserve(commands_weights.size());
	weights.reserve(commands_weights.size());

	for (const auto &[item, weight] : commands_weights) {
		items.push_back(item);
		weights.push_back(weight);
	}

	auto selected = Choices::choices(items, weights, 1);
	if (selected.empty()) {
		return "";
	}

	return Pool::interpret(selected.front());
}

std::string gen_name() {
	static const std::vector<std::pair<std::string, double>> patterns = {{"c/gv/gv/a", 2.0},
																		 {"c/hv/a/v", 2.0},
																		 {"hv/v/c/f", 1.0}};

	std::string raw_name = Pool::weight_pick(patterns);

	if (!raw_name.empty()) {
		std::u32string u32_name = utf8_to_utf32(raw_name);
		if (!u32_name.empty()) {
			u32_name[0] = custom_towupper(u32_name[0]);
			return utf32_to_utf8(u32_name);
		}
	}

	return raw_name;
}