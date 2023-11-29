import csv
import matplotlib.pyplot as plt
import math

mydata =[]

with open("ver1_server.txt",mode="r") as myfile:
    csv_data_1 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_1)

with open("ver2_server.txt",mode="r") as myfile:
    csv_data_2 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_2)


with open("ver3_server.txt",mode="r") as myfile:
    csv_data_3 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_3)

with open("ver4_server.txt",mode="r") as myfile:
    csv_data_4 = list(csv.reader(myfile))[1:]
    mydata.append(csv_data_4)


clients=[[] for x in mydata]
cpu =[[] for x in mydata]
threads=[[] for x in mydata]
queue_size=[[] for x in mydata]
service_time=[[] for x in mydata]


for i in range(len(mydata)):
    cp=[]
    th=[]
    qs=[]
    st=[]
    for line in mydata[i]:
        if len(line)==1:
            clients[i].append(int(line[0]))
            if len(clients[i])!=1:
                cpu[i].append(sum(cp)/len(cp))
                threads[i].append(sum(th)/len(th))
                queue_size[i].append(sum(qs)/len(qs))
                service_time[i].append(sum(st)/len(st))
                cp.clear()
                th.clear()
                qs.clear()
                st.clear()
        else:
            cp.append(float(line[0]))
            th.append(float(line[1]))
            qs.append(float(line[2]))
            st.append(float(line[3]))
    
for i in range(len(mydata)):
    clients[i].pop(-1)


# plotting the graph
#cpu utilization
plt.plot(clients[0],cpu[0],label="Version 1")
plt.plot(clients[1],cpu[1],label="Version 2") 
plt.plot(clients[2],cpu[2],label="Version 3")
plt.plot(clients[3],cpu[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("CPU Utilization %")
plt.savefig("cpu.png")
plt.clf()



#number of threads
plt.plot(clients[0],threads[0],label="Version 1")
plt.plot(clients[1],threads[1],label="Version 2") 
plt.plot(clients[2],threads[2],label="Version 3")
plt.plot(clients[3],threads[3],label="Version 4")

plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Average number of threads")
plt.savefig("threads.png")
plt.clf()


#average queue size
plt.plot(clients[0],queue_size[0],label="Version 1")
plt.plot(clients[1],queue_size[1],label="Version 2") 
plt.plot(clients[2],queue_size[2],label="Version 3")
plt.plot(clients[3],queue_size[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Average queue size")
plt.savefig("queue.png")
plt.clf()

#error percent
plt.plot(clients[0],service_time[0],label="Version 1")
plt.plot(clients[1],service_time[1],label="Version 2") 
plt.plot(clients[2],service_time[2],label="Version 3")
plt.plot(clients[3],service_time[3],label="Version 4")
plt.legend(loc=0)
plt.grid()
plt.xlabel("Number of Clients")
plt.ylabel("Service time (ms)")
plt.savefig("service_time.png")
plt.clf()

