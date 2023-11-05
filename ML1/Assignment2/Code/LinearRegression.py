import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Task 1 --------------------------------------------------------------------------------------------------------------------------------------------
car_df = pd.read_csv('./mtcarsdata-4features.csv')
turkish_df = pd.read_csv('./turkish-se-SP500vsMSCI.csv', header=0)

# Task 2 --------------------------------------------------------------------------------------------------------------------------------------------
# 1 

def linear_1d(X,y,b):
    # Calculate means
    X_mean = np.mean(X)
    y_mean = np.mean(y)

    # Calculate the slope (m) using the formula
    m = np.sum((X - X_mean) * (y - y_mean)) / np.sum((X - X_mean) ** 2)

    #if intercept is 0
    if b == 0:
        b=0
    #if there is intercept
    elif b==1:
        b = y_mean - m * X_mean
    else:
        exit

    


    # Visualization
    plt.scatter(X, y, label='Data')
    plt.plot(X, m * X + b, color='red', label='Regression Line')
    plt.legend()
    plt.show()


    return 0

X = turkish_df.iloc[:,0]
y = turkish_df.iloc[:,1]
X = np.array(X)
y = np.array(y)
linear_1d(X,y,b=0)

# 2 -------------------------------------------------------------------------------------------------



X = turkish_df.iloc[:,0]
y = turkish_df.iloc[:,1]
X = np.array(X)
y = np.array(y)
percentage = 0.10
num_elements_to_select = int(percentage * len(X))

# Randomly sample elements from X
X = np.random.choice(X, num_elements_to_select, replace=False)
y = np.random.choice(y, num_elements_to_select, replace=False)
linear_1d(X,y,b=1)


 



# # 3 -------------------------------------------------------------------------------------------------
X = car_df[' mpg']
y = car_df[' weight']
X = np.array(X)
y = np.array(y)
linear_1d(X,y,1)

# # 4 -------------------------------------------------------------------------------------------------
def multi_dimensional_regression(X, y):
    # Add a column of ones for the intercept term
    X_with_bias = np.c_[np.ones((X.shape[0], 1)), X]

    # Calculate coefficients using the closed-form solution
    coefficients = np.linalg.pinv(X_with_bias.T.dot(X_with_bias)).dot(X_with_bias.T).dot(y)

    # Make predictions
    y_pred = X_with_bias.dot(coefficients)

    # Visualize the actual vs. predicted values
    plt.scatter(y, y_pred)
    plt.xlabel("Actual 'mpg'")
    plt.ylabel("Predicted 'mpg'")
    plt.title("Actual vs. Predicted 'mpg'")
    plt.show()

    return coefficients, y_pred


y = car_df[' mpg']
X = car_df.drop(['Model', ' mpg'], axis=1)

coefficients, y_pred = multi_dimensional_regression(X, y)
print("Intercept:", coefficients[0])
print("Coefficients:", coefficients[1:])



# Task 3 -------------------------------------------------------------------------------------------------
# 1 
def calculate_mse1(model, X, y):
    # Calculate the predicted values using the model (m and b)
    y_pred = model * X  # Assuming model represents the slope 'm'

    # Calculate the Mean Squared Error (MSE)
    mse = ((y - y_pred) ** 2).mean()

    return mse


def repeat_and_calculate_mse(X, y, b, percentage, num_repeats):
    mse_list = []

    for i in range(num_repeats):
        # Randomly sample data for the specified percentage
        num_elements_to_select = int(percentage * len(X))
        selected_indices = np.random.choice(len(X), num_elements_to_select, replace=False)
        X_train = X[selected_indices]
        y_train = y[selected_indices]

        # Calculate the model using the training data
        model = linear_1d(X_train, y_train,b)

        # Calculate the MSE on the training data
        mse_train = calculate_mse1(model, X_train, y_train)

        # Calculate the MSE on the remaining data
        remaining_indices = np.setdiff1d(np.arange(len(X)), selected_indices)
        X_remaining = X[remaining_indices]
        y_remaining = y[remaining_indices]
        mse_remaining = calculate_mse1(model, X_remaining, y_remaining)

        mse_list.append((mse_train, mse_remaining))
        print(f"Iteration {i + 1}:")
        print(f"Training Data MSE: {mse_train:.15f}")
        print(f"Remaining Data MSE: {mse_remaining:.15f}")
        print()

    return mse_list


# Sample data (replace with your actual data)
X = turkish_df.iloc[:,0]
y = turkish_df.iloc[:,1]
X = np.array(X)
y = np.array(y)
mse_list = repeat_and_calculate_mse(X, y, b=0, percentage=0.05, num_repeats=10)





# # 3 -------------------------------------------------------------------------------------------------
X = car_df[' mpg']
y = car_df[' weight']
X = np.array(X)
y = np.array(y)
mse_list = repeat_and_calculate_mse(X, y, b=1, percentage=0.2, num_repeats=10)

# 4 -------------------------------------------------------------------------------------------------

def calculate_mse(y, y_pred):
    # Calculate the Mean Squared Error (MSE)
    mse = ((y - y_pred) ** 2).mean()
    return mse

def repeat_regression(X, y, iterations, percentage):
    for i in range(iterations):
        indices = np.arange(len(X))
        np.random.shuffle(indices)
        train_indices = indices[:int(percentage * len(X))]
        test_indices = indices[int(percentage * len(X)):]

        # Split the data into training and testing sets
        X_train, y_train = X.iloc[train_indices], y.iloc[train_indices]
        X_test, y_test = X.iloc[test_indices], y.iloc[test_indices]

        # Perform multi-dimensional regression to obtain the model
        coefficients, y_pred_train = multi_dimensional_regression(X_train, y_train)

        # Calculate the MSE for training data using the same model
        mse_train = calculate_mse(y_train, y_pred_train)

        # Use the same model to predict the test data
        X_test_with_bias = np.c_[np.ones((X_test.shape[0], 1)), X_test]
        y_pred_test = X_test_with_bias.dot(coefficients)

        # Calculate the MSE for testing data using the same model
        mse_test = calculate_mse(y_test, y_pred_test)

        print(f"Iteration {i + 1}:")
        print(f"Training Data MSE: {mse_train:.15f}")
        print(f"Testing Data MSE: {mse_test:.15f}")
        print()

# Call the function to repeat the process for 10 iterations with a 20% training set
y = car_df[' mpg']
X = car_df.drop(['Model', ' mpg'], axis=1)
repeat_regression(X, y, iterations=10, percentage=0.2)
