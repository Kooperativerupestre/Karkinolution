#pragma once
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/binary/protocol/layouts.hpp>

namespace ProtocolLayout = Karkinolution::Binary::Protocol::Layout;

using Karkinolution::Binary::Protocol::NumericType;
using Karkinolution::Binary::Protocol::Aliases::FrameDeclaredSize;

using Karkinolution::Binary::Protocol::Aliases::RequestId;
using Karkinolution::Binary::Protocol::Request::Resource;
using BinaryTypes         = Karkinolution::Binary::Protocol::Type;
using ResponseCardinality = Karkinolution::Binary::Protocol::Response::Cardinality;

using RequestSubTypes  = Karkinolution::Binary::Protocol::Request::Operation;
using ResponseSubTypes = Karkinolution::Binary::Protocol::Response::Code;
using ErrorSubTypes    = Karkinolution::Binary::Protocol::Error::Code;
