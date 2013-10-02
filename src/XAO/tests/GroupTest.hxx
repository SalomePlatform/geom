#ifndef __XAO_GROUP_TEST_HXX__
#define __XAO_GROUP_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class GroupTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(GroupTest);
        CPPUNIT_TEST(testGroup);
        CPPUNIT_TEST(testGroupErrors);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGroup();
        void testGroupErrors();
    };
}

#endif // __XAO_GROUP_TEST_HXX__
