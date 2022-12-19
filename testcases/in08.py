#Return list at index
def getElement(data,index):
    element = data[index]
    return element

#Increment val
def increment(val):
    val = val + 1
    return val

data = [100,99,88,77,66,65,72,53,1,134,23,51,38,50,6]

a = getElement(data,0)
l= [1,2,3]
l = l + [a]

print("l: ",l)

h=0
# insert at head
l= [h]+l
print("l: ",l)

e=100
# append at end
l= l+[e]
print("l: ",l)

# print string length
listlen=len(l)
print("listlen",listlen)

