#pragma once

#include <karkinolution/networking/frame/world/models.hpp>
#include <variant>

using ResponseFrame        = Karkinolution::Networking::Frames::World::Models::Response::Response;
using RequestFrame         = Karkinolution::Networking::Frames::World::Models::Request::Request;
using ErrorFrame           = Karkinolution::Networking::Frames::World::Models::Error::Error;
using ResponseFrameVariant = std::variant<ResponseFrame, ErrorFrame>;