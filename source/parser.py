import os

# Read in the file
with open('game.cpp', 'r') as file :
  filedata = file.read()
  
# nt == windows
if(os.name == 'nt'):
  filedata = filedata.replace('../', './')
else:
  filedata = filedata.replace('./', '../')
  
# Write the file out again
with open('game.cpp', 'w') as file:
  file.write(filedata)
