instFile = open('test.dat', 'r')
lines = instFile.readlines()

for i in range(int(len(lines)/3)):
  if(i%64==0):
    if(abs((float(lines[i]) + float(lines[i+16384]))-float(lines[i+2*16384]))>0.0001):
      print(float(lines[i]), float(lines[i+16384]), float(lines[i+2*16384]))