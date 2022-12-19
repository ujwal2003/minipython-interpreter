#Get max val between a and b
def Max(a, b):
    if a >= b:
        return a
    else:
        return b

def GetVal(i):
    val = 0
    flag = 0

    if i <=5 :
        val = val + 10
        if flag == 1 :
            if i == 2:
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
                val = val + 60
            else:
                val = val + 70
        else:
            if i == 6:
                val = val + 80
            else:
                val = val + 90

    return val


val = Max(GetVal(1),GetVal(6))
print("val: ",val)
