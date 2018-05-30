//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_PATTERNMATCHING_CMD_LINE_OPTION_H
#define TOTEM_PATTERNMATCHING_CMD_LINE_OPTION_H

#include <string>

namespace patternmatching {

class CmdLineOption {
 public:
  CmdLineOption();

  void parseCmdLine(int argc, char** argv);
  void printHelp();

  const std::string &getInputGraphFilePath() const;
  void setInputGraphFilePath(const std::string &inputGraphFilePath);
  const std::string &getInputVertexMetadataFilePath() const;
  void setInputVertexMetadataFilePath(const std::string &inputVertexMetadataFilePath);
  const std::string &getInputEdgeMetadataFilePath() const;
  void setInputEdgeMetadataFilePath(const std::string &inputEdgeMetadataFilePath);
  const std::string &getInputPatternDirectory() const;
  void setInputPatternDirectory(const std::string &inputPatternDirectory);
  const std::string &getOutputResultDirectory() const;
  void setOutputResultDirectory(const std::string &outputResultDirectory);

 private:
  static const int C_REQUIRED_QRGUMENTS_NUMBER = 3;
  enum RequiredArguments{ INPUT_GRAPH_FILE = 0, INPUT_PATTERN_DIRECTORY, OUTPUT_RESULT_DIRECTORY};

  std::string m_inputGraphFilePath;
  std::string m_inputVertexMetadataFilePath;
  std::string m_inputEdgeMetadataFilePath;
  std::string m_inputPatternDirectory;
  std::string m_outputResultDirectory;

};

}

#endif //TOTEM_PATTERNMATCHING_CMD_LINE_OPTION_H
