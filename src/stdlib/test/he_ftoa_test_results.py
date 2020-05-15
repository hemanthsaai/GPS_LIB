file = open(r"C:\hemanth\MyProject\Src\GPS_LIB\he_ftoa_test_results.log","r")

file.readline()
count = 0
count1 = 0
while 1:
	data = file.readline()
	if not data:
		break
	count = count + 1
	count1 = count1 + 1
	if count1 > 1000000:
		count1 = 0
		print(data)
	dat0 = data.split('\t')[0]
	dat1 = data.split('\t')[2].split('\n')[0]
	if dat0 == dat1:
		continue
	else:
		print("Error in line   ", data)
		break

print("YoYo We rocked it")
print("Tested ", count, " values with no error. ")