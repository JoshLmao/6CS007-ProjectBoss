import unreal_engine as ue
# Pandas & OS
import pandas as pd
import os.path
# NumPy
import numpy as np
# SK Learn
from sklearn.preprocessing import normalize

# Main update_costs class to use with Project Boss
class PBUpdateCosts:

    # CSV data loaded from file
    CSV_Data = None
    # Trained theta_best from ML data
    ThetaBeta = None
    # Prediction multiplier
    PredictMultiplier = 1

#
# public
#
    # Initializes the ML with data from 'csv_path'
    def init_ml(self, csv_path):
        ue.log('Initializing ML training...')
        # Check for errors with csv_path and return if error
        isValid = self.checkPath(csv_path)
        if isValid == False:
            return None # Return if problem with CSV path

        # Read CSV data and normalize to be between 0 and 1
        CSV_Data = pd.read_csv(csv_path, header=None)       # Ignore header, data starts on 0th row
        CSV_Data = CSV_Data.to_numpy()
        CSV_Data = normalize(CSV_Data, norm='l2')

        input_output_count = CSV_Data.shape[0]
        values_count = CSV_Data.shape[1]

        # Determine input count by removing last row which is output
        inputs_count = input_output_count - 1
        output_count = 1

        # Split CSV_Data into input
        input_values = CSV_Data[ 0:inputs_count ]
        output_values = CSV_Data[ inputs_count:inputs_count + output_count ]

        # Insert bias of 1's
        input_values = self.insert_bias(input_values)
        # Transpose to make data compatiable
        input_values = input_values.T

        # setup values for ML
        iterations = 1000
        eta = 0.1
        # Set M to amount of data values
        m = output_values.shape[1]
        # random theta with shape of inputs
        theta = np.random.rand(input_values.shape[1], 1)

        # Iterate for the amount to learn data
        for i in range(iterations):
            err = input_values.dot(theta)
            gradients = 2 / m * input_values.T.dot(err)
            # Update theta
            theta = theta - eta * gradients
            # Square the error
            sq_err = err * err
            # Determine mean of error
            mean_err = np.mean(sq_err)

        # set best theta to determined theta
        self.ThetaBeta = theta
        ue.log("Determined ThetaBest! Now able to predict")


    # Called through UE4 C++. Generates a new
    def predict_cost(self, baseCost, wasSuccess, damage, averageExecuteSeconds):
        if self.ThetaBeta == None:
            ue.error("No theta_best has been determined!")
            return None

        ue.log("Predicting action's cost...")
        ue.log("baseCost:'{0}' success:'{1}' damage:'{2}' avgExecuteSecs:'{3}'".format(baseCost, wasSuccess, damage, averageExecuteSeconds))
        
        # Construct array and normalize with given values
        input_vals =  [ [ baseCost, wasSuccess, damage, averageExecuteSeconds ] ]
        input_vals = normalize(input_vals, norm='l2')
        
        # Transpose and insert bias into correct place
        input_vals = input_vals.T
        input_vals = self.insert_bias(input_vals)

        # Transpose back to make compatiable with theta
        input_vals = input_vals.T

        # Make prediction and get single value
        prediction = input_vals.dot(self.ThetaBest)
        # Multiply by multiplier
        prediction *= self.PredictMultiplier

        new_cost = prediction[0][0]

        # Log and return value
        ue.log("Prediction of '{0}'".format(new_cost))
        return new_cost

    # Sets the prediction multiplier to the given value
    def set_predict_multiplier(self, multiplier):
        # Only set multiplier if valid, more than 1
        if multiplier <= 1:
            self.PredictMultiplier = 1
        else:
            self.PredictMultiplier = multiplier


#
# private
#
    # Check if a path is valid and file exists
    def checkPath(path):
        if path is None:
            # No CSV path given, error and return
            print("CSV Path is blank!")
            return False
        if os.path.exists(path) == False:
            # No file at path location, error and return
            print("CSV file doesn't exist!")
            return False
        return True
    
    # Insert's a bias of 1's to the data
    def insert_bias(data):
        ones = np.ones((1, data.shape[1]))
        return np.append(ones, data, axis=0)
		