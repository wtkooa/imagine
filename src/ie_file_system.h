#ifndef IE_FILE_SYSTEM_H
#define IE_FILE_SYSTEM_H

//___|"ie_file_system.h"|_______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

namespace ie
{

  class File
  {
    public:
    File();
    File(std::string);
    File(std::string, std::string);

    void setPath(std::string);
    void setName(std::string);
    void setExtension(std::string);

    std::string getPath(void);
    std::string getName(void);
    std::string getExtension(void);
    std::string getString(void);
    const char* getCString(void);

    private:
    std::string path;
    std::string name;
    std::string extension;

    void parseFilename(std::string);

  };

}

#endif
