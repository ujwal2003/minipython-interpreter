def IncrementIterator(itr):
    itr = itr + 1
    return itr
    
def add(val):
    x = [1,2,3,4,5]
    #TYPE ERROR
    #cout an error
    temp = x + val
    print(temp)
    

#initialize
elements = [3,6,9]
i = 0

#increment
i = IncrementIterator(i)
add(i)
