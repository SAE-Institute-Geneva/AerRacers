#pragma once
#include <cinttypes>

namespace neko::vk
{
struct PipelineStage
{
	std::uint32_t renderPassId = 0;
	std::uint32_t subPassId    = 0;

	PipelineStage() = default;
	PipelineStage(const std::uint32_t renderPassId, const std::uint32_t subPassId)
		: renderPassId(renderPassId), subPassId(subPassId)
	{}

	bool operator==(const PipelineStage& right) const
	{
		return renderPassId == right.renderPassId && subPassId == right.subPassId;
	}

	bool operator!=(const PipelineStage& right) const { return !(*this == right); }

	bool operator<(const PipelineStage& right) const
	{
		return renderPassId < right.renderPassId ||
		       (right.renderPassId >= renderPassId && subPassId < right.subPassId);
	}

	bool operator>(const PipelineStage& right) const { return right < *this; }
};
}
