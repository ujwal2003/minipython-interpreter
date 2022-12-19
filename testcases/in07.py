listX1 = [10,0,0,0]
listX2 = [0,20,0,0]
listX3 = [0,0,30,0]
listX4 = [0,0,0,40]
listX5 = listX1[:]
listX5[1:] = listX2[1:]
listX5[2:] = listX3[2:]
listX5[3:] = listX4[3:]


if listX1[0] == 10:
    if listX2[1] == 0:
        result = 0
        print(result)
    else:
        result = 1
        print(result)
if listX3[2] == 30:
    if listX4[3] == 40:
        print(listX5)
    else:
        result = 3
        print(result)
