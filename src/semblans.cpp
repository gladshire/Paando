#include "semblans.h"

std::atomic<bool> procRunning(false);

// Print ASCII startup screen for Semblans
void print_intro(std::string logFile) {
  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  logOutput("\n", logFile);
  logOutput("\n", logFile);
  logOutput("                            |     |                     \n", logFile);
  logOutput("      __|   _ \\  __ `__ \\   __ \\  |   _` |  __ \\    __| \n", logFile);
  logOutput("    \\__ \\   __/  |   |   |  |   | |  (   |  |   | \\__ \\ \n", logFile);
  logOutput("    ____/ \\___| _|  _|  _| _.__/ _| \\__,_| _|  _| ____/ \n", logFile); 
  logOutput("\n\n\n", logFile);
  printBreakLine(logFile, 6, 47);
}

// Print base help message for Semblans in terminal
void print_help_base() {
  std::cout << "USAGE:\n" << std::endl;
  std::cout << "  semblans [--help/-h] [COMMAND] [--config/-cfg]\n"
            << "           [--threads/-t] [--ram/-r] [--retain/-f]\n"
            << "           [--verbose/-v]\n" << std::endl;
  std::cout << "ARGUMENTS:\n" << std::endl;
  std::cout << "  [COMMAND]" << std::endl;
  std::cout << "    preprocess           Performs pre-assembly steps only" << std::endl;
  std::cout << "    assemble             Performs de novo assembly step only" << std::endl;
  std::cout << "    postprocess          Performs post-assembly steps only" << std::endl;
  std::cout << "    all (default)        Performs all steps in pipeline\n" << std::endl;
  std::cout << "  [MODE]" << std::endl;
  std::cout << "  -1/-2, --left/--right  Specifies simple mode." << std::endl;
  std::cout << "                         Path to left/right read FASTQ(s) should follow the corresponding flags." << std::endl;
  std::cout << "                         To specify multiple pairs of files, separate file paths with commas" << std::endl;
  std::cout << "  -od, --output          Specifies path to where Semblans should place output files." << std::endl;
  std::cout << "                         Directory must already exist" << std::endl;
  std::cout << "  -kdb, --kraken-db      Specifies which Kraken databases to use during removal of foreign reads." << std::endl;
  std::cout << "                         To specify multiple databases, separate their paths with commas." << std::endl;
  std::cout << "                         If omitted, Semblans will skip foreign read filtration" << std::endl;
  std::cout << "  -a, --assembly         Specifies an assembly for Semblans to process." << std::endl;
  std::cout << "                         Only used during postprocessing." << std::endl;
  std::cout << "  -rp, --ref-proteome    Specifies the reference proteome Semblans will use for" << std::endl;
  std::cout << "                         chimera detection and coding region prediction." << std::endl;
  std::cout << "                         Only used during postprocessing." << std::endl;
  std::cout << std::endl;
  std::cout << "  -cfg, --config         Specifies config file mode." << std::endl;
  std::cout << "                         Path to config file should follow this argument" << std::endl;
  std::cout << std::endl;
  std::cout << "  [FLAGS]" << std::endl;
  std::cout << "  -t,   --threads        Specifies number of threads/CPU cores to employ" << std::endl;
  std::cout << "  -r,   --ram            Specifies ammount of memory/RAM (GB) to dedicate" << std::endl;
  std::cout << "  -f,   --retain         Prevents deletion of intermediate files in pipeline" << std::endl;
  std::cout << "  -v,   --verbose        Prints all output from Semblans and sub-programs" << std::endl;
  std::cout << "  -h,   --help           Displays this help screen" << std::endl;

}


void print_help_preprocess() {
  std::cout << "USAGE:\n" << std::endl;
  std::cout << "  semblans preprocess [--help/-h] [--config/-cfg]\n"
            << "                      [--left/-l reads_1_left.fq,reads_2_left.fq,...]\n"
            << "                      [--right/-r reads_1_right.fq,reads_2_right.fq,...]\n"
            << "                      [--threads/-t] [--ram/-r] [--retain/-f]\n"
            << "                      [--verbose/-v]\n" << std::endl;
  std::cout << "ARGUMENTS:\n" << std::endl;
  std::cout << "  -1/-2, --left/--right  Specifies simple mode." << std::endl;
  std::cout << "                         Path to left/right read FASTQ(s) should follow the corresponding flags." << std::endl;
  std::cout << "                         To specify multiple pairs of files, separate file paths with commas" << std::endl;
  std::cout << "  -od,  --output         Specifies path to where Semblans should place output files." << std::endl;
  std::cout << "                         Directory must already exist" << std::endl;
  std::cout << "  -kdb, --kraken-db      Specifies which Kraken databases to use during removal of foreign reads." << std::endl;
  std::cout << "                         To specify multiple databases, separate their paths with commas." << std::endl;
  std::cout << "  -cfg, --config         Specifies config file mode." << std::endl;
  std::cout << "                         Path to config file should follow this argument" << std::endl;
  std::cout << "  -t,   --threads        Specifies number of threads/CPU cores to employ" << std::endl;
  std::cout << "  -r,   --ram            Specifies ammount of memory/RAM (GB) to dedicate" << std::endl;
  std::cout << "  -f,   --retain         Prevents deletion of intermediate files in pipeline" << std::endl;
  std::cout << "  -v,   --verbose        Prints all output from Semblans and sub-programs" << std::endl;
  std::cout << "  -h,   --help           Displays this help screen" << std::endl;
}

void print_help_postprocess() {
  std::cout << "USAGE:\n" << std::endl;
  std::cout << "  semblans postprocess [--help/-h] [--config/-cfg]\n"
            << "                       [--assembly/-a reads_assembly.fa]\n"
            << "                       [--left/-1 reads_1_left.fq,reads_2_left.fq,...]\n"
            << "                       [--right/-2 reads_1_right.fq,reads_2_right.fq,...]\n"
            << "                       [--threads/-t] [--ram/-r] [--retain/-f]\n"
            << "                       [--verbose/-v]\n" << std::endl;
  std::cout << "ARGUMENTS:\n" << std::endl;
  std::cout << "  -1/-2, --left/--right  Specifies simple mode." << std::endl;
  std::cout << "                         Path to left/right read FASTQ(s) should follow the corresponding flags." << std::endl;
  std::cout << "                         To specify multiple pairs of files, separate file paths with commas" << std::endl;
  std::cout << "  -a,   --assembly       Specifies an assembly for Semblans to operate on." << std::endl;
  std::cout << "                         Only used during postprocessing." << std::endl;
  std::cout << "  -od,  --output         Specifies path to where Semblans should place output files." << std::endl;
  std::cout << "                         Directory must already exist" << std::endl;
  std::cout << "  -cfg, --config         Specifies config file mode." << std::endl;
  std::cout << "                         Path to config file should follow this argument" << std::endl;
  std::cout << "  -t,   --threads        Specifies number of threads/CPU cores to employ" << std::endl;
  std::cout << "  -r,   --ram            Specifies ammount of memory/RAM (GB) to dedicate" << std::endl;
  std::cout << "  -f,   --retain         Prevents deletion of intermediate files in pipeline" << std::endl;
  std::cout << "  -v,   --verbose        Prints all output from Semblans and sub-programs" << std::endl;
  std::cout << "  -h,   --help           Displays this help screen" << std::endl;
 
}


void parseArgv(int argc, char * argv[], std::string & command,
               std::string & leftReads, std::string & rightReads,
               std::string & assembly, std::string & refProt,
               std::string & outDir, std::string & threadStr,
               std::string & ramStr, std::string & pathConfig,
               std::string & kraken2Dbs, bool & retainInterFiles,
               bool & verboseOutput) {

  for (int i = 0; i < argc; i++) {

    // Check for semblans command (preprocess/assemble/postprocess)
    // If none is given, will perform all
    if (strcmp("preprocess", argv[i]) == 0 ||
        strcmp("Preprocess", argv[i]) == 0 ||
        strcmp("pre", argv[i]) == 0 ||
        strcmp("Pre", argv[i]) == 0 ||
        strcmp("pr", argv[i]) == 0 ||
        strcmp("Pr", argv[i]) == 0) {
      command = "preprocess";
    }
    else if (strcmp("assemble", argv[i]) == 0 ||
             strcmp("Assemble", argv[i]) == 0 ||
             strcmp("ass", argv[i]) == 0 ||
             strcmp("Ass", argv[i]) == 0 ||
             strcmp("as", argv[i]) == 0 ||
             strcmp("As", argv[i]) == 0 ||
             strcmp("a", argv[i]) == 0 ||
             strcmp("A", argv[i]) == 0) {
      command = "assemble";
    }
    else if (strcmp("postprocess", argv[i]) == 0 ||
             strcmp("Postprocess", argv[i]) == 0 ||
             strcmp("post", argv[i]) == 0 ||
             strcmp("Post", argv[i]) == 0 ||
             strcmp("pos", argv[i]) == 0 ||
             strcmp("Pos", argv[i]) == 0 ||
             strcmp("po", argv[i]) == 0 ||
             strcmp("Po", argv[i]) == 0) {
      command = "postprocess";
    }
    else if (strcmp("all", argv[i]) == 0 ||
             strcmp("All", argv[i]) == 0) {
      command = "all";
    }

    // Check for config file path
    else if (strcmp("--config", argv[i]) == 0 ||
             strcmp("-cfg", argv[i]) == 0) {
      if (strcmp(argv[i + 1] + strlen(argv[i + 1]) - 4, ".ini") == 0 ||
          strcmp(argv[i + 1] + strlen(argv[i + 1]) - 4, ".INI") == 0) {
        pathConfig = argv[i + 1];
        fs::path pathConfigFile(pathConfig.c_str());
        if (!fs::exists(pathConfigFile)) {
          // ERROR: Config file not found!
          std::cout << "ERROR: Config file: " << pathConfig << " not found\n" << std::endl;
          exit(1);
        }
      }
      else {
        // ERROR: Config flag invoked, but no config file specified!
        std::cerr << "ERROR: If using '--config', you must specify config file (.INI)" << std::endl;
        std::cerr << "  (example: --config path/to/config.ini)\n" << std::endl;
        exit(1);
      }
    }

    // Check for '--left' read sequence FASTQ file(s)
    else if (strcmp("--left", argv[i]) == 0 ||
             strcmp("-1", argv[i]) == 0) {
      leftReads = argv[i + 1];
    }
    // Check for '--right' read sequence FASTQ file(s)
    else if (strcmp("--right", argv[i]) == 0 ||
             strcmp("-2", argv[i]) == 0) {
      rightReads = argv[i + 1];
    }
    // Check for '--assembly' transcripts sequence FASTA file
    else if (strcmp("--assembly", argv[i]) == 0 ||
             strcmp("-a", argv[i]) == 0) {
      assembly = argv[i + 1];
      if (!fs::exists(fs::path(assembly.c_str()))) {
        std::cerr << "ERROR: Assemble '" + assembly + "' does not exist\n" << std::endl;
        exit(1);
      }
    }
    else if (strcmp("--reference-proteome", argv[i]) == 0 ||
             strcmp("--ref-proteome", argv[i]) == 0 ||
             strcmp("-rp", argv[i]) == 0) {
      refProt = argv[i + 1];
      if (!fs::exists(fs::path(refProt.c_str()))) {
        std::cerr << "ERROR: Reference proteome '" + refProt + "' does not exist\n" << std::endl;
        exit(1);
      }
    }
    else if (strcmp("--kraken-db", argv[i]) == 0 ||
             strcmp("-kdb", argv[i]) == 0) {
      kraken2Dbs = argv[i + 1];
    }
    // Check for '--output', for specifying where outputs should
    // go if no config file is used
    else if (strcmp("--output-directory", argv[i]) == 0 ||
             strcmp("--output", argv[i]) == 0 ||
             strcmp("-od", argv[i]) == 0) {
      outDir = argv[i + 1];
      if (!fs::exists(outDir.c_str())) {
        std::cerr << "ERROR: Output directory '" + outDir + "' does not exist\n" << std::endl;
        exit(1);
      }
    }

    // Check for number of threads flag
    else if (strcmp("--threads", argv[i]) == 0 ||
             strcmp("--Threads", argv[i]) == 0 ||
             strcmp("-t", argv[i]) == 0 ||
             strcmp("-T", argv[i]) == 0) {
      if (i != argc - 1) {
        threadStr = argv[i + 1];
        for (int j = 0; j < strlen(argv[i + 1]); j++) {
          if (!isdigit(threadStr[j])) {
            // ERROR: Bad thread argument
            std::cerr << "ERROR: Invalid argument given for --threads/-t" << std::endl;
            std::cerr << "  (example: --threads 8)\n" << std::endl;
            exit(1);
          }
        }
      }
      else {
        // No thread count given, using 1 thread
        threadStr = "1";
      }
    }

    // Check for ammount of ram to be dedicated
    else if (strcmp("--ram", argv[i]) == 0 ||
             strcmp("--Ram", argv[i]) == 0 ||
             strcmp("-r", argv[i]) == 0 ||
             strcmp("-R", argv[i]) == 0) {

      if (i != argc - 1) {
        ramStr = argv[i + 1];

        for (int j = 0; j < strlen(argv[i + 1]); j++) {
          if (!isdigit(ramStr[j])) {
            // ERROR: Bad memory argument
            std::cerr << "ERROR: Invalid argument given for --ram/-r" << std::endl;
            std::cerr << "  (example: --ram 10)\n" << std::endl;
            exit(1);
          }
        }
      }
      else {
        // No RAM ammount given, using 1 GB
        ramStr = "8";
      }
    }
    // Check for '--retain' flag, which tells Semblans not to delete outputs from
    // intermediate steps in the pipeline
    else if (strcmp("--retain", argv[i]) == 0 ||
             strcmp("--Retain", argv[i]) == 0 ||
             strcmp("-f", argv[i]) == 0 ||
             strcmp("-F", argv[i]) == 0) {
      retainInterFiles = true;
    }
    // Check for '--berbose' flag, which tells Semblans to print a detailed output
    // to the terminal's standard output. This does not affect verbosity of log files.
    // Regardless of terminal output, log files always receive maximum verbosity.
    else if (strcmp("--verbose", argv[i]) == 0 ||
             strcmp("--Verbose", argv[i]) == 0 ||
             strcmp("-v", argv[i]) == 0 ||
             strcmp("-V", argv[i]) == 0) {
      verboseOutput = true;
    }
    // Flag not recognized. Report and exit;
    else {
      std::cerr << "ERROR: Unrecognized flag '" << argv[i] << "'" << std::endl;
      std::cerr << "  For a list of command flags, call" << std::endl;
      std::cerr << "  semblans --help/-h\n" << std::endl;
      exit(1);
    }
  }
}

void preprocess(std::vector<std::string> sraRuns, bool serialProcess, std::string pathConfig,
                std::string leftReads, std::string rightReads, std::string kraken2Dbs,
                std::string outDir, std::string threadStr, std::string ramStr,
                std::string retain, std::string verbose, std::string entirePipeline,
                std::string logFilePath) {
  
  std::ifstream cfgIniFile;
  std::ofstream cfgIniSub;
  std::string currLine;
  std::string currCfgIniSub;
  std::string currPreCmd;
  size_t pos;
  int result;

  if (pathConfig == "") {
    if (leftReads == "" && rightReads == "") {
      std::cerr << "ERROR: If not using '--config', user must specify ";
      std::cerr << "the left/right read files for preprocessing" << std::endl;
      std::cerr << "  (example: --left reads_1_left.fq,reads_2_left.fq,..." << std::endl;
      std::cerr << "            --right reads_1_right.fq,reads2_right.fq,..." << std::endl;
      exit(1);
    }
  }

  std::string preCmd = SEMBLANS_DIR + "preprocess " + pathConfig + " " +
                       leftReads + " " + rightReads + " " + kraken2Dbs + " " +
                       outDir + " " + threadStr + " " + ramStr +
                       retain + verbose + entirePipeline;

  if (serialProcess) {
    for (auto sraStr : sraRuns) {
      currCfgIniSub = pathConfig.substr(0, pathConfig.rfind(".ini")).insert(pathConfig.rfind("/") + 1, ".") +
                      "." + std::string(fs::path(sraStr.c_str()).stem().c_str()) + ".ini";
      std::replace(currCfgIniSub.begin(), currCfgIniSub.end(), ' ', '_');
      cfgIniFile.open(pathConfig);
      cfgIniSub.open(currCfgIniSub);
      while (std::getline(cfgIniFile, currLine)) {
        for (auto sraStrOther : sraRuns) {
          if (sraStrOther == sraStr) {
            continue;
          }
          pos = currLine.find(sraStrOther);
          if (pos != std::string::npos) {
            currLine.replace(currLine.find(sraStrOther), sraStrOther.length(), "");
          }
        }
        cfgIniSub << currLine << std::endl;
      }
      cfgIniFile.close();
      cfgIniSub.close();
      currPreCmd = SEMBLANS_DIR + "preprocess " + currCfgIniSub + " " +
                   leftReads + " " + rightReads + " " + kraken2Dbs + " " +
                   outDir + " " + threadStr + " " + ramStr + " " + retain +
                   verbose + entirePipeline;

      logOutput("\nPerforming preprocessing only\n\n", logFilePath);
      if (verbose == " true") {
        logOutput("  Running command: " + currPreCmd + "\n", logFilePath);
      }

      result = system(currPreCmd.c_str());
      checkExitSignal(result, logFilePath);
    }
  }
  else {
    logOutput("\nPerforming preprocessing only\n\n", logFilePath);
    if (verbose == " true") {
      logOutput("  Running command: " + preCmd + "\n", logFilePath);
    }
    result = system(preCmd.c_str());
    checkExitSignal(result, logFilePath);
  }
}


void assemble(std::vector<std::string> sraRuns, bool serialProcess, std::string pathConfig,
              std::string leftReads, std::string rightReads, std::string kraken2Dbs,
              std::string outDir, std::string threadStr, std::string ramStr,
              std::string retain, std::string verbose, std::string entirePipeline,
              std::string logFilePath) {

  std::ifstream cfgIniFile;
  std::ofstream cfgIniSub;
  std::string currLine;
  std::string currCfgIniSub;
  std::string currAssCmd;
  size_t pos;
  int result;

  std::string assCmd = SEMBLANS_DIR + "assemble " + pathConfig + " " +
                       leftReads + " " + rightReads + " " + outDir + " " +
                       threadStr + " " + ramStr + retain + verbose + entirePipeline;

  if (serialProcess) {
    for (auto sraStr : sraRuns) {
      currCfgIniSub = pathConfig.substr(0, pathConfig.rfind(".ini")).insert(pathConfig.rfind("/") + 1, ".") +
                      "." + std::string(fs::path(sraStr.c_str()).stem().c_str()) + ".ini";
      std::replace(currCfgIniSub.begin(), currCfgIniSub.end(), ' ', '_');
      cfgIniFile.open(pathConfig);
      cfgIniSub.open(currCfgIniSub);
      while (std::getline(cfgIniFile, currLine)) {
        for (auto sraStrOther : sraRuns) {
          if (sraStrOther == sraStr) {
            continue;
          }
          pos = currLine.find(sraStrOther);
          if (pos != std::string::npos) {
            currLine.replace(currLine.find(sraStrOther), sraStrOther.length(), "");
          }
        }
        cfgIniSub << currLine << std::endl;
      }
      cfgIniFile.close();
      cfgIniSub.close();

      currAssCmd = SEMBLANS_DIR + "assemble " + currCfgIniSub + " " +
                   leftReads + " " + rightReads + " " + outDir + " " +
                   threadStr + " " + ramStr + retain + verbose + entirePipeline;

      logOutput("\nPerforming assembly only\n\n", logFilePath);
      if (verbose == " true") {
        logOutput("  Running command: " + currAssCmd + "\n", logFilePath);
      }
      result = system(currAssCmd.c_str());
      checkExitSignal(result, logFilePath);
    }
  }
  else {
    logOutput("\nPerforming assembly only\n\n", logFilePath);
    if (verbose == " true") {
      logOutput("  Running command: " + assCmd + "\n", logFilePath);
    }
    result = system(assCmd.c_str());
    checkExitSignal(result, logFilePath);
  }
}


void postprocess(std::vector<std::string> sraRuns, bool serialProcess, std::string pathConfig,
                 std::string leftReads, std::string rightReads, std::string assembly,
                 std::string refProt, std::string outDir, std::string threadStr,
                 std::string ramStr, std::string retain, std::string verbose,
                 std::string entirePipeline, std::string logFilePath) {

  std::ifstream cfgIniFile;
  std::ofstream cfgIniSub;
  std::string currLine;
  std::string currCfgIniSub;
  std::string currPostCmd;
  size_t pos;
  int result;

  if (pathConfig == "") {
    if (assembly == "" || (leftReads == "" && rightReads == "")) {
      std::cerr << "ERROR: If not using '--config', user must specify assembly, " << std::endl;
      std::cerr << "the left/right read files that were used in assembly, and" << std::endl;
      std::cerr << "a reference proteome." << std::endl;
      std::cerr << "  (example: --assembly transcripts.fa" << std::endl;
      std::cerr << "            --ref-proteome refProt.fa" << std::endl;
      std::cerr << "            --left reads_1_left.fq,reads_2_left.fq,..." << std::endl;
      std::cerr << "            --right reads_1_right.fq, reads_2_right.fq,..." << std::endl;
      exit(1);
    }
    if (refProt == "") {
      std::cerr << "ERROR: If not using '--config', ";
      std::cerr << "user must specify a reference proteome FASTA" << std::endl;
      std::cerr << "  (example: --reference-proteome path/to/ref_prot.fa)\n" << std::endl;
      exit(1);
    }
  }

  std::string postCmd = SEMBLANS_DIR + "postprocess " + pathConfig + " " +
                        leftReads + " " + rightReads + " " + assembly + " " +
                        refProt + " " + outDir + " " + threadStr + " " + ramStr + " " +
                        retain + verbose + entirePipeline;

  if (serialProcess) {
    for (auto sraStr : sraRuns) {
      currCfgIniSub = pathConfig.substr(0, pathConfig.rfind(".ini")).insert(pathConfig.rfind("/") + 1, ".") +
                      "." + std::string(fs::path(sraStr.c_str()).stem().c_str()) + ".ini";
      std::replace(currCfgIniSub.begin(), currCfgIniSub.end(), ' ', '_');
      cfgIniFile.open(pathConfig);
      cfgIniSub.open(currCfgIniSub);
      while (std::getline(cfgIniFile, currLine)) {
        for (auto sraStrOther : sraRuns) {
          if (sraStrOther == sraStr) {
            continue;
          }
          pos = currLine.find(sraStrOther);
          if (pos != std::string::npos) {
            currLine.replace(currLine.find(sraStrOther), sraStrOther.length(), "");
          }
        }
        cfgIniSub << currLine << std::endl;
      }
      cfgIniFile.close();
      cfgIniSub.close();

      currPostCmd = SEMBLANS_DIR + "postprocess " + pathConfig + " " +
                    leftReads + " " + rightReads + " " + assembly + " " +
                    refProt + " " + outDir + " " + threadStr + " " + ramStr +
                    retain + verbose + entirePipeline;

      logOutput("\nPerforming postprocess only\n\n", logFilePath);
      if (verbose == " true") {
        logOutput("  Running command: " + currPostCmd + "\n", logFilePath);
      }
      result = system(currPostCmd.c_str());
      checkExitSignal(result, logFilePath);
    }
  }
  else {
    logOutput("\nPerforming postprocess only\n\n", logFilePath);
    if (verbose == " true") {
      logOutput("  Running command: " + postCmd + "\n", logFilePath);
    }
    result = system(postCmd.c_str());
    checkExitSignal(result, logFilePath);
  }
}


int main(int argc, char * argv[]) {
  //print_intro();
  INI_MAP cfgIni;
  INI_MAP_ENTRY cfgIniGen;
  std::vector<std::string> sraRuns;
  std::string logFilePath;
  std::string threadStr;
  std::string ramStr;
  int numThreads;
  int ram;
  std::string pathConfig;
  std::string command;
  std::string leftReads;
  std::string rightReads;
  std::string assembly;
  std::string refProt;
  std::string kraken2Dbs;
  std::string outDir;
  bool useCfg = true;
  bool serialProcess = false;
  bool retainInterFiles;
  bool verboseOutput;

  std::string preCmd;
  std::string assCmd;
  std::string postCmd;
  if (argc == 1 || 
      (argc == 2 && 
       (strcmp("--help", argv[1]) == 0 ||
        strcmp("-h", argv[1]) == 0))) {
    print_help_base();
    exit(0);
  }
  if (argc == 2 && (strcmp("--version", argv[1]) == 0 ||
                    strcmp("-v", argv[1]) == 0)) {
    std::cout << "Semblans version: v1.0.6" << std::endl;
  }
  else {
    // Parse through flags in commands
    threadStr = "";
    ramStr = "";
    numThreads = 1;
    ram = 1;
    std::string pathConfig = "";
    std::string command = "";
    retainInterFiles = false;
    verboseOutput = false;

    // Parse through Semblans argument vector
    parseArgv(argc, argv, command, leftReads, rightReads,
              assembly, refProt, outDir, threadStr, ramStr,
              pathConfig, kraken2Dbs, retainInterFiles,
              verboseOutput);

    // If no Semblans submodule specified, run all three (preprocess, assemble, postprocess)
    if (command == "") {
      command = "all";
    }

    if (kraken2Dbs == "") {
      kraken2Dbs = "null";
    }
    // If no config file specified, check for sequence files in Semblans call
    if (pathConfig == "") {
      useCfg = false;
      if (outDir == "") {
        outDir = ".";
      }
      // If running entire assembly without config file, define names of resulting assemblies
      if (command == "all") {
        // Placeholder: create assemblies vector
        size_t commaIndF;
        size_t commaIndR;
        size_t currPosF = 0;
        size_t currPosR = 0;
        size_t numIndex;
        std::string currForward;
        std::string currReverse;
        std::string currPrefixF;
        std::string currPrefixR;
        fs::path currPathF;
        fs::path currPathR;
        std::string currAssembly;
        std::vector<std::string> assemblies;
        do {
          commaIndF = leftReads.find(',', currPosF);
          commaIndR = rightReads.find(',', currPosR);

          currForward = leftReads.substr(currPosF, commaIndF - currPosF - 1);
          currReverse = rightReads.substr(currPosR, commaIndR - currPosR - 1);

          currPathF = fs::canonical(fs::path(currForward.c_str()));
          currPathR = fs::canonical(fs::path(currReverse.c_str()));

          currPrefixF = std::string(currPathF.stem().c_str());
          currPrefixR = std::string(currPathR.stem().c_str());

          if (currPrefixF.back() == '1' && currPrefixR.back() == '2') {
            currPrefixF.pop_back();
            currPrefixF.pop_back();
          }
          currAssembly = outDir + "/assembly/00-Transcript_assembly/" + currPrefixF + ".Trinity.fasta";
          assemblies.push_back(currAssembly);
          currPosF = commaIndF + 1;
          currPosR = commaIndR + 1;
        } while (commaIndF != std::string::npos);
        for (auto ass : assemblies) {
          assembly += ass;
          if (ass != assemblies.back()) {
            assembly += ",";
          }
        }
      }
    }

    // If config file being used, set up workspace for project
    if (useCfg) {
      leftReads = "null";
      rightReads = "null";
      kraken2Dbs = "null";
      refProt = "null";
      outDir = "null";
      assembly = "null";
      cfgIni = make_ini_map(pathConfig.c_str());
      cfgIniGen = cfgIni["General"];
      logFilePath = std::string((fs::canonical(fs::path(cfgIniGen["log_file"].c_str()).parent_path()) /
                              fs::path(cfgIniGen["log_file"].c_str()).filename()).c_str());
    
      fs::create_directory((fs::canonical(fs::path(cfgIniGen["output_directory"].c_str())) /
                            fs::path(cfgIniGen["project_name"].c_str())));
      //std::ofstream logFile(logFilePath, std::ios_base::trunc);

      serialProcess = ini_get_bool(cfgIniGen.at("serial_processing").c_str(), 0);
      if (serialProcess) {
        for (auto sraCode : cfgIni.at("SRA accessions")) {
          sraRuns.push_back(sraCode.first);
        }
        for (auto sraFile : cfgIni.at("Local files")) {
          sraRuns.push_back(sraFile.first);
        }
      }
    }
    // If config file not being used
    else {
      pathConfig = "null";
      logFilePath = std::string((fs::canonical(fs::path(outDir.c_str()).parent_path()) /
                                "log.txt").c_str());
    }

    std::ofstream logFile(logFilePath, std::ios_base::trunc);
 
    std::string verbose;
    std::string retain;
    std::string entirePipeline;
    if (verboseOutput) {
      verbose = " true";
    }
    else {
      verbose = " false";
    }
    if (retainInterFiles) {
      retain = " true";
    }
    else {
      retain = " false";
    }
    if (command == "all") {
      entirePipeline = " true";
    }
    else {
      entirePipeline = " false";
    }
    postCmd = SEMBLANS_DIR + "postprocess " + pathConfig + " " +
              leftReads + " " + rightReads + " " + assembly + " " +
              refProt + " " + outDir + " " +
              std::to_string(numThreads) + " " +
              std::to_string(ram);
    postCmd += retain;
    postCmd += verbose;
    postCmd += entirePipeline;
    int result;

    print_intro(logFilePath);

    std::ifstream cfgIniFile;
    std::ofstream cfgIniSub;
    std::string currLine;
    std::string currCfgIniSub;
    std::string currPreCmd;
    std::string currAssCmd;
    std::string currPostCmd;
    size_t pos;

    // Case 1: preprocess
    if (command == "preprocess") {
      preprocess(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
                 kraken2Dbs, outDir, threadStr, ramStr, retain, verbose,
                 entirePipeline, logFilePath);
      exit(0);
    }
    // Case 2: assemble
    if (command == "assemble") {
      assemble(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
               kraken2Dbs, outDir, threadStr, ramStr, retain, verbose,
               entirePipeline, logFilePath);
      exit(0);
    }
    // Case 3: postprocess
    if (command == "postprocess") {
      postprocess(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
                  assembly, outDir, refProt, threadStr, ramStr, retain, verbose,
                  entirePipeline, logFilePath);
            
      exit(0);
    }
    // Case 4: all three
    if (command == "all") {
      std::string currLeft;
      std::string currRight;
      std::vector<std::string> assemblies;
      if (serialProcess) {
        for (auto sraStr : sraRuns) {
          currCfgIniSub = pathConfig.substr(0, pathConfig.rfind(".ini")).insert(pathConfig.rfind("/") + 1, ".") +
                          "." + std::string(fs::path(sraStr.c_str()).stem().c_str()) + ".ini";
          
          std::replace(currCfgIniSub.begin(), currCfgIniSub.end(), ' ', '_');
          cfgIniFile.open(pathConfig);
          cfgIniSub.open(currCfgIniSub);
          while (std::getline(cfgIniFile, currLine)) {
            for (auto sraStrOther : sraRuns) {
              if (sraStrOther == sraStr) {
                continue;
              }
              pos = currLine.find(sraStrOther);
              if (pos != std::string::npos) {
                currLine.replace(currLine.find(sraStrOther), sraStrOther.length(), "");
              }
            }
            cfgIniSub << currLine << std::endl;
          }
          cfgIniFile.close();
          cfgIniSub.close();

          std::replace(sraStr.begin(), sraStr.end(), ' ', '\n');

          currPreCmd = SEMBLANS_DIR + "preprocess " + currCfgIniSub + " " +
                       leftReads + " " + rightReads + " " + kraken2Dbs + " " +
                       outDir + " " +
                       std::to_string(numThreads) + " " +
                       std::to_string(ram) + retain + verbose + entirePipeline;

          currAssCmd = SEMBLANS_DIR + "assemble " + currCfgIniSub + " " +
                       leftReads + " " + rightReads + " " + outDir + " " +
                       std::to_string(numThreads) + " " +
                       std::to_string(ram) + retain + verbose + entirePipeline;

          currPostCmd = SEMBLANS_DIR + "postprocess " + pathConfig + " " +
                        leftReads + " " + rightReads + " " + assembly + " " +
                        refProt + " " + outDir + " " +
                        std::to_string(numThreads) + " " +
                        std::to_string(ram) + retain + verbose + entirePipeline;



          logOutput("Performing entire assembly on:\n" + sraStr, logFilePath);
          logOutput("\n ┌───────────────────────────────────────────────────────┐", logFilePath);
          logOutput("\n │         Phase 1: Preprocessing of Short-reads         │", logFilePath);
          logOutput("\n └───────────────────────────────────────────────────────┘\n", logFilePath);
          if (verbose == " true") {
            logOutput("  Running command: " + currPreCmd + "\n", logFilePath);
          }
          result = system(currPreCmd.c_str());
          if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
            std::cerr << "\nPreprocess exited" << std::endl;
            system("setterm -cursor on");
            exit(1);
          }
          logOutput("\n ┌───────────────────────────────────────────────────────┐", logFilePath);
          logOutput("\n │        Phase 2: De Novo Assembly of Short-reads       │", logFilePath);
          logOutput("\n └───────────────────────────────────────────────────────┘\n", logFilePath);
          if (verbose == " true") {
            logOutput("  Running command: " + currAssCmd + "\n", logFilePath);
          }
          result = system(currAssCmd.c_str());
          if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
            std::cerr << "\nAssembly exited" << std::endl;
            system("setterm -cursor on");
            exit(1);
          }
          logOutput("\n ┌────────────────────────────────────────────────────────┐", logFilePath);
          logOutput("\n │    Phase 3: Postprocessing of Assembled Transcripts    │", logFilePath);
          logOutput("\n └────────────────────────────────────────────────────────┘\n", logFilePath);
          if (verbose == " true") {
            logOutput("  Running command: " + currPostCmd + "\n", logFilePath);
          }
          result = system(currPostCmd.c_str());
          if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
           std::cerr << "\nPostprocess exited" << std::endl;
           system("setterm -cursor on");
           exit(1);
          }
        }
      }
      else {
        logOutput("Performing entire assembly\n", logFilePath);
        logOutput("\n ┌───────────────────────────────────────────────────────┐", logFilePath);
        logOutput("\n │         Phase 1: Preprocessing of Short-reads         │", logFilePath);
        logOutput("\n └───────────────────────────────────────────────────────┘\n", logFilePath);
        preprocess(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
                   kraken2Dbs, outDir, threadStr, ramStr, retain, verbose,
                   entirePipeline, logFilePath);

        if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
          std::cerr << "\nPreprocess exited" << std::endl;
          system("setterm -cursor on");
          exit(1);
        }
        logOutput("\n ┌───────────────────────────────────────────────────────┐", logFilePath);
        logOutput("\n │        Phase 2: De Novo Assembly of Short-reads       │", logFilePath);
        logOutput("\n └───────────────────────────────────────────────────────┘\n", logFilePath);
        assemble(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
                 kraken2Dbs, outDir, threadStr, ramStr, retain, verbose,
                 entirePipeline, logFilePath);
        if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
          std::cerr << "\nAssembly exited" << std::endl;
          system("setterm -cursor on");
          exit(1);
        }
        logOutput("\n ┌────────────────────────────────────────────────────────┐", logFilePath);
        logOutput("\n │    Phase 3: Postprocessing of Assembled Transcripts    │", logFilePath);
        logOutput("\n └────────────────────────────────────────────────────────┘\n", logFilePath);
        postprocess(sraRuns, serialProcess, pathConfig, leftReads, rightReads,
                    assembly, refProt, outDir, threadStr, ramStr, retain, verbose,
                    entirePipeline, logFilePath);
        if (WIFSIGNALED(result) || (result != 0 && WIFEXITED(result) == 1)) {
          std::cerr << "\nPostprocess exited" << std::endl;
          system("setterm -cursor on");
          exit(1);
        }
        exit(0);
      }
    }
  }
}
