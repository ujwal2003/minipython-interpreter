def f(i):
 i=i+100
 return i

# notice one variable returned to make it simple
def add(x,y):
 z=x+y
 return z


# TA can change these values 
x=1
y=1
sum=add(x,y)

if sum<1:
  sum=1000
else:
  if x<0:
    sum=sum+1
  else:
    sum=sum+sum

print("sum: ",sum)

sum=sum+f(100)
print("sum: ",sum)
