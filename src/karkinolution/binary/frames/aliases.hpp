#pragma once

#include <karkinolution/binary/frames/models.hpp>

#include <variant>

using ResponseFrame = Karkinolution::Binary::Frames::Models::Response::Response;
using RequestFrame  = Karkinolution::Binary::Frames::Models::Request::Request;
using ErrorFrame    = Karkinolution::Binary::Frames::Models::Error::Error;
using ResponseFrameVariant = std::variant<ResponseFrame, ErrorFrame>;