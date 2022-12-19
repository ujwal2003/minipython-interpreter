# list, function

def IncrementIterator(itr):
    itr = itr + 1
    return itr

#initialize
elements = [3,6,9]
i = 0

#increment
i = IncrementIterator(i)

#print
print("i=",i)
element = elements[i]
print("ith element=",element)
