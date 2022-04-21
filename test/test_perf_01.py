from contextlib import contextmanager
from datetime import datetime, timedelta
from inspect import getfile
from os.path import abspath, dirname, join
from unittest import TestCase, main

import salome
salome.salome_init_without_session()

from salome.geom import geomBuilder
geom_builder = geomBuilder.New()

data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))

@contextmanager
def no_longer_than(seconds):
    """
    Context mananger to check that an execution of given code does not
    exceed maximum expected time.   

    Example of usage:

        with wait(5):
            do_something_that_should_take_no_more_than_five_seconds()

    Arguments:
        seconds: max time limit

    Raises:
        AssertionError: if time limit is exceeded.
    """
    expected = timedelta(seconds=seconds)
    start = datetime.now()
    yield
    end = datetime.now()
    spent = end-start
    assert spent <= expected, f'Expected maximum delay is exceeded: {spent.total_seconds():.2f} > {seconds:.2f}!'

class TesselationTest(TestCase):
    """Test Tesselate() method."""

    def test_performance_01(self):
        """Check performance of Tesselate() method: case 01."""
        shape = geom_builder.ImportBREP(join(data_dir, 'test_performance_01.brep'))
        with no_longer_than(5):
            geom_builder.Tesselate(shape, 0.17, False, 20)

if __name__ == "__main__":
    main()
