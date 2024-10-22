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
    ThetaBest = None
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
        CSV_Data = pd.read_csv(csv_path, header=0)
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
        self.ThetaBest = theta
        ue.log("Determined ThetaBest! Now able to predict")


    # Called through UE4 C++. Generates a new
    def predict_cost(self, args):
        # Make sure we have a ThetaBest
        if np.any(self.ThetaBest) == False:
            ue.log_error("No theta_best has been determined!")
            return None
        # Check for blank args
        if args == None:
            ue.log_error("No arguments passed to predict_cost!");
            return None

        # Validate args length is as expected
        splitChar = ','
        split = args.split(splitChar)
        if len(split) != 5:
            ue.log_error("predict_cost recieved incorrect argument count! Should be 5, is {0}".format(len(split)))
            return None

        # Parse arguments into variables
        actionName, baseCost, wasSuccess, damage, averageExecuteSeconds = args.split(splitChar)

        ue.log("Predicting action's cost...")
        ue.log("actionName:'{0}' baseCost:'{1}' success:'{2}' damage:'{3}' avgExecuteSecs:'{4}'".format(actionName, baseCost, wasSuccess, damage, averageExecuteSeconds))
        
        # Construct array and normalize with given values
        input_vals =  [ [ float(baseCost), float(wasSuccess), float(damage), float(averageExecuteSeconds) ] ]
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
        # Convert to float
        multi = float(multiplier)
        # Only set multiplier if valid, more than 1
        if multi <= 1:
            self.PredictMultiplier = 1
        else:
            self.PredictMultiplier = multi


#
# private
#
    # Check if a path is valid and file exists
    def checkPath(self, path):
        if path is None:
            # No CSV path given, error and return
            ue.log_error("CSV Path is blank!")
            return False
        if os.path.exists(path) == False:
            # No file at path location, error and return
            ue.log_error("CSV file '{file}' doesn't exist!".format(file=path))
            return False
        return True
    
    # Insert's a bias of 1's to the data
    def insert_bias(self, data):
        ones = np.ones((1, data.shape[1]))
        return np.append(ones, data, axis=0)
		