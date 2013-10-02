#ifndef __XAO_GEOMETRY_TEST_HXX__
#define __XAO_GEOMETRY_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class GeometryTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(GeometryTest);
        CPPUNIT_TEST(testGeometryElement);
        CPPUNIT_TEST(testGeometryElementList);
        CPPUNIT_TEST(testGeometry);
        CPPUNIT_TEST(testGeometryErrors);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGeometryElement();
        void testGeometryElementList();
        void testGeometry();
        void testGeometryErrors();
        void testSetElement();
    };
}

#endif // __XAO_GEOMETRY_TEST_HXX__
