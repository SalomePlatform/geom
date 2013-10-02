#ifndef __XAO_FIELD_TEST_HXX__
#define __XAO_FIELD_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

#include "../XAO_Xao.hxx"
#include "../XAO_Field.hxx"
#include "../XAO_Step.hxx"

namespace XAO
{
    class FieldTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(FieldTest);
        CPPUNIT_TEST(testBooleanField);
        CPPUNIT_TEST(testDoubleField);
        CPPUNIT_TEST(testIntegerField);
        CPPUNIT_TEST(testStringField);
        CPPUNIT_TEST(testBooleanStep);
        CPPUNIT_TEST(testIntegerStep);
        CPPUNIT_TEST(testDoubleStep);
        CPPUNIT_TEST(testStringStep);
        CPPUNIT_TEST(testBooleanStepValues);
        CPPUNIT_TEST(testIntegerStepValues);
        CPPUNIT_TEST(testDoubleStepValues);
        CPPUNIT_TEST(testStringStepValues);
        CPPUNIT_TEST(testSetComponents);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        Field* testField(XAO::Type type);
        void testBooleanField();
        void testIntegerField();
        void testDoubleField();
        void testStringField();

        void testStep(XAO::Type type, Step* step);
        void testBooleanStep();
        void testIntegerStep();
        void testDoubleStep();
        void testStringStep();

        void testBooleanStepValues();
        void testIntegerStepValues();
        void testDoubleStepValues();
        void testStringStepValues();
        void testSetComponents();
    };
}

#endif // __XAO_FIELD_TEST_HXX__
