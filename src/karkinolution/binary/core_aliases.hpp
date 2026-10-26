#pragma once
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/binary/protocol/layouts.hpp>

namespace ProtocolLayout = Karkinolution::Binary::Protocol::Layout;

using Karkinolution::Binary::Protocol::Aliases::FrameDeclaredSize;
using Karkinolution::Binary::Protocol::Aliases::RequestId;

using Karkinolution::Binary::Protocol::ErrorCode;
using Karkinolution::Binary::Protocol::NumericType;
using Karkinolution::Binary::Protocol::Operation;
using Karkinolution::Binary::Protocol::Resource;
using Karkinolution::Binary::Protocol::ResponseCode;
using BinaryTypes = Karkinolution::Binary::Protocol::Type;

using RequestSubTypes  = Operation;
using ResponseSubTypes = ResponseCode;
using ErrorSubTypes    = ErrorCode;