#ifndef __XAO_XAO_TEST_HXX__
#define __XAO_XAO_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class XaoTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(XaoTest);
        CPPUNIT_TEST(testGroups);
        CPPUNIT_TEST(testFields);
        CPPUNIT_TEST(testFieldsTypes);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGroups();
        void testFields();
        void testFieldsTypes();
    };
}

#endif // __XAO_GROUP_TEST_HXX__
