#include <gtest/gtest.h>
#include <karkinolution/binary/deserialization/interpreters/math/vec.hpp>
#include <karkinolution/binary/serialization/interpreters/math/ vec.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <vector>

TEST(MathSerializeDeserializeRoundtrip, Vec3) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const Vec3 original = RandomGenerators::generate<Vec3>();

		const auto                   vec_bytes = VecSRI::serialize_vec(original);
		const std::vector<std::byte> buffer(vec_bytes.begin(), vec_bytes.end());

		const Vec3 result = VecDSI::deserialize_vec(buffer, 0);

		EXPECT_EQ(result, original);
	}
}