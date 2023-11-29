import csv
import time
import matplotlib.pyplot as plt
import math
mydata =[]

with open("ver_1.txt",mode="r") as myfile:
    csv_data_1 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_1)

with open("ver_2.txt",mode="r") as myfile:
    csv_data_2 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_2)


with open("ver_3.txt",mode="r") as myfile:
    csv_data_3 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_3)
    

with open("ver_4.txt",mode="r") as myfile:
    csv_data_4 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_4)


# code repeated (instead of 3d array ) for easy understanding 

clients =[]
requests=[]
successes=[]
timeout=[]
error =[]
average_rt =[]
throughput=[]
average_rt_2=[]

for i in range(len(mydata)):
    extracted = [int(x[0]) for x in mydata[i]]
    clients.append(extracted)

    extracted = [round(float(x[1]),2) for x in mydata[i]]
    requests.append(extracted)

    extracted = [round(float(x[2]),2) for x in mydata[i]]
    successes.append(extracted)

    extracted = [round(float(x[3]),2) for x in mydata[i]]
    timeout.append(extracted)

    extracted = [round(float(x[4]),2) for x in mydata[i]]
    error.append(extracted)

    extracted = [round(float(x[5]),2) for x in mydata[i]]
    average_rt.append(extracted)

    extracted = [round(float(x[6]),2) for x in mydata[i]]
    throughput.append(extracted)

    if i==3:
        extracted = [round(float(x[7]),2) for x in mydata[i]]
        average_rt_2.append(extracted)

    


# plotting the graph

#request rate
plt.plot(clients[0],requests[0],label="Version 1")
plt.plot(clients[1],requests[1],label="Version 2") 
plt.plot(clients[2],requests[2],label="Version 3")
plt.plot(clients[3],requests[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Request rate (s)")
plt.savefig("request_rate.png")
plt.clf()



#success percent
plt.plot(clients[0],successes[0],label="Version 1")
plt.plot(clients[1],successes[1],label="Version 2") 
plt.plot(clients[2],successes[2],label="Version 3")
plt.plot(clients[3],successes[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Success percent")
plt.savefig("success_percent.png")
plt.clf()


#timeout percent
plt.plot(clients[0],timeout[0],label="Version 1")
plt.plot(clients[1],timeout[1],label="Version 2") 
plt.plot(clients[2],timeout[2],label="Version 3")
plt.plot(clients[3],timeout[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Timeout percent")
plt.savefig("timeout_percent.png")
plt.clf()

#error percent
plt.plot(clients[0],error[0],label="Version 1")
plt.plot(clients[1],error[1],label="Version 2") 
plt.plot(clients[2],error[2],label="Version 3")
plt.plot(clients[3],error[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Other errors percent")
plt.savefig("error_percent.png")
plt.clf()


#average response time 
plt.plot(clients[0],average_rt[0],label="Version 1")
plt.plot(clients[1],average_rt[1],label="Version 2") 
plt.plot(clients[2],average_rt[2],label="Version 3")
plt.plot(clients[3],average_rt[3],label="Version 4")
plt.plot(clients[3],average_rt_2[3],label="Version 4 RT2")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Average response time (ms)")
plt.savefig("averate_rt_percent.png")
plt.clf()

#throughput 
plt.plot(clients[0],throughput[0],label="Version 1")
plt.plot(clients[1],throughput[1],label="Version 2") 
plt.plot(clients[2],throughput[2],label="Version 3")
plt.plot(clients[3],throughput[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Throughput")
plt.savefig("throughput.png")
plt.clf()

