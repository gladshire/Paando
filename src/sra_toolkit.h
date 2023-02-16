#pragma once
#include "sra.h"
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

namespace dl = boost::dll;
namespace fs = boost::filesystem;

#define PATH_SRA_BIN std::string((dl::program_location().parent_path() / fs::path(std::string("../external/sratoolkit.3.0.1/bin/"))).c_str())
#define PATH_PREFETCH std::string(PATH_SRA_BIN + "prefetch")
#define PATH_FASTERQ std::string(PATH_SRA_BIN + "fasterq-dump")

std::vector<SRA> get_sras(const INI_MAP & iniFile);

void prefetch_sra(std::vector<SRA> sras);

void fasterq_sra(std::vector<SRA> sras, std::string threads);

void align_file_buffer(std::ifstream & inFile1, std::ifstream & inFile2,
                       char * inFile1Data, char * inFile2Data,
                       std::streamsize & s1, std::streamsize & s2);
