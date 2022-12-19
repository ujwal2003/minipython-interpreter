def append(listX,x):
    listTemp = [x,x,x]
    listX = listX + listTemp
    return listX

def add(x,y):
    return x + y

def increment(val):
    val = val + 1
    return val

#Initialize list
listX = []

#Increment x
x = 0
x = increment(x)
#Append x
listX = append(listX,x)

y = 10
x = increment(x)
x = add(x,y)
listX = append(listX,x)

x = increment(x)
x = add(x,y)
listX = append(listX,x)

print("listX: ", listX)
