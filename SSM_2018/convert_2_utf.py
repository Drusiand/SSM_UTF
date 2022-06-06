#!/usr/bin/env python
# coding: utf-8

# In[1]:


import math
import numpy as np
import sys
import h5py


# In[48]:


file_in = sys.argv[1]


# In[49]:


h5_in = h5py.File(file_in, 'r')


# In[50]:


file_out = sys.argv[2]


# In[51]:


h5_out = h5py.File(file_out, 'w')


# In[52]:


keys_in = list(h5_in.keys())


# In[54]:


ds = h5_in['gr_names']
dsa = h5_in['gr_names'][:]


# In[55]:


dta = h5py.string_dtype(encoding='utf-8')


# In[56]:


h5_out.create_dataset('species', data = h5_in['species'][:], dtype = dta)


# In[57]:


h5_out.create_dataset('gr_names', data = dsa, dtype = dta)


# In[59]:


for k in keys_in:
    if k != 'gr_names' and k != 'species':
        h5_out.create_dataset(k, data = h5_in[k][:])


# In[60]:


h5_out.close()


# In[ ]:




