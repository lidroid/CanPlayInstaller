#include <sciter-x-window.hpp>
#include <sciter-x-threads.h>
#include <sciter-x-api.h>

#include <easylogging++.h>

#include "resources.cpp"
#include "utils.h"

static sciter::value fs_api() {
	sciter::value api_map;

	api_map.set_item("exeFileName", sciter::vfunc(fs_exeFileName));
	api_map.set_item("exists", sciter::vfunc(fs_exists));
	api_map.set_item("createDirectories", sciter::vfunc(fs_createDirectories));
	api_map.set_item("copyFile", sciter::vfunc(fs_copyFile));
	api_map.set_item("remove", sciter::vfunc(fs_remove));
	api_map.set_item("rename", sciter::vfunc(fs_rename));
	api_map.set_item("fileSize", sciter::vfunc(fs_fileSize));
	return api_map;
}

static sciter::value rsa_api() {
	sciter::value api_map;

	api_map.set_item("encrypt", sciter::vfunc(rsa_encrypt));
	api_map.set_item("decrypt", sciter::vfunc(rsa_decrypt));

	return api_map;
}

static sciter::value aes_api() {
	sciter::value api_map;

	api_map.set_item("encrypt", sciter::vfunc(aes_encrypt));
	api_map.set_item("decrypt", sciter::vfunc(aes_decrypt));

	return api_map;
}

static sciter::value native_log(sciter::value type, sciter::value log)
{
	std::string type_str = w2utf(type.to_string());
	std::string log_str = w2utf(log.to_string());
	if (type_str == "warning") {
		LOG(WARNING) << log_str;
	} else if (type_str == "info") {
		LOG(INFO) << log_str;
	} else if (type_str == "error") {
		LOG(ERROR) << log_str;
	} else {
		LOG(DEBUG) << log_str;
	}

	return sciter::value();
}

class frame : public sciter::window
{
public:
	frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

BEGIN_FUNCTION_MAP
	FUNCTION_0("fs", fs_api);
	FUNCTION_0("rsa", rsa_api);
	FUNCTION_0("aes", aes_api);
	FUNCTION_1("md5", md5sum);
	FUNCTION_2("log", native_log);
END_FUNCTION_MAP
};

INITIALIZE_EASYLOGGINGPP

int uimain(std::function<int()> run)
{
#if defined(WINDOWS) && !defined(NDEBUG)
	sciter::debug_output_console _console;
#endif

	frame *pwin = new frame();

#if defined(WINDOWS)
	sciter::sync::gui_thread_ctx _ctx;
#endif

	const std::vector<sciter::string> &argv = sciter::application::argv();
	sciter::string str = WSTR("this://app/default.html");
	if (argv.size() > 1)
		str = argv[1];
	else
		sciter::archive::instance().open(aux::elements_of(resources));

	if (pwin->load(str.c_str())) pwin->expand();

	auto result = run();

	return result;
}
