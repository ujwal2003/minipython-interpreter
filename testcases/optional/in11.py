def appendVal(listX,x):
    listTemp = [x,x,x]
    listX = listX + listTemp
    return listX
    
def Increment(flag):
    if flag == 0:
        val = 10
    else:
        val = 100
    return val

flag = 1
listA = [1,2,3,4]
listA = appendVal(listA,Increment(flag))
print(listA)
