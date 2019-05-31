####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     darknet_to_ell_impporter_test.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x
#
####################################################################################################

import sys
import os
import unittest

import numpy as np

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, "..", "..", "..", "utilities", "pythonlibs"))
sys.path.append(os.path.join(script_path, '..'))
import find_ell  # noqa 401
import ell
import logger
import darknet_to_ell


# Load a test darknet model and verify its output.
# The unit test model verifies the most common darknet layers:
# - convolutional
# - maxpool
# - avgpool
# - fully connected
# - softmax
class DarknetModelTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def test_darknet_model(self):
        # Create synthetic input data
        input1 = np.arange(28 * 28, dtype=np.float).reshape(28, 28, 1)
        # Create an ELL predictor from the darknet model files
        predictor = darknet_to_ell.predictor_from_darknet_model(
            'unittest.cfg', 'unittest.weights')

        # Feed the input through the model
        result1 = predictor.Predict(input1.ravel())
        # Verify its what we expect
        expectedResult1 = [0.09134083986282349, 0.09748589247465134, 0.09064911305904388, 0.13794259727001190,
                           0.16832095384597778, 0.08976214379072190, 0.06458559632301330, 0.07894224673509598,
                           0.12377665191888809, 0.05719388648867607]
        np.testing.assert_array_almost_equal(
            result1, expectedResult1, 5, 'prediction of first input does not match expected results!')

        # Feed the next input through the model
        input2 = np.flipud(input1)
        result2 = predictor.Predict(input2.ravel())
        # Verify its what we expect
        expectedResult2 = [0.08052270114421844, 0.08739096671342850, 0.08180813491344452, 0.24630726873874664,
                           0.12944690883159637, 0.08548084646463394, 0.06091265007853508, 0.07173667103052139,
                           0.11159289628267288, 0.04480091854929924]
        np.testing.assert_array_almost_equal(
            result2, expectedResult2, 5, 'prediction of second input does not match expected results!')

        # create a map and save to file
        ell_map = ell.neural.utilities.ell_map_from_predictor(predictor)
        ell_map.Save("darknet_test.map")

        # create a map and save to file
        ell_map = ell.neural.utilities.ell_map_from_predictor(
            predictor, step_interval_msec=100, lag_threshold_msec=150, function_prefix="DarknetTest")
        ell_map.Save("darknet_steppable_test.map")

        # now compile it
        compiler_options = ell.model.MapCompilerOptions()
        compiler_options.useBlas = False
        compiled = ell_map.Compile("host", "model", "test1", compilerOptions=compiler_options, dtype=np.float32)

        # call the compiled model
        compiledResults = np.array(compiled.Compute(input2.ravel(), dtype=np.float32))

        np.testing.assert_array_almost_equal(
            compiledResults, expectedResult2, 5,
            "Compiled prediction does not match expected results to 5 decimal places!")

        return


if __name__ == '__main__':
    logger.setup()
    unittest.main()
