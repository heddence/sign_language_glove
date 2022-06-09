import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

from sklearn import preprocessing
from sklearn.model_selection import train_test_split

# Classifiers
from sklearn.neighbors import KNeighborsClassifier
from sklearn.naive_bayes import GaussianNB, BernoulliNB
from sklearn.linear_model import LogisticRegression, SGDClassifier
from sklearn.svm import SVC, LinearSVC, NuSVC

# Metrics
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn.metrics import recall_score
from sklearn.metrics import precision_score
from sklearn.metrics import f1_score

# Load dataset
df = pd.read_csv('dataset/good-bad.csv')

# Preprocess data: remove unwanted columns
df = df.drop(labels=['FLEX5'], axis=1)
print(df.head())

# Make target column labeled
label_encoder = preprocessing.LabelEncoder()
df['target'] = label_encoder.fit_transform(df['target'])
print(df['target'].unique())

# Data to ML
data = df[['ACCEL_XOUT_H', 'ACCEL_YOUT_H', 'ACCEL_ZOUT_H',
           'GYRO_XOUT_H', 'GYRO_YOUT_H', 'GYRO_ZOUT_H',
           'FLEX1', 'FLEX2', 'FLEX3', 'FLEX4', 'CHECKSUM']]
target = df['target'].to_numpy()

# Scale data
scaler = preprocessing.StandardScaler()
scaled = scaler.fit_transform(data)

# Split data
x1, x2, y1, y2 = train_test_split(scaled, target, train_size=0.8)
print(x1.shape, x2.shape)
print(y1.shape, y2.shape)

# LSVC Classifier
LSVC = LinearSVC()
LSVC.fit(x1, y1)
y2_LSVC_model = LSVC.predict(x2)
print(f'LSVC Accuracy : {accuracy_score(y2, y2_LSVC_model):.4f}')
print(f'LSVC Recall : {recall_score(y2, y2_LSVC_model, average="micro"):.4f}')
print(f'LSVC Precision : {precision_score(y2, y2_LSVC_model, average="micro"):.4f}')
print(f'LSVC f1-score : {f1_score(y2, y2_LSVC_model, average="micro"):.4f}')

if __name__ == '__main__':
    # Plots
    sns.set_theme()
    cm = 1 / 2.54

    # Accelerometer's data plot (classes)
    df_accel = df[['ACCEL_XOUT_H', 'ACCEL_YOUT_H', 'ACCEL_ZOUT_H', 'target']]  # Take columns for plot
    dfm = df_accel.melt('target', var_name='ACCEL_OUT_H', value_name='values')  # Melt DataFrame for sns plot

    fig1, ax1 = plt.subplots(figsize=(17 * cm, 9 * cm), dpi=150)
    scatter = sns.scatterplot(ax=ax1, y='target', x='values', hue='ACCEL_OUT_H', data=dfm, s=100)
    scatter.set_title('Данные аксселерометра (классы)', fontsize=20, y=1.03)  # Set title of figure

    ax1.yaxis.set_ticks([0, 1, 2])
    ax1.yaxis.set_ticklabels(['Плохо', 'Хорошо', 'Расслаб.'], rotation=45)
    ax1.set_ylabel(None)
    ax1.set_xlabel(None)
    h, _ = ax1.get_legend_handles_labels()
    ax1.legend(h, ['X', 'Y', 'Z'], fontsize=15)
    plt.savefig('figs/accel-scatter.png')

    # Accelerometer's data plot (time)
    time_array = np.arange(0, 1500, 1)
    y1_acc = df.iloc[:, 0]
    y2_acc = df.iloc[:, 1]
    y3_acc = df.iloc[:, 2]

    fig2, axs2 = plt.subplots(3, 1, figsize=(17 * cm, 9 * cm), dpi=150, sharex=True)
    fig2.suptitle('Данные аксселерометра (время)', fontsize=15, y=0.97)
    lp1 = sns.lineplot(x=time_array, y=y1_acc, ax=axs2[0])
    lp2 = sns.lineplot(x=time_array, y=y2_acc, ax=axs2[1])
    lp3 = sns.lineplot(x=time_array, y=y3_acc, ax=axs2[2])

    axs2[0].xaxis.set_ticks(np.arange(0, 1501, 500))
    axs2[0].yaxis.set_label_position('right')
    axs2[1].yaxis.set_label_position('right')
    axs2[2].yaxis.set_label_position('right')
    lp1.set_ylabel('X')
    lp2.set_ylabel('Y')
    lp3.set_ylabel('Z')

    axs2[0].fill_between(x=time_array, y1=y1_acc, y2=y1_acc.min() - 100, alpha=0.25)
    axs2[1].fill_between(x=time_array, y1=y2_acc, y2=y2_acc.min() - 100, alpha=0.25)
    axs2[2].fill_between(x=time_array, y1=y3_acc, y2=y3_acc.min() - 100, alpha=0.25)
    plt.savefig('figs/acc-data.png')

    # Initialize classifiers, fit, print accuracy
    gnb = GaussianNB()
    gnb.fit(x1, y1)
    y2_GNB_model = gnb.predict(x2)
    print(f"GaussianNB Accuracy : {accuracy_score(y2, y2_GNB_model):.3f}")

    KNN = KNeighborsClassifier(n_neighbors=1)
    KNN.fit(x1, y1)
    y2_KNN_model = KNN.predict(x2)
    print(f"KNN Accuracy : {accuracy_score(y2, y2_KNN_model):.4f}")

    BNB = BernoulliNB()
    BNB.fit(x1, y1)
    y2_BNB_model = BNB.predict(x2)
    print(f"BNB Accuracy : {accuracy_score(y2, y2_BNB_model):.4f}")

    LR = LogisticRegression()
    LR.fit(x1, y1)
    y2_LR_model = LR.predict(x2)
    print(f"LR Accuracy : {accuracy_score(y2, y2_LR_model):.4f}")

    SDG = SGDClassifier()
    SDG.fit(x1, y1)
    y2_SDG_model = SDG.predict(x2)
    print(f"SDG Accuracy : {accuracy_score(y2, y2_SDG_model):.4f}")

    SVC = SVC()
    SVC.fit(x1, y1)
    y2_SVC_model = SVC.predict(x2)
    print(f"SVC Accuracy : {accuracy_score(y2, y2_SVC_model):.4f}")

    NSVC = NuSVC()
    NSVC.fit(x1, y1)
    y2_NSVC_model = NSVC.predict(x2)
    print(f"NSVC Accuracy : {accuracy_score(y2, y2_NSVC_model):.4f}")

    # Plot confusion matrix
    cf_matrix = confusion_matrix(y2, y2_LSVC_model)
    plt.figure(figsize=(17 * cm, 12 * cm), dpi=150)
    htm = sns.heatmap(cf_matrix, annot=True, cmap='Blues', fmt='g',
                      xticklabels=list(predictions.values()), yticklabels=list(predictions.values()))
    htm.set_title('Матрица ошибок', fontsize=20, y=1.03)  # Set title of figure

    htm.set_ylabel('Настоящие значения')
    htm.set_xlabel('Предсказанные значения')
    htm.yaxis.labelpad = 15
    plt.savefig('figs/confusion-matrix.png')

    """
    accuracies = []
    sample_num = []
    portion = [i for i in range(1, 10, 1)]
    for i in portion:
        x1, x2, y1, y2 = train_test_split(scaled, target, test_size=i / 10)
        sample_num.append(x1.shape[0])
        LSVC.fit(x1, y1)
        y2_LSVC_model = LSVC.predict(x2)
        accuracies.append(accuracy_score(y2, y2_LSVC_model))
    print(accuracies)
    print(sample_num)
    """
