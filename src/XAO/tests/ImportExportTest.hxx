#ifndef __XAO_IMPORT_TEST_HXX__
#define __XAO_IMPORT_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

#include "../XAO_Xao.hxx"

namespace XAO
{
    class ImportExportTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(ImportExportTest);
        CPPUNIT_TEST(testExportNoGeometry);
        CPPUNIT_TEST(testExportGeometry);
        CPPUNIT_TEST(testGeometryError);
        CPPUNIT_TEST(testImportXao);
        CPPUNIT_TEST(testImportXaoFromText);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testExportNoGeometry();
        void testExportGeometry();
        void testGeometryError();
        void testImportXao();
        void testImportXaoFromText();

        void checkImport(Xao& xao);
    };
}

#endif // __XAO_IMPORT_TEST_HXX__
