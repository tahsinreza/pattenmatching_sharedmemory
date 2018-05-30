//
// Created by qiu on 17/05/18.
//

#include <bitset>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

#include "totem_patternmatching_cmd_line_option.h"

namespace patternmatching {

CmdLineOption::CmdLineOption() {
  m_inputGraphFilePath="";
  m_inputVertexMetadataFilePath="";
  m_inputEdgeMetadataFilePath="";
  m_inputPatternDirectory="";
  m_outputResultDirectory="";
}

void CmdLineOption::printHelp() {
  std::cerr << "Usage: -i <string> -p <string> -o <string>\n"
            << " -i <string>   - input graph base filename (required)\n"
            << " -v <string>   - vertex metadata base filename (optional, Default is degree based metadata)\n"
            << " -e <string>   - edge metadata base filename (optional)\n"
            << " -p <string>   - pattern base directory (required)\n"
            << " -o <string>   - output base directory (required)\n"
            << " -h            - print help and exit\n\n";

}

void CmdLineOption::parseCmdLine(int argc, char** argv) {

  bool printHelp = false;
  std::bitset<CmdLineOption::C_REQUIRED_QRGUMENTS_NUMBER> requiredInputSet;
  requiredInputSet.reset();

  char c;
  while ((c = static_cast<char>(getopt(argc, argv, "i:b:v:e:p:o:x:h "))) != -1) {
    switch (c) {
      case 'i' :
        m_inputGraphFilePath = optarg;
        requiredInputSet.set(INPUT_GRAPH_FILE);
      case 'v' :
        m_inputVertexMetadataFilePath = optarg;
        break;
      case 'e' :
        m_inputEdgeMetadataFilePath = optarg;
        break;
      case 'p' :
        m_inputPatternDirectory = optarg;
        requiredInputSet.set(INPUT_PATTERN_DIRECTORY);
        break;
      case 'o' :
        m_outputResultDirectory = optarg;
        requiredInputSet.set(OUTPUT_RESULT_DIRECTORY);
        break;

      case 'h' :
        printHelp = true;
        break;
      default:
        std::invalid_argument(std::string("Unrecognized Option : ") + c + ", Ignore.");
        printHelp = true;
        break;
    }
  }

  if (printHelp || !requiredInputSet.all()) {
    CmdLineOption::printHelp();
    exit(-1);
  }
}

const std::string &CmdLineOption::getInputGraphFilePath() const {
  return m_inputGraphFilePath;
}
void CmdLineOption::setInputGraphFilePath(const std::string &inputGraphFilePath) {
  m_inputGraphFilePath = inputGraphFilePath;
}
const std::string &CmdLineOption::getInputVertexMetadataFilePath() const {
  return m_inputVertexMetadataFilePath;
}
void CmdLineOption::setInputVertexMetadataFilePath(const std::string &inputVertexMetadataFilePath) {
  m_inputVertexMetadataFilePath = inputVertexMetadataFilePath;
}
const std::string &CmdLineOption::getInputEdgeMetadataFilePath() const {
  return m_inputEdgeMetadataFilePath;
}
void CmdLineOption::setInputEdgeMetadataFilePath(const std::string &inputEdgeMetadataFilePath) {
  m_inputEdgeMetadataFilePath = inputEdgeMetadataFilePath;
}
const std::string &CmdLineOption::getInputPatternDirectory() const {
  return m_inputPatternDirectory;
}
void CmdLineOption::setInputPatternDirectory(const std::string &inputPatternDirectory) {
  m_inputPatternDirectory = inputPatternDirectory;
}
const std::string &CmdLineOption::getOutputResultDirectory() const {
  return m_outputResultDirectory;
}
void CmdLineOption::setOutputResultDirectory(const std::string &outputResultDirectory) {
  m_outputResultDirectory = outputResultDirectory;
}

}