#ifdef NEKO_ASSERT
namespace neko::vk
{
void CheckVkError(VkResult err, const char* msg, const char* file, int line)
{
	std::string log;
	if (err >= 0 || err == VK_ERROR_OUT_OF_DATE_KHR) log += "[Info] ";
	else log += "[Error] ";
	switch (err)
	{
		case VK_SUCCESS: return;
		case VK_NOT_READY: log += "VK Not Ready"; break;
		case VK_TIMEOUT: log += "VK Timeout"; break;
		case VK_EVENT_SET: log += "VK Event Set"; break;
		case VK_EVENT_RESET: log += "VK Event Reset"; break;
		case VK_INCOMPLETE: log += "VK Incomplete"; break;

		case VK_ERROR_OUT_OF_HOST_MEMORY: log += "VK Out of Host Memory"; break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: log += "VK Out of Device Memory"; break;
		case VK_ERROR_OUT_OF_POOL_MEMORY: log += "VK Out of Pool Memory"; break;

		case VK_ERROR_INITIALIZATION_FAILED: log += "VK Initialization Failed"; break;
		case VK_ERROR_MEMORY_MAP_FAILED: log += "VK Memory Map Failed"; break;

		case VK_ERROR_DEVICE_LOST: log += "VK Device Lost"; break;

		case VK_ERROR_SURFACE_LOST_KHR: log += "VK Surface Lost KHR"; break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: log += "VK Native Window in Use KHR"; break;
		case VK_ERROR_OUT_OF_DATE_KHR: log += "VK Out of Date KHR"; break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: log += "VK Incompatible Display KHR"; break;

		case VK_ERROR_LAYER_NOT_PRESENT: log += "VK Layer Not Present"; break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: log += "VK Extension Not Present"; break;
		case VK_ERROR_FEATURE_NOT_PRESENT: log += "VK Feature Not Present"; break;

		case VK_ERROR_INCOMPATIBLE_DRIVER: log += "VK Incompatible Driver"; break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED: log += "VK Format Not Supported"; break;

		case VK_ERROR_TOO_MANY_OBJECTS: log += "VK Too Many Objects"; break;
		case VK_ERROR_FRAGMENTED_POOL: log += "VK Fragmented Pool"; break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE: log += "VK Invalid External Handle"; break;
		case VK_ERROR_FRAGMENTATION: log += "VK Fragmentation"; break;
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			log += "VK Invalid Opaque Capture Address";
			break;

		case VK_ERROR_INVALID_SHADER_NV: log += "VK Invalid Shader NV"; break;

		case VK_ERROR_VALIDATION_FAILED_EXT: log += "VK Validation Failed EXT"; break;
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			log += "VK Invalid DRM Format Modifier Plane Layout EXT";
			break;
		case VK_ERROR_NOT_PERMITTED_EXT: log += "VK Not Permitted EXT"; break;
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			log += "VK Full Screen Exclusive Mode Lost EXT";
			break;

		case VK_SUBOPTIMAL_KHR: log += "VK Suboptimal KHR"; break;
		case VK_THREAD_IDLE_KHR: log += "VK Thread Idle KHR"; break;
		case VK_THREAD_DONE_KHR: log += "VK Thread Done KHR"; break;
		case VK_OPERATION_DEFERRED_KHR: log += "VK Operation Deferred KHR"; break;
		case VK_OPERATION_NOT_DEFERRED_KHR: log += "VK Operation Not Deferred KHR"; break;
		case VK_PIPELINE_COMPILE_REQUIRED_EXT: log += "VK Pipeline Compile Required EXT"; break;
		case VK_RESULT_MAX_ENUM: log += "VK Result Max Enum"; break;

		case VK_ERROR_UNKNOWN: log += "VK Unknown"; break;
	}

	if (err >= 0 || err == VK_ERROR_OUT_OF_DATE_KHR) logDebug(log);
	else neko_assert(false, fmt::format("{}: {} | At line {} in {}", log, msg, line, file));
}
void PrintKtxError(ktx_error_code_e result, const char* file, int line)
{
	std::string log;
	switch (result)
	{
		case KTX_FILE_DATA_ERROR: log = "[KTX] Error file data error"; break;
		case KTX_FILE_ISPIPE: log = "[KTX] Error file is pipe"; break;
		case KTX_FILE_OPEN_FAILED: log = "[KTX] Error file open failed"; break;
		case KTX_FILE_OVERFLOW: log = "[KTX] Error file overflow"; break;
		case KTX_FILE_READ_ERROR: log = "[KTX] Error file read error"; break;
		case KTX_FILE_SEEK_ERROR: log = "[KTX] Error file seek error"; break;
		case KTX_FILE_UNEXPECTED_EOF: log = "[KTX] Error file unexpected eof"; break;
		case KTX_FILE_WRITE_ERROR: log = "[KTX] Error file write error"; break;
		case KTX_GL_ERROR: log = "[KTX] Error gl error"; break;
		case KTX_INVALID_OPERATION: log = "[KTX] Error Invalid Operation"; break;
		case KTX_INVALID_VALUE: log = "[KTX] Error Invliad value"; break;
		case KTX_NOT_FOUND: log = "[KTX] Error KTX not found"; break;
		case KTX_OUT_OF_MEMORY: log = "[KTX] Error Out fo memory"; break;
		case KTX_TRANSCODE_FAILED: log = "[KTX] Error transcode failed"; break;
		case KTX_UNKNOWN_FILE_FORMAT: log = "[KTX] Error file unknown file format"; break;
		case KTX_UNSUPPORTED_TEXTURE_TYPE: log = "[KTX] Error unsupported texture type"; break;
		case KTX_UNSUPPORTED_FEATURE: log = "[KTX] Error unsupported feature"; break;
		case KTX_LIBRARY_NOT_LINKED: log = "[KTX] Error Library not linked"; break;
		default: return;
	}

	logDebug(fmt::format("{} | At line {} in {}", log, line, file));
}
}    // namespace neko::vk
#endif