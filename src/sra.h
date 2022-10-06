#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include "ini_parse.h"

namespace fs = boost::filesystem;

// SRA class definition
// SRA run information is collated into single object
class SRA {
  private:
    std::string sra_accession;
    std::string org_name;
    std::string tax_id;
    int spots;
    int spots_m;
    long int bp;
    bool paired;
    fs::path fastqc_dir_1;
    fs::path fastqc_dir_2;
    fs::path sra_path_raw_1;
    fs::path sra_path_raw_2;
    fs::path sra_path_corr_1;
    fs::path sra_path_corr_2;
    fs::path sra_path_trim_1;
    fs::path sra_path_trim_2;
    fs::path sra_path_filt_1;
    fs::path sra_path_filt_2;
  public:
    SRA();
    SRA(std::string sra_accession, INI_MAP cfgIni);
    std::string get_accession();
    std::string get_org_name();
    std::string get_tax_id();
    int get_spots();
    int get_spots_m();
    long int get_bp();
    bool is_paired();
    std::pair<fs::path, fs::path> get_fastqc_dir();
    std::pair<fs::path, fs::path> get_sra_path_raw();
    std::pair<fs::path, fs::path> get_sra_path_corr();
    std::pair<fs::path, fs::path> get_sra_path_trim();
    std::pair<fs::path, fs::path> get_sra_path_filt();
    std::string make_file_str();
};
