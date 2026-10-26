#include "karkinolution/binary/protocol/contract.hpp"

#include <gtest/gtest.h>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/interpreters/geometry.hpp>
#include <karkinolution/binary/interpreters/stats.hpp>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/test/utils.hpp>
#include <karkinolution/utils/k_random.hpp>

// Math: Vec

TEST(MathSerializeDeserializeRoundtrip, Vec3) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const Vec3 original = RandomGenerators::generate<Vec3>();

		const auto                   vec_bytes = VecSRI::serialize_vec(original);
		const std::vector<std::byte> buffer(vec_bytes.begin(), vec_bytes.end());

		const Vec3 result = VecDSI::get_vec(buffer, 0);

		EXPECT_EQ(result, original);
	}
}

// Math: Stats - Full Value, Field Getters, and Type

TEST(MathSerializeDeserializeRoundtrip, Stats_AllAccessorsAndType) {
	// GenericRuntimeValue<double>
	{
		GenericRuntimeValue<double> stat(50.0, 100.0, 0.0);

		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer, 0),
				  Karkinolution::Binary::Protocol::NumericType::DOUBLE);
		EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, 0), stat.value());
		EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer, 0), stat.max());
		EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer, 0), stat.min());

		const auto result = StatsDSI::get_stats<double>(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, stat.value());
		EXPECT_DOUBLE_EQ(result.max, stat.max());
		EXPECT_DOUBLE_EQ(result.min, stat.min());
	}

	// GenericRuntimeValue<float>
	{
		GenericRuntimeValue<float> stat(25.5f, 50.0f, -10.0f);

		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer, 0),
				  Karkinolution::Binary::Protocol::NumericType::FLOAT);
		EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, 0), stat.value());
		EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer, 0), stat.max());
		EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer, 0), stat.min());

		const auto result = StatsDSI::get_stats<float>(buffer, 0);

		EXPECT_FLOAT_EQ(result.value, stat.value());
		EXPECT_FLOAT_EQ(result.max, stat.max());
		EXPECT_FLOAT_EQ(result.min, stat.min());
	}

	// GenericRuntimeValue<std::uint32_t>
	{
		GenericRuntimeValue<std::uint32_t> stat(75U, 200U, 0U);

		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer, 0),
				  Karkinolution::Binary::Protocol::NumericType::UINT32);
		EXPECT_EQ(StatsDSI::get_value<std::uint32_t>(buffer, 0), stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint32_t>(buffer, 0), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint32_t>(buffer, 0), stat.min());

		const auto result = StatsDSI::get_stats<std::uint32_t>(buffer, 0);

		EXPECT_EQ(result.value, stat.value());
		EXPECT_EQ(result.max, stat.max());
		EXPECT_EQ(result.min, stat.min());
	}

	// GenericRuntimeValue<std::uint64_t>
	{
		GenericRuntimeValue<std::uint64_t> stat(123456789ULL, 999999999ULL, 0ULL);

		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer, 0),
				  Karkinolution::Binary::Protocol::NumericType::UINT64);
		EXPECT_EQ(StatsDSI::get_value<std::uint64_t>(buffer, 0), stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint64_t>(buffer, 0), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint64_t>(buffer, 0), stat.min());

		const auto result = StatsDSI::get_stats<std::uint64_t>(buffer, 0);

		EXPECT_EQ(result.value, stat.value());
		EXPECT_EQ(result.max, stat.max());
		EXPECT_EQ(result.min, stat.min());
	}

	// GenericRuntimeValue<std::uint8_t>
	{
		GenericRuntimeValue<std::uint8_t> stat(15, 100, 0);

		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer, 0),
				  Karkinolution::Binary::Protocol::NumericType::UINT8);
		EXPECT_EQ(StatsDSI::get_value<std::uint8_t>(buffer, 0), stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint8_t>(buffer, 0), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint8_t>(buffer, 0), stat.min());

		const auto result = StatsDSI::get_stats<std::uint8_t>(buffer, 0);

		EXPECT_EQ(result.value, stat.value());
		EXPECT_EQ(result.max, stat.max());
		EXPECT_EQ(result.min, stat.min());
	}
}

// Math: PhysicsUnits Roundtrip

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits) {
	// Volume
	{
		constexpr Volume original{42.5};

		const auto                   bytes = UnitsSRI::serialize_volume(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_volume(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Mass
	{
		const PhysicsStats::Mass original{123.456};

		const auto                   bytes = UnitsSRI::serialize_mass(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_mass(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Density
	{
		const PhysicsStats::Density original{78.9};

		const auto                   bytes = UnitsSRI::serialize_density(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_density(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Meter
	{
		const Meter original{9.81};

		const auto                   bytes = UnitsSRI::serialize_meter(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_meter(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Lateral, Height, Depth
	{
		constexpr Size original{.lateral = Lateral{1.2},
								.height  = Height{3.4},
								.depth   = Depth{5.6}};

		const auto                   bytes = UnitsSRI::serialize_size(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_size(buffer, 0);

		EXPECT_DOUBLE_EQ(result.lateral.value, original.lateral.value);
		EXPECT_DOUBLE_EQ(result.height.value, original.height.value);
		EXPECT_DOUBLE_EQ(result.depth.value, original.depth.value);
	}
	// Size
	{
		constexpr Size original{.lateral = Lateral{2.5},
								.height  = Height{4.0},
								.depth   = Depth{1.5}};

		const auto                   bytes = UnitsSRI::serialize_size(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_size(buffer, 0);

		EXPECT_DOUBLE_EQ(result.lateral.value, original.lateral.value);
		EXPECT_DOUBLE_EQ(result.height.value, original.height.value);
		EXPECT_DOUBLE_EQ(result.depth.value, original.depth.value);
	}

	// SharedVolume
	{
		const PhysicsStats::SharedVolume original{0.75};

		const auto                   bytes = UnitsSRI::serialize_shared_volume(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_shared_volume(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value(), original.value());
	}

	// Efficiency
	{
		const Efficiency original{0.85f};

		const auto                   bytes = UnitsSRI::serialize_efficiency(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_efficiency(buffer, 0);

		EXPECT_FLOAT_EQ(result.value(), original.value());
	}

	// Quality
	{
		const Quality original{0.92f};

		const auto                   bytes = UnitsSRI::serialize_quality(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = UnitsDSI::deserialize_quality(buffer, 0);

		EXPECT_FLOAT_EQ(result.value(), original.value());
	}
}

// Math: Stats - NormalizedValue Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Stats_NormalizedValueRoundtrip) {
	// NormalizedValue<double>
	{
		for (double val : {0.0, 0.25, 0.5, 0.75, 1.0}) {
			NormalizedValue<double> original(val);

			const auto                   bytes = StatsSRI::serialize_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer, 0),
					  Karkinolution::Binary::Protocol::NumericType::DOUBLE);
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, 0), original.value());
			EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer, 0), 1.0);
			EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer, 0), 0.0);
		}

		// Clamping behavior
		{
			NormalizedValue<double> clamped_high(5.0);

			const auto bytes_h = StatsSRI::serialize_normalized_value(clamped_high);
			const std::vector<std::byte> buffer_h(bytes_h.begin(), bytes_h.end());

			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_h, 0), 1.0);

			NormalizedValue<double> clamped_low(-3.0);

			const auto bytes_l = StatsSRI::serialize_normalized_value(clamped_low);
			const std::vector<std::byte> buffer_l(bytes_l.begin(), bytes_l.end());

			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_l, 0), 0.0);
		}
	}

	// NormalizedValue<float>
	{
		for (float val : {0.0f, 0.33f, 0.66f, 1.0f}) {
			NormalizedValue<float> original(val);

			const auto                   bytes = StatsSRI::serialize_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer, 0),
					  Karkinolution::Binary::Protocol::NumericType::FLOAT);
			EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, 0), original.value());
			EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer, 0), 1.0f);
			EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer, 0), 0.0f);
		}
	}
}

// Math: Stats - SignedNormalizedValue Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Stats_SignedNormalizedValueRoundtrip) {
	// SignedNormalizedValue<double>
	{
		for (double val : {-1.0, -0.5, 0.0, 0.5, 1.0}) {
			SignedNormalizedValue<double> original(val);

			const auto bytes = StatsSRI::serialize_signed_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer, 0),
					  Karkinolution::Binary::Protocol::NumericType::DOUBLE);
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, 0), original.value());
			EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer, 0), 1.0);
			EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer, 0), -1.0);
		}

		// Clamping behavior
		{
			SignedNormalizedValue<double> clamped_high(10.0);

			const auto bytes_h = StatsSRI::serialize_signed_normalized_value(clamped_high);
			const std::vector<std::byte> buffer_h(bytes_h.begin(), bytes_h.end());

			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_h, 0), 1.0);

			SignedNormalizedValue<double> clamped_low(-10.0);

			const auto bytes_l = StatsSRI::serialize_signed_normalized_value(clamped_low);
			const std::vector<std::byte> buffer_l(bytes_l.begin(), bytes_l.end());

			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_l, 0), -1.0);
		}
	}

	// SignedNormalizedValue<float>
	{
		for (float val : {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f}) {
			SignedNormalizedValue<float> original(val);

			const auto bytes = StatsSRI::serialize_signed_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer, 0),
					  Karkinolution::Binary::Protocol::NumericType::FLOAT);
			EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, 0), original.value());
			EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer, 0), 1.0f);
			EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer, 0), -1.0f);
		}
	}
}

// Math: PhysicsUnits - Randomized Multi-iteration Roundtrip

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_RandomizedRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const double rand_val1 = RandomGenerators::generate<double>();
		const double rand_val2 = RandomGenerators::generate<double>();
		const double rand_val3 = RandomGenerators::generate<double>();
		const double rand_val4 = RandomGenerators::generate<double>();

		// Volume
		{
			const Volume original{rand_val1};

			const auto                   bytes = UnitsSRI::serialize_volume(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_volume(buffer, 0).value, original.value);
		}

		// Mass
		{
			const PhysicsStats::Mass original{rand_val2};

			const auto                   bytes = UnitsSRI::serialize_mass(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_mass(buffer, 0).value, original.value);
		}

		// Density
		{
			const PhysicsStats::Density original{rand_val3};

			const auto                   bytes = UnitsSRI::serialize_density(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_density(buffer, 0).value, original.value);
		}
		// Meter, Lateral, Height, Depth
		{
			const Meter orig_meter{rand_val4};

			const auto                   b_m = UnitsSRI::serialize_meter(orig_meter);
			const std::vector<std::byte> buffer_m(b_m.begin(), b_m.end());

			EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_meter(buffer_m, 0).value, orig_meter.value);
		}

		// Size
		{
			const Size original{.lateral = Lateral{rand_val1},
								.height  = Height{rand_val2},
								.depth   = Depth{rand_val3}};

			const auto                   bytes = UnitsSRI::serialize_size(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			const auto result = UnitsDSI::deserialize_size(buffer, 0);

			EXPECT_DOUBLE_EQ(result.lateral.value, original.lateral.value);
			EXPECT_DOUBLE_EQ(result.height.value, original.height.value);
			EXPECT_DOUBLE_EQ(result.depth.value, original.depth.value);
		}
	}
}

// Math: PhysicsUnits - Stream With Offsets

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_StreamWithOffsets) {
	const Volume                orig_vol{10.5};
	const PhysicsStats::Mass    orig_mass{200.0};
	const PhysicsStats::Density orig_dens{1.25};
	const Meter                 orig_meter{55.0};

	const Size orig_size{.lateral = Lateral{2.0}, .height = Height{3.0}, .depth = Depth{4.0}};

	const PhysicsStats::SharedVolume orig_shared{0.6};
	const Efficiency                 orig_eff{0.8f};
	const Quality                    orig_qual{0.9f};

	std::vector<std::byte> stream = {std::byte{0xDE}, std::byte{0xAD}};

	const size_t off_vol = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_volume(orig_vol));

	const size_t off_mass = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_mass(orig_mass));

	const size_t off_dens = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_density(orig_dens));

	const size_t off_meter = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_meter(orig_meter));

	const size_t off_size = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_size(orig_size));

	const size_t off_shared = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_shared_volume(orig_shared));

	const size_t off_eff = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_efficiency(orig_eff));

	const size_t off_qual = stream.size();
	TestUtils::append_to_buffer(stream, UnitsSRI::serialize_quality(orig_qual));

	stream.push_back(std::byte{0xBE});
	stream.push_back(std::byte{0xEF});

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_volume(stream, off_vol).value, orig_vol.value);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_mass(stream, off_mass).value, orig_mass.value);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_density(stream, off_dens).value, orig_dens.value);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_meter(stream, off_meter).value, orig_meter.value);

	const auto deserialized_size = UnitsDSI::deserialize_size(stream, off_size);

	EXPECT_DOUBLE_EQ(deserialized_size.lateral.value, orig_size.lateral.value);
	EXPECT_DOUBLE_EQ(deserialized_size.height.value, orig_size.height.value);
	EXPECT_DOUBLE_EQ(deserialized_size.depth.value, orig_size.depth.value);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_shared_volume(stream, off_shared).value(),
					 orig_shared.value());

	EXPECT_FLOAT_EQ(UnitsDSI::deserialize_efficiency(stream, off_eff).value(), orig_eff.value());

	EXPECT_FLOAT_EQ(UnitsDSI::deserialize_quality(stream, off_qual).value(), orig_qual.value());
}

// Math: PhysicsUnits - Size Operations and Volume Calculation

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_SizeOperations) {
	const Size size{.lateral = Lateral{2.0}, .height = Height{3.0}, .depth = Depth{4.0}};

	const auto                   bytes = UnitsSRI::serialize_size(size);
	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

	const auto deserialized = UnitsDSI::deserialize_size(buffer, 0);

	EXPECT_DOUBLE_EQ(deserialized.volume().value, 24.0);
	EXPECT_DOUBLE_EQ(deserialized.volume().value, size.volume().value);

	const auto static_vol =
		Size::volume(deserialized.lateral, deserialized.height, deserialized.depth);

	EXPECT_DOUBLE_EQ(static_vol.value, 24.0);

	Size modified = deserialized;
	modified.modify(NormalizedValue<float>(0.5f));

	EXPECT_DOUBLE_EQ(modified.lateral.value, 1.0);
	EXPECT_DOUBLE_EQ(modified.height.value, 1.5);
	EXPECT_DOUBLE_EQ(modified.depth.value, 2.0);
	EXPECT_DOUBLE_EQ(modified.volume().value, 3.0);

	const auto                   mod_bytes = UnitsSRI::serialize_size(modified);
	const std::vector<std::byte> mod_buffer(mod_bytes.begin(), mod_bytes.end());

	const auto mod_deserialized = UnitsDSI::deserialize_size(mod_buffer, 0);

	EXPECT_DOUBLE_EQ(mod_deserialized.volume().value, 3.0);
}

// Math: PhysicsUnits - Insufficient Buffer Size Throws Error

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;
	const std::vector<std::byte> short_buffer = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};

	EXPECT_THROW(UnitsDSI::deserialize_volume(empty_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_volume(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_mass(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_density(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_meter(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_lateral(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_height(short_buffer, 0), ByteError);
	EXPECT_THROW(UnitsDSI::deserialize_depth(short_buffer, 0), ByteError);

	const std::vector<std::byte> size_short_buffer(16, std::byte{0x01});
	EXPECT_THROW(UnitsDSI::deserialize_size(size_short_buffer, 0), ByteError);

	// SharedVolume reads a double at offset 1, requiring at least 9 bytes.
	const std::vector<std::byte> shared_vol_short_buffer(8, std::byte{0x01});
	EXPECT_THROW(UnitsDSI::deserialize_shared_volume(shared_vol_short_buffer, 5), ByteError);

	// Efficiency and Quality read a float at offset 1, requiring at least 5 bytes.
	const std::vector<std::byte> stat_float_short_buffer(4, std::byte{0x01});

	EXPECT_THROW(UnitsDSI::deserialize_efficiency(stat_float_short_buffer, 0), ByteError);

	EXPECT_THROW(UnitsDSI::deserialize_quality(stat_float_short_buffer, 0), ByteError);
}

// Math: Geometry Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Geometry_Roundtrip) {
	// Radius
	{
		const GeometryForms::Radius original{5.0};

		const auto                   bytes = GeometrySRI::serialize_radius(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = GeometryDSI::deserialize_radius(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Circumference
	{
		constexpr GeometryForms::Circumference original{31.41592653589793};

		const auto                   bytes = GeometrySRI::serialize_circumference(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = GeometryDSI::deserialize_circumference(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Diameter
	{
		constexpr GeometryForms::Diameter original{10.0};

		const auto                   bytes = GeometrySRI::serialize_diameter(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = GeometryDSI::deserialize_diameter(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Area
	{
		constexpr GeometryForms::Area original{78.53981633974483};

		const auto                   bytes = GeometrySRI::serialize_area(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto result = GeometryDSI::deserialize_area(buffer, 0);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}
}

// Math: Geometry - Randomized Multi-iteration Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Geometry_RandomizedRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const double rand_r = RandomGenerators::generate<double>();
		const double rand_c = RandomGenerators::generate<double>();
		const double rand_d = RandomGenerators::generate<double>();
		const double rand_a = RandomGenerators::generate<double>();

		// Radius
		{
			const GeometryForms::Radius original{rand_r};

			const auto                   bytes = GeometrySRI::serialize_radius(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_radius(buffer, 0).value, original.value);
		}

		// Circumference
		{
			const GeometryForms::Circumference original{rand_c};

			const auto                   bytes = GeometrySRI::serialize_circumference(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_circumference(buffer, 0).value,
							 original.value);
		}

		// Diameter
		{
			const GeometryForms::Diameter original{rand_d};

			const auto                   bytes = GeometrySRI::serialize_diameter(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_diameter(buffer, 0).value, original.value);
		}

		// Area
		{
			const GeometryForms::Area original{rand_a};

			const auto                   bytes = GeometrySRI::serialize_area(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_area(buffer, 0).value, original.value);
		}
	}
}

// Math: Geometry - Stream With Offsets

TEST(MathSerializeDeserializeRoundtrip, Geometry_StreamWithOffsets) {
	constexpr GeometryForms::Radius        orig_radius{7.5};
	constexpr GeometryForms::Circumference orig_circ{47.12388980384689};
	constexpr GeometryForms::Diameter      orig_diam{15.0};
	constexpr GeometryForms::Area          orig_area{176.71458676442586};

	std::vector<std::byte> stream = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};

	const size_t off_r = stream.size();
	TestUtils::append_to_buffer(stream, GeometrySRI::serialize_radius(orig_radius));

	const size_t off_c = stream.size();
	TestUtils::append_to_buffer(stream, GeometrySRI::serialize_circumference(orig_circ));

	const size_t off_d = stream.size();
	TestUtils::append_to_buffer(stream, GeometrySRI::serialize_diameter(orig_diam));

	const size_t off_a = stream.size();
	TestUtils::append_to_buffer(stream, GeometrySRI::serialize_area(orig_area));

	stream.push_back(std::byte{0xDD});
	stream.push_back(std::byte{0xEE});

	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_radius(stream, off_r).value, orig_radius.value);

	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_circumference(stream, off_c).value, orig_circ.value);

	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_diameter(stream, off_d).value, orig_diam.value);

	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_area(stream, off_a).value, orig_area.value);
}

// Math: Geometry - Insufficient Buffer Size Throws Error

TEST(MathSerializeDeserializeRoundtrip, Geometry_InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;
	const std::vector<std::byte> short_buffer = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};

	EXPECT_THROW(GeometryDSI::deserialize_radius(empty_buffer, 0), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_radius(short_buffer, 0), ByteError);

	EXPECT_THROW(GeometryDSI::deserialize_circumference(empty_buffer, 0), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_circumference(short_buffer, 0), ByteError);

	EXPECT_THROW(GeometryDSI::deserialize_diameter(empty_buffer, 0), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_diameter(short_buffer, 0), ByteError);

	EXPECT_THROW(GeometryDSI::deserialize_area(empty_buffer, 0), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_area(short_buffer, 0), ByteError);

	const std::vector<std::byte> valid_size_buffer(sizeof(double), std::byte{0x01});

	EXPECT_THROW(GeometryDSI::deserialize_radius(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_circumference(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_diameter(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_area(valid_size_buffer, 1), ByteError);
}