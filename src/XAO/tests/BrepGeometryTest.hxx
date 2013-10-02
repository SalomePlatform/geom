#ifndef __XAO_BREPGEOMETRY_TEST_HXX__
#define __XAO_BREPGEOMETRY_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class BrepGeometryTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(BrepGeometryTest);
        CPPUNIT_TEST(testGetIDs);
        CPPUNIT_TEST(testGetReferences);
        CPPUNIT_TEST(testGetNames);
        CPPUNIT_TEST(testGetEdgeVertices);
        CPPUNIT_TEST(testGetFaceEdges);
        CPPUNIT_TEST(testSolidFaces);
        CPPUNIT_TEST(testGetVertex);
        CPPUNIT_TEST(testGetEdgeLength);
        CPPUNIT_TEST(testGetFaceArea);
        CPPUNIT_TEST(testGetSolidVolume);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGetIDs();
        void testGetReferences();
        void testGetNames();
        void testGetEdgeVertices();
        void testGetFaceEdges();
        void testSolidFaces();

        void testGetVertex();
        void testGetEdgeLength();
        void testGetFaceArea();
        void testGetSolidVolume();

        void testParse();
    };
}

#endif // __XAO_BREPGEOMETRY_TEST_HXX__
