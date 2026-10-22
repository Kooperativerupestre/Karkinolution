#pragma once
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/binary/protocol/layouts.hpp>

namespace ProtocolLayout = Karkinolution::Binary::Protocol::Layout;

using Karkinolution::Binary::Protocol::Aliases::RequestId;
using Karkinolution::Binary::Protocol::Types::BinaryTypes;
using RequestSubTypes  = Karkinolution::Binary::Protocol::Types::BinarySubTypes::Request;
using ResponseSubTypes = Karkinolution::Binary::Protocol::Types::BinarySubTypes::Response;
using ErrorSubTypes    = Karkinolution::Binary::Protocol::Types::BinarySubTypes::Error;
using GenericResponseSubTypes =
	Karkinolution::Binary::Protocol::Types::BinarySubTypes::GenericResponseSubType;
using Karkinolution::Binary::Protocol::Types::RequestResource;

using FrameDeclaredSizeType    = Karkinolution::Binary::Protocol::Aliases::FrameDeclaredSize;
using FrameBinaryTypesEnumType = Karkinolution::Binary::Protocol::Aliases::BinaryTypesEnumType;
using FrameSubTypeEnumType     = Karkinolution::Binary::Protocol::Aliases::BinarySubTypesEnumType;
using RequestResourceEnumType  = Karkinolution::Binary::Protocol::Aliases::RequestResourceEnumType;
using Karkinolution::Binary::Protocol::Types::BinaryNLT;
