
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>

#include "ConfigHandler.hpp"
#include "Gui.hpp"
#include "NFDFileHandler.hpp"
#include "PlotHandler.hpp"
#include "gitversion.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#if defined(unix) || defined(__unix__) || defined(__unix)
#define _UNIX
#endif

bool done = false;
std::mutex mtx;

int main(int ac, char** av)
{
#ifdef _UNIX
	std::string logDirectory = std::string(std::getenv("HOME")) + "/STMViewer/logs/logfile.txt";
#elif _WIN32
	std::string logDirectory = std::string(std::getenv("APPDATA")) + "/STMViewer/logs/logfile.txt";
#else
#error "Your system is not supported!"
#endif

	spdlog::sinks_init_list sinkList = {std::make_shared<spdlog::sinks::stdout_color_sink_st>(),
										std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory, true)};
	std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("logger", sinkList.begin(), sinkList.end());

	logger->info("Starting STMViewer!");
	logger->info("Version: {}.{}.{}", STMVIEWER_VERSION_MAJOR, STMVIEWER_VERSION_MINOR, STMVIEWER_VERSION_REVISION);
	logger->info("Commit hash {}", GIT_HASH);

	PlotHandler plotHandler(done, &mtx, logger);
	ConfigHandler configHandler("", &plotHandler, logger);
	NFDFileHandler fileHandler;
	Gui gui(&plotHandler, &configHandler, &fileHandler, done, &mtx, logger);

	while (!done)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	logger->info("Closing STMViewer!");
	spdlog::shutdown();
	return 0;
}
