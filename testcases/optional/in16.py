def increment(val):
    val = val + 1
    return val


i=0
i = increment(i)
i = increment(i)
i = increment(i)
i = increment(i)
i = increment(i)
i = increment(i)

a=0
if i <=5 :
    if i == 1:
        if i == 2:
            a=a+1
            b=b*a
            c=a+b
            print("c: ",c)
    else:
        if i == 3:
            print("C: ",i)
        else:
            print("D: ",i)
else:
    if i >= 6:
        if i == 2:
            print("E: ",i)
        else:
            print("F: ",i)
    else:
        if i == 3:
            print("G: ",i)
        else:
            print("H: ",i)
