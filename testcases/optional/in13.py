def SumOfHead(list1,list2):
    tmp = list1[0] + list2[1]
    return tmp

def InitList(list1,val):
    list1 = [val,val]
    return list1

def Max(a, b):
    if a >= b:
        return a
    else:
        return b
        
def Increment(val):
    val = val+1
    return val

def MaxElement():
    listA = []
    listB = []
    listC = []
    listD = []

    listA = InitList(listA,1)
    listB = InitList(listB,2)
    listC = InitList(listC,3)
    listD = InitList(listD,4)

    sum1 = SumOfHead(listA,listB)
    sum2 = SumOfHead(listC,listD)

    maxSum = Max(Increment(sum1),Increment(sum2))
    print(maxSum)



MaxElement()
