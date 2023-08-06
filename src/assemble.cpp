#include "assemble.h"


// Update Trinity output transcript headers
void updateHeaders(std::string fastaFilePath, uintmax_t ram_b) {
  uintmax_t numBytesFasta = fs::file_size(fastaFilePath.c_str());
  uintmax_t lenHashTable = numBytesFasta / 160;

  seqHash fastaHashTable(lenHashTable, fastaFilePath, ram_b);

  //std::vector<sequence> * hashData = fastaHashTable.getHashData();
  linkedList * hashData = fastaHashTable.getHashData();

  std::string newPrefix(fs::path(fastaFilePath.c_str()).stem().stem().c_str());
  std::string currOldHeader;
  std::string currNewHeader;
  sequence currSeq;
  for (uintmax_t i = 0; i < lenHashTable; i++) {
    if (hashData[i].empty()) {
      continue;
    }
    else {
      hashData[i].updateSeqHead(newPrefix);
    }
  }
  fastaHashTable.dump(fastaFilePath);
}

// Given a FASTA input, divide all SRA reads into those which map to it and those that do not.
// Create two new files for containing the two.
void isolateReads(const std::vector<SRA> & sras, std::string fastaInput, std::string threads,
                  std::string ram_gb, bool dispOutput, const INI_MAP & cfgIni, std::string logFile) {

  logOutput("\nStarting isolation of reads of interest", logFile);
  INI_MAP_ENTRY cfgPipeline = cfgIni.at("Pipeline");
  std::ifstream unmappedReadFile;
  std::string fastaIndex;
  transcript dummyTrans;

  // Create index of fastaInput with Salmon
  transcript transTemp(fastaInput, cfgIni);
  // Check if indexing checkpoint exists
  fastaIndex = std::string(transTemp.get_trans_path_trinity().parent_path().c_str()) + "/" +
               std::string(fs::path(fastaInput.c_str()).stem().c_str()) + "_index";
  
  if (!transTemp.checkpointExists("idx.iso")) {
    salmon_index(fastaInput, fastaIndex, threads, dispOutput, logFile);
    transTemp.makeCheckpoint("idx.iso");
  }

  uintmax_t ram_b = (uintmax_t)stoi(ram_gb) * 1000000000;
  std::pair<std::string, std::string> currSraIn;
  std::string outDir;
  std::string fastaQuant;
  std::string filePrefix1;
  std::string filePrefix2;
  // Iterate through SRAs, copying their mapped and unmapped reads into separate files
  for (auto sra : sras) {
    if (sra.checkpointExists(std::string(fs::path(fastaInput.c_str()).stem().c_str()) + ".iso")) {
      logOutput("  Read isolation checkpoint found for: " + sra.get_accession() +
                " with " + fastaInput, logFile);
      continue;
    }
    std::vector<std::pair<std::string, std::string>> sraRunsIn;
    logOutput("  Now isolating reads for:", logFile);
    summarize_sing_sra(sra, logFile, 4);
    currSraIn.first = sra.get_sra_path_orep_filt().first.c_str();
    currSraIn.second = sra.get_sra_path_orep_filt().second.c_str();
    outDir = sra.get_sra_path_orep_filt().first.parent_path().c_str();

    // Obtain read input files, based on pipeline preferences
    if (!ini_get_bool(cfgPipeline.at("remove_overrepresented").c_str(), 0)) {
      if (!ini_get_bool(cfgPipeline.at("filter_foreign_reads").c_str(), 0)) {
        if (!ini_get_bool(cfgPipeline.at("trim_adapter_seqs").c_str(), 0)) {
          if (!ini_get_bool(cfgPipeline.at("error_correction").c_str(), 0)) {
            currSraIn.first = sra.get_sra_path_raw().first.c_str();
            currSraIn.second = sra.get_sra_path_raw().second.c_str();
            outDir = sra.get_sra_path_raw().first.parent_path().c_str();
          }
          else {
            currSraIn.first = sra.get_sra_path_corr_fix().first.c_str();
            currSraIn.second = sra.get_sra_path_corr_fix().second.c_str();
            outDir = sra.get_sra_path_corr_fix().first.parent_path().c_str();
          }
        }
        else {
          currSraIn.first = sra.get_sra_path_trim_p().first.c_str();
          currSraIn.second = sra.get_sra_path_trim_p().second.c_str();
          outDir = sra.get_sra_path_trim_p().first.parent_path().c_str();
        }
      }
      else {
        currSraIn.first = sra.get_sra_path_for_filt().first.c_str();
        currSraIn.second = sra.get_sra_path_for_filt().second.c_str();
        outDir = sra.get_sra_path_for_filt().first.parent_path().c_str();
      }
    }
    filePrefix1 = fs::path(currSraIn.first).filename().c_str();
    filePrefix2 = fs::path(currSraIn.second).filename().c_str();
    while (!fs::path(filePrefix1).extension().empty()) {
      filePrefix1 = fs::path(filePrefix1).stem().c_str();
    }
    while (!fs::path(filePrefix2).extension().empty()) {
      filePrefix2 = fs::path(filePrefix2).stem().c_str();
    }
    outDir = fs::path(currSraIn.first).parent_path().c_str();
    sraRunsIn.push_back(currSraIn);
    std::string sraPrefix(sra.get_file_prefix().first);

          
    // Quantify / map SRA reads against fastaInput index    
    if (sra.is_paired()) {
      fastaQuant = std::string(transTemp.get_trans_path_trinity().parent_path().c_str()) + "/" +
                   std::string(fs::path(fastaInput.c_str()).stem().c_str()) + "_" + 
                   std::string(sraPrefix.substr(0, sraPrefix.find_last_of("_"))) + "_quant";
    }
    else {
      fastaQuant = std::string(transTemp.get_trans_path_trinity().parent_path().c_str()) + "/" +
                   std::string(fs::path(fastaInput.c_str()).stem().c_str()) + "_" +
                   std::string(sraPrefix) + "_quant";
    }
    // Check if quant checkpoint exists
    if (!sra.checkpointExists(std::string(fs::path(fastaInput.c_str()).stem().c_str()) + ".qt.iso")) {
      logOutput("  Mapping reads to sequences of interest", logFile); 
      salmon_quant(fastaInput, fastaIndex, fastaQuant, sraRunsIn, threads, dispOutput, logFile);
      // Create checkpoint for salmon quant of SRA against seqs of interest
      sra.makeCheckpoint(std::string(fs::path(fastaInput.c_str()).stem().c_str()) + ".qt.iso");
    }
 
    // Parse Salmon output file with names of unmapped reads
    // Use sequence hash tables to quickly separate mapped from unmapped 
    uintmax_t numBytesReads1;
    uintmax_t lenReadsHash1;
    uintmax_t numBytesReads2;
    uintmax_t lenReadsHash2;
    
    std::string currLine;
    sequence currSeq;
    std::string currHead;
    std::string currSeqData;
    std::string currQual;

    if (true) {
      numBytesReads1 = fs::file_size(currSraIn.first.c_str());
      lenReadsHash1 = numBytesReads1 / 160;
      logOutput("  Creating hash table from forward-ended reads", logFile);
      seqHash readHashTable1(lenReadsHash1, fs::path(currSraIn.first.c_str()), ram_b);
      seqHash unmappedHash1(lenReadsHash1);

      logOutput("  Now splitting reads into mapped and unmapped", logFile);
      unmappedReadFile.open(fastaQuant + "/aux_info/unmapped_names.txt");

      // Iterate through headers in unmapped reads file
      // Fill hash tables accordingly
      while (getline(unmappedReadFile, currLine)) {
        currHead = currLine.substr(0, currLine.find(" "));
        currSeq = readHashTable1.getSeq(currHead);
        currHead = currSeq.get_header();

        currSeqData = currSeq.get_sequence();
        currQual = currSeq.get_quality();
      
        readHashTable1.deleteHash(currHead);
        unmappedHash1.insertHash(currHead, currSeqData, currQual);
      }
      // Dump filled sequence hash tables to new files, containing the mapped
      // and unmapped reads respectively
      logOutput("  Dumping split reads to mapped and unmapped files", logFile);
      readHashTable1.dump(outDir + "/" + filePrefix1 + ".mapped.fq");
      unmappedHash1.dump(outDir + "/" + filePrefix1 + ".unmapped.fq");
      logOutput("", logFile);
      unmappedReadFile.close();
    }

    if (sra.is_paired()) {
      numBytesReads2 = fs::file_size(currSraIn.second.c_str());
      lenReadsHash2 = numBytesReads2 / 160;
      logOutput("  Creating hash table from reverse-ended reads", logFile);

      seqHash readHashTable2(lenReadsHash2, fs::path(currSraIn.second.c_str()), ram_b);
      seqHash unmappedHash2(lenReadsHash2);

      logOutput("  Now splitting reads into mapped and unmapped", logFile);
      unmappedReadFile.open(fastaQuant + "/aux_info/unmapped_names.txt");
      // Iterate through headers in unmapped reads file
      // Fill hash tables accordingly
      while (getline(unmappedReadFile, currLine)) {
        currHead = currLine.substr(0, currLine.find(" "));
        currSeq = readHashTable2.getSeq(currHead);
        currHead = currSeq.get_header();

        currSeqData = currSeq.get_sequence();
        currQual = currSeq.get_quality();

        readHashTable2.deleteHash(currHead);
        unmappedHash2.insertHash(currHead, currSeqData, currQual);
      }
      logOutput("  Dumping split reads to mapped and unmapped files", logFile);
      readHashTable2.dump(outDir + "/" + filePrefix2 + ".mapped.fq");
      unmappedHash2.dump(outDir + "/" + filePrefix2 + ".unmapped.fq");
      logOutput("", logFile);
      unmappedReadFile.close();
    }
    sra.makeCheckpoint(std::string(fs::path(fastaInput.c_str()).stem().c_str()) + ".iso");    
  }
}

// Utility function. Given vector of SRA objects, return vector of transcript
// objects constructed off of them
std::vector<transcript> get_transcript(std::vector<SRA> sras) {
  std::vector<transcript> transcripts;
  for (auto &sra : sras) {
    transcript currTrans(sra);
    transcripts.push_back(currTrans);
  }
  return transcripts;
}

// Utility function. Given a "true" / "false" string, return the corresponding
// boolean
bool stringToBool(std::string boolStr) {
  bool boolConv;
  for (int i = 0; i < boolStr.length(); i++) {
    boolStr[i] = std::tolower(boolStr[i]);
  }
  boolConv = (boolStr == "true") ? true : false;
  return boolConv;
}

// Create a checkpoint for a given assembly group
void makeGroupCheckpoint(std::string cpDir, std::string prefix) {
  std::string cpFileName = cpDir + "/" + prefix + ".trinity.ok";
  std::ofstream cpFile;
  cpFile.open(cpFileName);
  cpFile.close();
}

// Determine whether a group checkpoint file exists
bool groupCheckpointExists(std::string cpDir, std::string prefix) {
  std::string cpFileName = cpDir + "/" + prefix + ".trinity.ok";
  fs::path cpFilePath(cpFileName.c_str());
  if (fs::exists(cpFilePath)) {
    return true;
  }
  else {
    return false;
  }
}

// Create a transcript info file containing the read files used in its assembly
void makeTransInfoFile(const std::vector<std::pair<std::string, std::string>> & sraRuns, std::string transInfoFileStr) {
  std::ofstream transInfoFile;
  transInfoFile.open(transInfoFileStr);
  for (auto sra : sraRuns) {
    transInfoFile << sra.first;
    if (sra.second != "") {
      transInfoFile << " " << sra.second;
    }
    transInfoFile << std::endl;
  }
  transInfoFile.close();
}

// Perform a bulk assembly for SRAs and groups of SRAs
void run_trinity_bulk(std::map<std::string, std::vector<SRA>> sraGroups,
                      std::string threads, std::string ram_gb,
                      bool assembSeqsInterest, bool assembSeqsNoInterest,
                      bool assembAllSeqs, std::string fastaSeqsFile,
                      bool dispOutput, bool retainInterFiles,
                      std::string logFile, const INI_MAP & cfgIni) {
  logOutput("\nStarting de-novo assembly (this may take awhile)", logFile);
  INI_MAP_ENTRY cfgPipeline = cfgIni.at("Pipeline");
  INI_MAP_ENTRY assembGroups = cfgIni.at("Assembly groups");
  std::string outDir;
  std::vector<std::pair<std::string, std::string>> sraRunsInTrin;
  std::vector<std::pair<std::string, std::string>> sraRunsInterest;
  std::vector<std::pair<std::string, std::string>> sraRunsNoInterest;
  std::pair<std::string, std::string> currTrinIn;
  std::string currTrinOutAll;
  std::string currTrinOutInt;
  std::string currTrinOutNon;
  fs::path cpDir;
  fs::path trinDir;
  std::string transInfoFileStr;
  uintmax_t ram_b = (uintmax_t)stoi(ram_gb) * 1000000000;

  for (auto sraGroup : sraGroups) {
    SRA dummySra = sraGroup.second.at(0);
    cpDir = dummySra.get_fastqc_dir_2().first.parent_path().parent_path().parent_path() / "checkpoints";

    // Check whether checkpoint exists
    /*
    if (groupCheckpointExists(std::string(cpDir.c_str()), sraGroup.first)) {
      logOutput("Assembly found for group: " + sraGroup.first, logFile);
      logOutput("  Containing:", logFile);
      summarize_all_sras(sraGroup.second, logFile, 4);
      continue;
    }*/

    transcript dummyTrans(dummySra);
    fs::path pathTrinDir = dummyTrans.get_trans_path_trinity().parent_path();
    
    outDir = std::string(pathTrinDir.c_str());
    currTrinOutAll = outDir + "/" + sraGroup.first + ".Trinity.fasta";
    currTrinOutInt = outDir + "/" + sraGroup.first + ".mapped.Trinity.fasta";
    currTrinOutNon = outDir + "/" + sraGroup.first + ".unmapped.Trinity.fasta";
  
    std::string currFilePrefix1;
    std::string currFilePrefix2;
    for (auto sra : sraGroup.second) {
      currFilePrefix1 = sra.get_sra_path_orep_filt().first.filename().stem().stem().stem().c_str();
      currFilePrefix2 = sra.get_sra_path_orep_filt().second.filename().stem().stem().stem().c_str();
      currTrinIn.first = sra.get_sra_path_orep_filt().first.c_str();
      currTrinIn.second = sra.get_sra_path_orep_filt().second.c_str();
      if (assembAllSeqs) {
        if (!ini_get_bool(cfgPipeline.at("remove_overrepresented").c_str(), 0)) {
          if (!ini_get_bool(cfgPipeline.at("filter_foreign_reads").c_str(), 0)) {
            if (!ini_get_bool(cfgPipeline.at("trim_adapter_seqs").c_str(), 0)) {
              if (!ini_get_bool(cfgPipeline.at("error_correction").c_str(), 0)) {
                currTrinIn.first = sra.get_sra_path_raw().first.c_str();
                currTrinIn.second = sra.get_sra_path_raw().second.c_str();
              }
              else {
                currTrinIn.first = sra.get_sra_path_corr_fix().first.c_str();
                currTrinIn.second = sra.get_sra_path_corr_fix().second.c_str();
              }
            }
            else {
              currTrinIn.first = sra.get_sra_path_trim_p().first.c_str();
              currTrinIn.second = sra.get_sra_path_trim_p().second.c_str();
            }
          }
          else {
            currTrinIn.first = sra.get_sra_path_for_filt().first.c_str();
            currTrinIn.second = sra.get_sra_path_for_filt().second.c_str();
          }
        }
        sraRunsInTrin.push_back(currTrinIn);
        outDir = fs::path(currTrinIn.first).parent_path().c_str();
      }
      if (assembSeqsInterest) {
        currTrinIn.first = outDir + "/" + currFilePrefix1 + ".mapped.fq";
        if (sra.is_paired()) {
          currTrinIn.second = outDir + "/" + currFilePrefix2 + ".mapped.fq";
        }
        sraRunsInterest.push_back(currTrinIn);
      }
      if (assembSeqsNoInterest) {
        currTrinIn.first = outDir + "/" + currFilePrefix1 + ".unmapped.fq";
        if (sra.is_paired()) {
          currTrinIn.second = outDir + "/" + currFilePrefix2 + ".unmapped.fq";
        }
        sraRunsNoInterest.push_back(currTrinIn);
      }
    }
    // TODO: fix updating of headers in Trinity output

    // Perform assembly for reads of interest (those that map to provided FASTA)
    if (assembSeqsInterest) {
      logOutput("  Now assembling reads that map to:\n  " + fastaSeqsFile, logFile);
      if (!groupCheckpointExists(std::string(cpDir.c_str()), sraGroup.first + ".mapped")) {
        if (sraRunsInterest.size() > 1) {
          run_trinity_comb(sraRunsInterest, currTrinOutInt, threads, ram_gb, dispOutput, logFile);
        }
        else {
          run_trinity(sraRunsInterest.at(0), currTrinOutInt, threads, ram_gb, dispOutput, logFile);
        }
        // Make file for mapped assembly containing associated SRAs
        transInfoFileStr = std::string(fs::path(currTrinOutInt.c_str()).replace_extension(".ti").c_str());
        makeTransInfoFile(sraRunsInterest, transInfoFileStr);
  
        makeGroupCheckpoint(std::string(cpDir.c_str()), sraGroup.first + ".mapped");
        //updateHeaders(currTrinOutInt, ram_b);
      }
      else {
        logOutput("    Mapped assembly checkpoint found for: " + sraGroup.first, logFile);
      }
      sraRunsInterest.clear();
    }
    // Perform assembly for reads of no interest (those that DO NOT map to provided FASTA)
    if (assembSeqsNoInterest) {
      logOutput("  Now assembling reads that do not map to:\n  " + fastaSeqsFile, logFile);
      if (!groupCheckpointExists(std::string(cpDir.c_str()), sraGroup.first + ".unmapped")) {
        if (sraRunsNoInterest.size() > 1) {
          run_trinity_comb(sraRunsNoInterest, currTrinOutNon, threads, ram_gb, dispOutput, logFile);
        }
        else {
          run_trinity(sraRunsNoInterest.at(0), currTrinOutNon, threads, ram_gb, dispOutput, logFile);
        }
        // Make file for unmapped assembly containing associated SRAs
        transInfoFileStr = std::string(fs::path(currTrinOutNon.c_str()).replace_extension(".ti").c_str());
        makeTransInfoFile(sraRunsNoInterest, transInfoFileStr);

        makeGroupCheckpoint(std::string(cpDir.c_str()), sraGroup.first + ".unmapped");
        //updateHeaders(currTrinOutNon, ram_b);
      }
      else {
        logOutput("    Unmapped assembly checkpoint found for: " + sraGroup.first, logFile);
      }
      sraRunsNoInterest.clear();
    }
    // Perform assembly for all reads
    if (assembAllSeqs) {
      logOutput("  Now assembling all reads", logFile);
      if (!groupCheckpointExists(std::string(cpDir.c_str()), sraGroup.first)) {
        if (sraRunsInTrin.size() > 1) {
          run_trinity_comb(sraRunsInTrin, currTrinOutAll, threads, ram_gb, dispOutput, logFile);
        }
        else {
          run_trinity(sraRunsInTrin.at(0), currTrinOutAll, threads, ram_gb, dispOutput, logFile);
        }
        // Make file for entire assembly containing associated SRAs
        transInfoFileStr = std::string(fs::path(currTrinOutAll.c_str()).replace_extension(".ti").c_str());
        makeTransInfoFile(sraRunsInTrin, transInfoFileStr);

        makeGroupCheckpoint(std::string(cpDir.c_str()), sraGroup.first);
        //updateHeaders(currTrinOutAll, ram_b);
      }
      else {
        logOutput("    Global assembly checkpoint found for: " + sraGroup.first, logFile);
      }
      sraRunsInTrin.clear();
    }
  }
}


void print_help() {
  std::cout << "\n" << "NAME_OF_PROGRAM" << " - "
            << "A tool for bulk assemblies of de novo transcriptome data" << std::endl;
  std::cout << "\n" << "COMMAND STRUCTURE" << std::endl;
  std::cout << "\n" << "assemble PATH/TO/CONFIG.INI num_threads RAM_GB (--mult)" << std::endl;
}

int main(int argc, char * argv[]) {
  system("setterm -cursor off");
  if (argc > 1) {
    std::vector<SRA> sras;
    std::vector<std::string> localDataFiles;
    // Get INI config file
    INI_MAP cfgIni = make_ini_map(argv[1]);
    INI_MAP_ENTRY cfgIniGen = cfgIni["General"];

    // Define log file
    std::string logFilePath((fs::canonical((fs::path(cfgIniGen["output_directory"].c_str()))) /
                             fs::path(cfgIniGen["project_name"].c_str()) /
                             fs::path(cfgIniGen["log_file"].c_str())).c_str());


    // Get number of threads
    std::string threads = argv[2];

    // Get RAM in GB
    std::string ram_gb = argv[3];

    // Get boolean for multiple sra processing
    // bool mult_sra = stringToBool(argv[4]);

    // Get boolean for intermediate file fate
    bool retainInterFiles = stringToBool(argv[5]);
    
    // Get boolean for verbose printing
    bool dispOutput = stringToBool(argv[6]);
  
    // Get sequences of interest from config
    bool selectiveAssembly = false;
    bool assembleInterest = ini_get_bool(cfgIniGen["assemble_seqs_of_interest"].c_str(), 0);
    bool assembleOthers = ini_get_bool(cfgIniGen["assemble_other_seqs"].c_str(), 0);
    bool assembleAllSeqs = ini_get_bool(cfgIniGen["assemble_all_seqs"].c_str(), 0);

    const char * home = std::getenv("HOME");
    std::string fastaSeqs = cfgIniGen["seqs_of_interest"];
    if (fastaSeqs[0] == '~') {
      fastaSeqs = std::string(home) + fastaSeqs.substr(1, fastaSeqs.size() - 1);
    }

    if (!fastaSeqs.empty() && fs::exists(fastaSeqs.c_str())) {
      selectiveAssembly = true;
    }
    else {
      if (fastaSeqs.empty()) {
        logOutput("User did not define sequences of interest. Skipping selective assembly.",
                  logFilePath);
      }
      else if (!fs::exists(fastaSeqs.c_str())) {
        logOutput("ERROR:\n  file \"" + fastaSeqs + "\" not found.\n  Exiting.", logFilePath);
        exit(1);
      }
      assembleInterest = false;
      assembleOthers = false;
    }
    
    // If all assembly options are disabled, exit
    if (!assembleInterest && !assembleOthers && !assembleAllSeqs) {
      logOutput("No assembly option specified. Have a nice day!", logFilePath);
      exit(1);
    }
    
    bool compressFiles = false;
    
    // Create file space
    make_proj_space(cfgIni, "assemble");

    // Obtain SRAs
    sras = get_sras(cfgIni, compressFiles);
    
    for (auto fqFileName : cfgIni.at("Local files")) {
      localDataFiles.push_back(fqFileName.first);
    }
    std::pair<std::string, std::string> sraRunsLocal;
    size_t pos;
    for (auto sraRun : localDataFiles) {
      sraRunsLocal.first = "";
      sraRunsLocal.second = "";
      pos = sraRun.find(" ");
      sraRunsLocal.first = sraRun.substr(0, pos);
      if (pos != std::string::npos) {
        sraRun.erase(0, pos + 1);
        pos = sraRun.find(" ");
        sraRunsLocal.second = sraRun.substr(0, pos);
      }
      if (fs::exists(cfgIni["General"]["local_data_directory"] + sraRunsLocal.first) &&
          fs::exists(cfgIni["General"]["local_data_directory"] + sraRunsLocal.second)) {
        sras.push_back(SRA(sraRunsLocal.first, sraRunsLocal.second, cfgIni, compressFiles));
      }
      else {
        if (sraRunsLocal.first != "" &&
            !fs::exists(cfgIni["General"]["local_data_directory"] + sraRunsLocal.first)) {
          logOutput("ERROR: Local run not found: \"" + sraRunsLocal.first + "\"", logFilePath);
        }
        if (sraRunsLocal.second != "" &&
            !fs::exists(cfgIni["General"]["local_data_directory"] + sraRunsLocal.second)) {
          logOutput("ERROR: Local run not found: \"" + sraRunsLocal.second + "\"", logFilePath);
        }
      }
    }

    // Check if no SRAs specified
    if (sras.empty()) {
      std::cout << "ERROR: No SRA runs specified. Please check config file" << std::endl;
    }

    // Get group specifications for SRAs
    std::map<std::string, std::vector<SRA>> sraGroups;
    INI_MAP_ENTRY assemblyGroups = cfgIni["Assembly groups"];
    std::string currGroupName;
    std::string currIniArrStr;
    std::vector<std::string> iniStrArray;
    std::vector<SRA> currSraGroup;
    // Iterate through user-defined assembly groups in config file
    if (assemblyGroups.empty()) {
      for (auto sra : sras) {
        currSraGroup.push_back(sra);
        sraGroups.emplace(sra.get_file_prefix().first.substr(0, 
                          sra.get_file_prefix().first.find_last_of("_")), currSraGroup);
        currSraGroup.clear();
      }
    }
    for (auto assemblyGroup : assemblyGroups) {
      // Get group name and group array string 
      currGroupName = assemblyGroup.first;
      currIniArrStr = assemblyGroup.second;

      // Remove all whitespace from string
      currIniArrStr.erase(remove_if(currIniArrStr.begin(), currIniArrStr.end(), isspace),
                          currIniArrStr.end());
      
      // Tokenize array string into vector of strings
      iniStrArray = getStrArray(currIniArrStr, ",");

      // Match vector's strings with SRAs, filling SRA group vector
      bool sraChosen;
      std::vector<bool> iniEntryFound(iniStrArray.size(), false);
      for (auto sra : sras) {
        sraChosen = false;
        for (int i = 0; i < iniStrArray.size(); i++) {
          // If SRA specified in group, push to group vector
          if (iniStrArray[i] == sra.get_accession() ||
              iniStrArray[i] == sra.get_file_prefix().first ||
              iniStrArray[i] == sra.get_file_prefix().second ||
              iniStrArray[i] == sra.get_file_prefix().first + ".fastq" ||
              iniStrArray[i] == sra.get_file_prefix().second + ".fastq") {
            iniEntryFound[i] = true;
            if (!sraChosen) {
              currSraGroup.push_back(sra);
              sraChosen = true;
            }
          }
        }
        if (!sraChosen) {
          sraGroups.emplace(sra.get_file_prefix().first,
                            std::vector<SRA>{sra});
        }
      }
      for (int i = 0; i < iniEntryFound.size(); i++) {
        if (iniEntryFound[i] == false) {
          logOutput("ERROR:\n  Entry \"" + iniStrArray[i] + "\" in group \"" +
                    currGroupName + "\" not found.\n  Proceeding without it.\n",
                    logFilePath);
        }
      }
      
      // Emplace current SRA group into map
      sraGroups.emplace(currGroupName, currSraGroup);
      currSraGroup.clear();
    }

    logOutput("Semblans Assemble started with following parameters:", logFilePath);
    logOutput("  Config file:     " + std::string(argv[1]), logFilePath);;
    logOutput("  Threads (Cores): " + threads, logFilePath);
    logOutput("  Memory (GB):     " + ram_gb, logFilePath);
    logOutput("  SRA Runs:\n", logFilePath);
    summarize_all_sras(sras, logFilePath, 6);
    // Separate out reads of interest
    if (selectiveAssembly) {
      isolateReads(sras, fastaSeqs, threads, ram_gb, dispOutput,
                   cfgIni, logFilePath);
    }
    // Perform assembly with Trinity
    run_trinity_bulk(sraGroups, threads, ram_gb, assembleInterest, 
                     assembleOthers, assembleAllSeqs, fastaSeqs, dispOutput,
                     retainInterFiles, logFilePath, cfgIni);
  }
  else {
    print_help();
  }
  system("setterm -cursor on");
  return 0;
}
