#include "ncbi_blast.h"

void makeBlastDb(std::string protRef, std::string outDir) {
  fs::path pathProtRef(protRef.c_str());
  std::string protRefStr(pathProtRef.stem().c_str()); 
  fs::path outDbFile((outDir + "/" + protRefStr).c_str());
  int result;
  if (fs::exists(outDbFile)) {
    std::cout << "BLAST database found for: " + protRefStr << std::endl;
    return;
  }
  std::string makeblastdbCmd = MAKEBLASTDB + " -in " + protRef + " -dbtype prot -out " +
                               outDbFile.c_str();
  result = system(makeblastdbCmd.c_str());
  if (WIFSIGNALED(result)) {
    std::cout << "Exited with signal " << WTERMSIG(result) << std::endl;
    exit(1);
  }
}


void blastx(transcript transcripts, std::string blastDb,
            std::string threads, std::string outDir) {
  fs::path pathTrans = transcripts.get_trans_path_trinity();
  std::string pathTransStr(pathTrans.c_str());
  std::string transStr(pathTrans.stem().c_str());

  fs::path outBlastxFile((outDir + "/" + transStr + ".blastx").c_str());
  std::string outBlastxStr(outBlastxFile.c_str());
  int result;
  if (fs::exists(outBlastxFile)) {
    std::cout << "BLASTX output found for: " + transStr << std::endl;
    return;
  }
  std::string blastxCmd = BLASTX + " -db " + blastDb + " -query " + pathTransStr +
                          " -evalue " + "0.01" +
                          " -outfmt " + "\"6 qseqid qlen sseqid slen frames pident nident length mismatch gapopen qstart qend start send evalue bitscord\"" +
                          " -out " + outBlastxStr +
                          " -num_threads " + threads +
                          " -max_target_seqs 100";
  result = system(blastxCmd.c_str());
  if (WIFSIGNALED(result)) {
    std::cout << "Exited with signal " << WTERMSIG(result) << std::endl;
    exit(1);
  }
}
