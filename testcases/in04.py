#Initialize list
listX = [1,2,3,4,5]
ListY = [7,8,9,10]
listZ = []

#Increment x
x = 0 
x = x+1

listZ = listX + ListY
listZ[0] = listZ[0] + x

x = x+1
listZ[1] = listZ[1] + x

x = x+1
listZ[2] = listZ[2] + x

x = x+1
listZ[3] = listZ[3] + x

if x <= 3:
    print(x)
else:
    print("listZ: ",listZ)
