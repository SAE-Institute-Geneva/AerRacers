#ifdef VALIDATION_LAYERS
namespace neko::vk
{
void CheckVkError(VkResult err, const char* msg, const char* file, int line)
{
	std::string error;
	if (err >= 0 || err == VK_ERROR_OUT_OF_DATE_KHR) error += "[Info] ";
	else error += "[Error] ";
	switch (err)
	{
		case VK_SUCCESS: return;
		case VK_NOT_READY: error += "VK Not Ready"; break;
		case VK_TIMEOUT: error += "VK Timeout"; break;
		case VK_EVENT_SET: error += "VK Event Set"; break;
		case VK_EVENT_RESET: error += "VK Event Reset"; break;
		case VK_INCOMPLETE: error += "VK Incomplete"; break;

		case VK_ERROR_OUT_OF_HOST_MEMORY: error += "VK Out of Host Memory"; break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: error += "VK Out of Device Memory"; break;
		case VK_ERROR_OUT_OF_POOL_MEMORY: error += "VK Out of Pool Memory"; break;

		case VK_ERROR_INITIALIZATION_FAILED: error += "VK Initialization Failed"; break;
		case VK_ERROR_MEMORY_MAP_FAILED: error += "VK Memory Map Failed"; break;

		case VK_ERROR_DEVICE_LOST: error += "VK Device Lost"; break;

		case VK_ERROR_SURFACE_LOST_KHR: error += "VK Surface Lost KHR"; break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: error += "VK Native Window in Use KHR"; break;
		case VK_SUBOPTIMAL_KHR: error += "VK Suboptimal KHR"; break;
		case VK_ERROR_OUT_OF_DATE_KHR: error += "VK Out of Date KHR"; break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: error += "VK Incompatible Display KHR"; break;

		case VK_ERROR_LAYER_NOT_PRESENT: error += "VK Layer Not Present"; break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: error += "VK Extension Not Present"; break;
		case VK_ERROR_FEATURE_NOT_PRESENT: error += "VK Feature Not Present"; break;

		case VK_ERROR_INCOMPATIBLE_DRIVER: error += "VK Incompatible Driver"; break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED: error += "VK Format Not Supported"; break;

		case VK_ERROR_TOO_MANY_OBJECTS: error += "VK Too Many Objects"; break;
		case VK_ERROR_FRAGMENTED_POOL: error += "VK Fragmented Pool"; break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE: error += "VK Invalid External Handle"; break;
		case VK_ERROR_FRAGMENTATION: error += "VK Fragmentation"; break;
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			error += "VK Invalid Opaque Capture Address";
			break;

		case VK_ERROR_INVALID_SHADER_NV: error += "VK Invalid Shader NV"; break;

		case VK_ERROR_VALIDATION_FAILED_EXT: error += "VK Validation Failed EXT"; break;
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			error += "VK Invalid DRM Format Modifier Plane Layout EXT";
			break;
		case VK_ERROR_NOT_PERMITTED_EXT: error += "VK Not Permitted EXT"; break;
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			error += "VK Full Screen Exclusive Mode Lost EXT";
			break;

		case VK_THREAD_IDLE_KHR: error += "VK Thread Idle KHR"; break;
		case VK_THREAD_DONE_KHR: error += "VK Thread Done KHR"; break;
		case VK_OPERATION_DEFERRED_KHR: error += "VK Operation Deferred KHR"; break;
		case VK_OPERATION_NOT_DEFERRED_KHR: error += "VK Operation Not Deferred KHR"; break;
		case VK_PIPELINE_COMPILE_REQUIRED_EXT: error += "VK Pipeline Compile Required EXT"; break;
		case VK_RESULT_MAX_ENUM: error += "VK Result Max Enum"; break;

		case VK_ERROR_UNKNOWN: error += "VK Unknown"; break;
	}

	if (err >= 0 || err == VK_ERROR_OUT_OF_DATE_KHR)
		logDebug(fmt::format("{}: {} | At line {} in {}", error, msg, line, file));
	else
		neko_assert(false, fmt::format("{}: {} | At line {} in {}", error, msg, line, file))
}
}    // namespace neko::vk
#endif