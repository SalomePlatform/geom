#ifndef __XAO_UTILS_TEST_HXX__
#define __XAO_UTILS_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class XaoUtilsTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(XaoUtilsTest);
        CPPUNIT_TEST(testBoolean);
        CPPUNIT_TEST(testInteger);
        CPPUNIT_TEST(testDouble);
        CPPUNIT_TEST(testDimension);
        CPPUNIT_TEST(testType);
        CPPUNIT_TEST(testFormat);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testBoolean();
        void testInteger();
        void testDouble();
        void testDimension();
        void testType();
        void testFormat();
    };
}

#endif // __XAO_FIELD_TEST_HXX__
