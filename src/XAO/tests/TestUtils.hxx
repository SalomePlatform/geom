#ifndef __XAO_TESTUTILS_HXX__
#define __XAO_TESTUTILS_HXX__

#include <fstream>
#include <cstdlib>

namespace XAO
{
    class TestUtils
    {
    public:
        static std::string getTestFilePath(const std::string& fileName)
        {
            std::string dataDir = getenv("GEOM_SRC_DIR");
            dataDir += "/src/XAO/tests/data/" + fileName;
            return dataDir;
        }

        static char* readTextFile(const std::string& filePath)
        {
            std::ifstream rstr;
            int length;
            rstr.open(filePath.c_str());
            rstr.seekg(0, rstr.end);        // go to the end
            length = rstr.tellg();          // report location (this is the length)
            rstr.seekg(0, rstr.beg);        // go back to the beginning
            char* txt = new char[length];   // allocate memory for a buffer of appropriate dimension
            rstr.read(txt, length);         // read the whole file into the buffer
            rstr.close();

            return txt;
        }
    };
}

#endif /* __XAO_TESTUTILS_HXX__ */
