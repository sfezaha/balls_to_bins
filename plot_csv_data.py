 
import os
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

BASE_PATH = '/home/asfez/personal/cpp_pojects/balls_to_bins/data/'

class CSVData:
    def __init__(self, base_path):
        self.filenames = [filename for filename in os.listdir(base_path) if filename.endswith('.csv')]
        self.base_path = base_path
        for filename in self.filenames:
            print(filename)
        self.df = None

    def set_df(self):
        dfs = []
        for filename in self.filenames:
            full_path = os.path.join(self.base_path, filename)
            print(full_path)
            dfs.append(pd.read_csv(full_path))
        
        self.df = pd.concat(dfs).groupby(level=0).mean()

    def plot_data(self, title, ylabel='', colnames=None):
        if self.df is not None:
            print(self.df)
            x_column = self.df.columns[0]
            if colnames is None:
                y_columns = self.df.columns[1:]
            else:
                y_columns = colnames
            # ax = self.df.plot(x=x_column, y=y_columns, kind='line', title=title)
            ax = self.df.plot(x=x_column, y=y_columns, kind='line', title=title, marker='o', markersize=4)  # Adjust markersize here

            
        # if title == 'Average Max Loads Simulation Results':
        #     x = np.linspace(16, 1024, 400)
        #     y1 = 10000 / x + np.sqrt((10000 * np.log(x)) / x)
        #     # y2 = 10000 / x + np.log(np.log(x))
        #     plt.plot(x, y1, label=r'$\frac{m}{n} + \sqrt{\frac{m \log(n)}{n}}$')
        #     # plt.plot(x, y2, label=r'$\frac{m}{n} + {\log(\log(n))}$')
        #     plt.legend()

            
            plt.legend()
            plt.ylabel(ylabel)  # Set the y-axis label

            plt.savefig(self.base_path + 'fig.pdf')
            plt.show()
        else:
            print("DataFrame not set. Call set_df() to load and average data.")
            
if __name__ == '__main__':
    csv_data = CSVData(BASE_PATH + 'max_gaps/')
    csv_data.set_df()
    csv_data.plot_data(title='Average Max Gaps Simulation Results', 
                       ylabel='maxGap',  # Here you can specify the y-axis label
                       colnames=['Power of 2 Choices', 'Absolute minimum load', "Random allocation"])
    csv_data = CSVData(BASE_PATH + 'max_loads/')
    csv_data.set_df()
    csv_data.plot_data(title='Average Max Loads Simulation Results',
                       ylabel='maxLoad',  # Specify the y-axis label
                       colnames=['Power of 2 Choices', 'Absolute minimum load', "Random allocation"])
                    #    colnames=["Random allocation"])
    csv_data = CSVData(BASE_PATH + 'runtimes/')
    csv_data.set_df()
    csv_data.plot_data(title='Average Runtime Simulation Results',
                       ylabel='Time [us]',  # Specify the y-axis label
                       colnames=['Power of 2 Choices', 'Absolute minimum load', "Random allocation"])

