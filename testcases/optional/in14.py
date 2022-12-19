def Increment(flag):
    if flag == 0:
        val = 10
    else:
        val = 20

    return val

def GetVal(i):
    val = 0
    flag = 1
    val2 = 50
    constants = [10,20,30,40,50,100]
    if i <=5 :
        val = val + 10
        if flag == 1 :
            tmp = Increment(flag)
            val = val + tmp
            tmp = constants[4]
            val = val + tmp
            if i == 2:
                val = val + val2
                val = val + val2
                val = val + 20
            else:
                val = val + 30
        else:
            if i == 2:
                val = val + 40
            else:
                val = val + 50
    else:
        val = val + 20
        if flag == 1 :
            if i == 6:
                tmp = Increment(flag)
                val = val + tmp
                tmp = constants[5]
                val = val + tmp
                val = val + 60
            else:
                val = val + 70
        else:
            if i == 6:
                val = val + 80
            else:
                val = val + 90

    return val


val1 = GetVal(2)
val2 = GetVal(6)
sumVal = val1 + val2
print(sumVal)
