#pragma once
#include <array>
#define RTV_MAX 7

using ShaderID = int;
using TextureID = int;
using SamplerID = int;
using BufferID = int;
using ViewportID = int;
using RenderTargetID = std::array<int, RTV_MAX>;

