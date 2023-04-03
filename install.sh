#!/bin/sh

echo "Initiating install of Paando ..."

# Prepare Paando file structure
mkdir include
mkdir lib
mkdir external

#========================================================================
#/////////////   INSTALLATION OF THIRD-PARTY LIBRARIES   \\\\\\\\\\\\\\\\
#========================================================================

echo "Now installing required libraries ..."

# Install boost libraries
echo "  Installing Boost libraries ..."
wget -q https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.gz
tar -xf boost_1_81_0.tar.gz
cd boost_1_81_0
./bootstrap.sh --prefix=../ --with-libraries=system,filesystem,regex
./b2 install
mv LICENSE_1_0.txt ../include/boost/
cd ..
rm -rf boost_1_81_0*

# Install rapidXML
echo "  Installing rapidXML library ..."
wget -q https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip
unzip rapidxml-1.13.zip
mv rapidxml-1.13 rapidxml
mv rapidxml include/
rm rapidxml-1.13.zip

# Install libconfini
echo "  Installing libconfini library ..."
wget -q https://github.com/madmurphy/libconfini/releases/download/1.16.4/libconfini-1.16.4-x86_64-bin.tar.xz
tar -xf libconfini-1.16.4-x86_64-bin.tar.xz
mkdir ./include/libconfini
mv ./usr/include/* ./include/libconfini/
mv ./usr/lib/* ./lib/
mv ./usr/share/doc/libconfini/AUTHORS ./include/libconfini/
mv ./usr/share/doc/libconfini/COPYING ./include/libconfini/
rm libconfini-1.16.4-x86_64-bin.tar.xz
rm -rf ./usr/

#========================================================================
#/////////////   INSTALLATION OF THIRD-PARTY PACKAGES    \\\\\\\\\\\\\\\\
#========================================================================

echo "Now installing required packages ..."

cd ./external/

# Install NCBI sra-tools
echo "Installing SRA-Tools ..."
wget -q --output-document sratoolkit.tar.gz https://ftp-trace.ncbi.nlm.nih.gov/sra/sdk/current/sratoolkit.current-ubuntu64.tar.gz
tar -xf sratoolkit.tar.gz
mv sratoolkit.3.0.1* sratoolkit.3.0.1
cd sratoolkit.3.0.1
wget -q https://raw.githubusercontent.com/ncbi/sra-tools/master/LICENSE
cd ../
rm sratoolkit.tar.gz

# Install FastQC
echo "Installing FastQC ..."
wget -q --output-document fastqc.zip https://www.bioinformatics.babraham.ac.uk/projects/fastqc/fastqc_v0.11.9.zip
unzip fastqc.zip
chmod 755 FastQC/fastqc
rm fastqc.zip

# Install Rcorrector
echo "Installing Rcorrector ..."
wget -q --output-document rcorrector.tar.gz https://github.com/mourisl/Rcorrector/archive/refs/tags/v1.0.5.tar.gz
tar -xf rcorrector.tar.gz
cd Rcorrector-1.0.5
make
cd ../
rm rcorrector.tar.gz

# Install Trimmomatic
echo "Installing Trimmomatic ..."
wget -q --output-document trimmomatic.zip https://github.com/usadellab/Trimmomatic/files/5854859/Trimmomatic-0.39.zip
unzip trimmomatic.zip
cd Trimmomatic-0.39/adapters
cat TruSeq*.fa > TruSeq_all.fa # combine all adapter seqs into one
cd ../../
rm trimmomatic.zip

# Install Kraken2
echo "Installing Kraken2 ..."
wget -q --output-document kraken2.tar.gz https://github.com/DerrickWood/kraken2/archive/refs/tags/v2.1.2.tar.gz
tar -xf kraken2.tar.gz
cd kraken2-2.1.2
./install_kraken2.sh .
cd ../
rm kraken2.tar.gz

# Install Trinity
echo "Installing Trinity ..."
wget -q --output-document trinity.tar.gz https://github.com/trinityrnaseq/trinityrnaseq/releases/download/Trinity-v2.15.1/trinityrnaseq-v2.15.1.FULL.tar.gz
tar -xf trinity.tar.gz
mv trinityrnaseq-v2.15.1 trinityrnaseq-2.15.1
cd trinityrnaseq-2.15.1
make
cd ../
rm trinity.tar.gz

# Install NCBI BLAST
echo "Installing NCBI-BLAST+ ..."
wget -q --output-document ncbi-blast.tar.gz https://ftp.ncbi.nlm.nih.gov/blast/executables/LATEST/ncbi-blast-2.13.0+-x64-linux.tar.gz
tar -xf ncbi-blast.tar.gz
rm ncbi-blast.tar.gz

# Install Diamond
echo "Installing Diamond ..."
mkdir diamond-2.1.3
wget -q --output-document diamond.tar.gz https://github.com/bbuchfink/diamond/releases/download/v2.1.3/diamond-linux64.tar.gz
tar -xf diamond.tar.gz
mv diamond diamond-2.1.3
cd diamond-2.1.3
wget -q https://raw.githubusercontent.com/bbuchfink/diamond/master/LICENSE
cd ../
rm diamond.tar.gz


# Install Corset
echo "Installing Corset ..."
wget -q --output-document corset.tar.gz https://github.com/Oshlack/Corset/releases/download/version-1.09/corset-1.09-linux64.tar.gz
tar -xf corset.tar.gz
mv corset-1.09-linux64 corset-1.09
cd corset-1.09
chmod -x LICENSE
chmod -x COPYING
cd ../
rm corset.tar.gz

# Install Salmon
echo "Installing Salmon ..."
wget -q --output-document salmon.tar.gz https://github.com/COMBINE-lab/salmon/releases/download/v1.9.0/salmon-1.9.0_linux_x86_64.tar.gz
tar -xf salmon.tar.gz
mv salmon-1.9.0_linux_x86_64 salmon-1.9.0
cd salmon-1.9.0
wget -q https://raw.githubusercontent.com/COMBINE-lab/salmon/v1.9.0/LICENSE
cd ../
rm salmon.tar.gz

# Install TransDecoder
echo "Installing TransDecoder ..."
wget -q --output-document transdecoder.tar.gz https://github.com/TransDecoder/TransDecoder/archive/refs/tags/TransDecoder-v5.7.0.tar.gz
tar -xf transdecoder.tar.gz
mv TransDecoder-TransDecoder-v5.7.0 TransDecoder-5.7.0
rm transdecoder.tar.gz

#========================================================================
#///////////////////   BUILD PAANDO FROM SOURCE    \\\\\\\\\\\\\\\\\\\\\\
#========================================================================

cd ../
mkdir ./bin/
make

echo "\nPaando has been installed successfully!\n"
