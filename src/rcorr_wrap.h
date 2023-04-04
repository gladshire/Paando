#pragma once
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <fstream>
#include "sra.h"
#include "print_info.h"

namespace fs = boost::filesystem;
namespace dl = boost::dll;

#define PATH_RCORR std::string((dl::program_location().parent_path() / fs::path(std::string("../external/Rcorrector-1.0.5/run_rcorrector.pl"))).c_str())

void run_rcorr(std::vector<std::pair<std::string, std::string>> sraRunFiles,
               std::string outDir, std::string threads, bool dispOutput,
               std::string logFile);
