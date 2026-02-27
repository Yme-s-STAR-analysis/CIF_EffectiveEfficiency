import pandas as pd
import numpy as np

energyTag = '7'
pro = pd.read_csv(f'ProEff.{energyTag}.csv', header=0, index_col=False)
pro.to_hdf('m02_efficiency.h5', key='proton')
pbar = pd.read_csv(f'PbarEff.{energyTag}.csv', header=0, index_col=False)
pbar.to_hdf('m02_efficiency.h5', key='antiproton')