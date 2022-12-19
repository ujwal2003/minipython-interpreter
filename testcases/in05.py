def addConstant(x):
    val = 3
    x = x + val
    return x

listA = [2,4,6,8,10]
listB = [3,6,9,12,15]
listC = [4,8,12,16,20]


x = 0
x = addConstant(x)

head1 = [100]
head2 = [300]
if x <= 1:
    listD = head1 + listA + listB + listC  
    print("listD: ",listD)
else:
    listA = head2 + listA + listB + listC  
    print("listA: ",listA)
