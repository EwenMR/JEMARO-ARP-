import pandas as pd
import numpy as np
from sklearn.model_selection import KFold
from sklearn.metrics import accuracy_score
TRAIN_RATE=0.72
ALPHA=1

# Task 1, data preprocessing ---------------------------------------------------------------------------------------------
# Read and format data
data = pd.read_csv('./data.txt', sep="\s+")

target = data.columns[-1]  
factors = data.columns[:-1]
print("Target:", target)
print("Factors:", factors)
# Split the data into training set and test set
data_train = data.sample(int(len(data)*TRAIN_RATE), random_state= 42)
data_test = data.drop(data_train.index)




# Task 2 Naive classifier ----------------------------------------------------------------------------------------------
def naive_bayes_classifier(train_data, target, factors, test_data):
    #probability of yes and no(the target)
    target_probability = train_data[target].value_counts(normalize=True).to_dict()
    probabilities = {}

    # pseudo code
    # FOR each factor 'Outlook', 'Temperature', 'Humidity', 'Windy'
    #     make a dictionary to store probabilities
    #     FOR every element of a factor that is not the same,(Outlook: Sunny, Overcast, Rainy) 
    #         make a sub-dictionary to store probabilities
    #         FOR every target that are not the same: (yes,no)
    #             calculate the probability: {Outlook {Sunny {Probability of 'Yes', Probability of 'No'}}}
    #Train
    for factor in factors:
        probabilities[factor] = {}
        for factor_value in train_data[factor].unique():
            probabilities[factor][factor_value] = {}
            for target_value in train_data[target].unique():
                numerator = len(train_data[(train_data[factor] == factor_value) & (train_data[target] == target_value)])
                denominator = len(train_data[train_data[target] == target_value])
                probabilities[factor][factor_value][target_value] = numerator / denominator


    #Test
    predictions = []
    for index, value in test_data.iterrows():
        target_probability_new = {}
        for target_value in train_data[target].unique():
            probability = target_probability[target_value]
            for factor in factors:
                probability *= probabilities[factor][value[factor]][target_value]
            target_probability_new[target_value] = probability
        predicted_class = max(target_probability_new, key=target_probability_new.get)
        predictions.append(predicted_class)

    return predictions

predictions = naive_bayes_classifier(data_train, target, factors, data_test)

result_df = pd.DataFrame({'Target': data_test[target].reset_index(drop=True), 'Prediction': predictions})

# Print result
# print("Result of Naive Bayes Classifier:")
# print(result_df)



# Task 3 Laplace smoothing ------------------------------------------------------------------------------------------------

def smoothed_classifier(train_data, target, factors, test_data, alpha=ALPHA):
    target_probability = train_data[target].value_counts(normalize=True).to_dict()

    probabilities = {}

    # Train
    for factor in factors:
        probabilities[factor] = {}
        for factor_value in train_data[factor].unique():
            probabilities[factor][factor_value] = {}
            for target_value in train_data[target].unique():
                # Apply Laplace smoothing to the numerator and denominator
                numerator = len(train_data[(train_data[factor] == factor_value) & (train_data[target] == target_value)]) + alpha
                denominator = len(train_data[train_data[target] == target_value]) + alpha * len(train_data[factor].unique())
                probabilities[factor][factor_value][target_value] = numerator / denominator

    # Test
    predictions = []
    for index, value in test_data.iterrows():
        target_probability_new = {}
        for target_value in train_data[target].unique():
            probability = target_probability[target_value]
            for factor in factors:
                probability *= probabilities[factor][value[factor]][target_value]
            target_probability_new[target_value] = probability
        predicted_class = max(target_probability_new, key=target_probability_new.get)
        predictions.append(predicted_class)

    return predictions

#def smoothed_classifier(train_data, target, factors, test_data,alpha=ALPHA):
    target_probability = laplace_smoothing(train_data[target].value_counts(), alpha).to_dict()

    probabilities = {}

    #Train
    for factor in factors:
        probabilities[factor] = {}
        for factor_value in train_data[factor].unique():
            probabilities[factor][factor_value] = {}
            for target_value in train_data[target].unique():
                numerator = len(train_data[(train_data[factor] == factor_value) & (train_data[target] == target_value)]) + alpha
                denominator = len(train_data[train_data[target] == target_value]) + alpha*len(train_data[factor].unique())
                probabilities[factor][factor_value][target_value] = numerator / denominator

    #Test
    predictions = []
    for index, value in test_data.iterrows():
        target_probability_new = {}
        for target_value in train_data[target].unique():
            probability = target_probability[target_value]
            for factor in factors:
                probability *= probabilities[factor][value[factor]][target_value]
            target_probability_new[target_value] = probability
        predicted_class = max(target_probability_new, key=target_probability_new.get)
        predictions.append(predicted_class)

    return predictions

predictions = smoothed_classifier(data_train, target, factors, data_test,alpha=ALPHA)
result_df = pd.DataFrame({'Target': data_test[target].reset_index(drop=True), 'Prediction': predictions})

# print result
# print("Result with laplace smoothing:")
# print(result_df)


# Task 4 Think further -------------------------------------------------------------------------------------------------------
# cross validation
def cross_validation(data, target_column, features, num_folds=5):
    kf = KFold(n_splits=num_folds, shuffle=True, random_state=42)
    NBC_accuracy_scores = []
    LS_accuracy_scores = []
    for train_index, test_index in kf.split(data):
        data_train, data_test = data.iloc[train_index], data.iloc[test_index]
        
        NBC_predictions = naive_bayes_classifier(data_train, target, factors, data_test)
        NBC_accuracy = accuracy_score(data_test[target], NBC_predictions)
        NBC_accuracy_scores.append(NBC_accuracy)

        LS_predictions = smoothed_classifier(data_train, target, factors, data_test, alpha=ALPHA)
        LS_accuracy = accuracy_score(data_test[target], LS_predictions)
        LS_accuracy_scores.append(LS_accuracy)
        # Calculate and store the accuracy score for this fold
        

    # Calculate and print the mean accuracy and error rate across all folds
    NBC_mean_accuracy = np.mean(NBC_accuracy_scores)
    NBC_error_rate = 1 - NBC_mean_accuracy

    LS_mean_accuracy = np.mean(LS_accuracy_scores)
    LS_error_rate = 1 - LS_mean_accuracy

    return NBC_mean_accuracy, NBC_error_rate, LS_mean_accuracy, LS_error_rate

NBC_mean_accuracy, NBC_error_rate, LS_mean_accuracy, LS_error_rate=cross_validation(data,target,factors,num_folds=5)


print("Result of Naive Bayes Classifier:")
print(result_df)
print(f"Mean Accuracy: {NBC_mean_accuracy:.2f}")
print(f"Error Rate: {NBC_error_rate:.2f}")

print("Result with laplace smoothing:")
print(result_df)
print(f"Mean Accuracy: {LS_mean_accuracy:.2f}")
print(f"Error Rate: {LS_error_rate:.2f}")
print("\n")
print("Error Rate for Naive Bayes Classifier:")
print(f"{NBC_error_rate:.2f}")
print("Error Rate after Laplace Smoothing:")
print(f"{LS_error_rate:.2f}")
