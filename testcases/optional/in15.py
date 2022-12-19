def Increment(counter):
    counter = counter + 1
    return counter




counter = 1
listX = [0,0]
#BEGIN
while (counter <= 100):
    if counter <= 50:
        listX[0] = listX[0] + 1
    if counter > 50:
        listX[1] = listX[1] + 5
    counter = Increment(counter)

tmp = listX[0] + listX[1]
print(tmp)
