#include "fastqc_wrap.h"

/*void run_fastqc(std::vector<SRA> sras, std::string threads, std::string outDir) {
  std::cout << "\nRunning quality analysis for:\n" << std::endl;
  summarize_all_sras(sras);
  std::string outFile;
  std::string inFile1;
  std::string inFile2;
  std::string fastqcFlags = " --extract -t " + threads + " -o ";
  for (auto sra : sras) {
    outFile = outDir + "/" + sra.make_file_str();
    if (fs::exists(fs::path(outFile.c_str()))) {
      std::cout << "FastQC analysis found for: " << sra.get_accession() << std::endl;
      continue;
    }
    system(("mkdir " + outFile).c_str());
    if (sra.is_paired()) {
      inFile1 = sra.get_sra_path_raw().first.c_str();
      inFile2 = sra.get_sra_path_raw().second.c_str();
      system((PATH_FASTQC + fastqcFlags + outFile + " " +
              inFile1 + " " + inFile2).c_str());
    }
    else {
      inFile1 = sra.get_sra_path_raw().first.c_str();
      system((PATH_FASTQC + fastqcFlags + outFile + " " + inFile1).c_str());
    }
  } 
}*/

void run_fastqc(SRA sra, std::string threads, std::string outDir) {
  std::string inFile1;
  std::string inFile2;
  std::string outFile = outDir + "/" + sra.make_file_str();
  std::string fastqcFlags = " --extract -t " + threads + " -o ";
  if (fs::exists(fs::path(outFile.c_str()))) {
    std::cout << "FastQC analysis found for: " << sra.get_accession() << std::endl;
    return;
  }
  system(("mkdir " + outFile).c_str());
  if (sra.is_paired()) {
    // This is not good. Fix at some point.
    if (outDir == std::string(sra.get_fastqc_dir_2().first.parent_path().parent_path().c_str())) {
      inFile1 = sra.get_sra_path_for_filt().first.c_str();
      inFile2 = sra.get_sra_path_for_filt().second.c_str();
    }
    else {
      inFile1 = sra.get_sra_path_raw().first.c_str();
      inFile2 = sra.get_sra_path_raw().second.c_str();
    }
    system((PATH_FASTQC + fastqcFlags + outFile + " " + inFile1 + " " + inFile2).c_str());
  }
  else {
    if (outDir == std::string(sra.get_fastqc_dir_2().first.parent_path().parent_path().c_str())) {
      inFile1 = sra.get_sra_path_for_filt().first.c_str();
    }
    else {
      inFile1 = sra.get_sra_path_raw().first.c_str();
    }
    system((PATH_FASTQC + fastqcFlags + outFile + " " + inFile1).c_str());
  }
}

void run_fastqc_bulk(std::vector<SRA> sras, std::string threads, std::string outDir) {
  std::cout << "\nRunning quality analysis for:\n" << std::endl;
  summarize_all_sras(sras);
  for (auto sra : sras) {
    run_fastqc(sra, threads, outDir);
  }
}
