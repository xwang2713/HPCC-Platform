### mini cluster (hthor)
1. list pods
```console
kubectl get pod
NAME                             READY   STATUS    RESTARTS   AGE
eclwatch-c565cc659-d45mr         0/1     Pending   0          8m47s
hthor-5968578967-59jjg           0/1     Pending   0          8m47s
mydali-7d46c5769c-f4gdv          0/2     Pending   0          8m47s
myeclccserver-7f695ccd7c-gf2cl   0/1     Pending   0          8m47s
spray-service-6977d9d46f-q4jd2   0/1     Pending   0          8m47s
```
2. virtual node
container group only has 400 mi cpu and 1600 MB memory
```console
   ming@RIS-Cfpgz3T8FNL:~/work/HPCC/Cloud/Azure/ACI/HPCC-Platform/helm$ kubectl describe node virtual-node-aci-linux
Name:               virtual-node-aci-linux
Roles:              agent
Labels:             alpha.service-controller.kubernetes.io/exclude-balancer=true
                    beta.kubernetes.io/os=linux
                    kubernetes.azure.com/managed=false
                    kubernetes.azure.com/role=agent
                    kubernetes.io/hostname=virtual-node-aci-linux
                    kubernetes.io/role=agent
                    node-role.kubernetes.io/agent=
                    node.kubernetes.io/exclude-from-external-load-balancers=true
                    type=virtual-kubelet
Annotations:        node.alpha.kubernetes.io/ttl: 0
                    virtual-kubelet.io/last-applied-node-status:
                      {"capacity":{"cpu":"10k","memory":"4Ti","pods":"5k"},"allocatable":{"cpu":"10k","memory":"4Ti","pods":"5k"},"conditions":[{"type":"Ready",...
                    virtual-kubelet.io/last-applied-object-meta:
                      {"name":"virtual-node-aci-linux","uid":"ab9308e3-2aef-486f-b03a-756e9c05e1fc","creationTimestamp":null,"labels":{"alpha.service-controller...
CreationTimestamp:  Mon, 17 Apr 2023 21:54:55 -0400
Taints:             virtual-kubelet.io/provider=azure:NoSchedule
Unschedulable:      false
Lease:
  HolderIdentity:  virtual-node-aci-linux
  AcquireTime:     <unset>
  RenewTime:       Mon, 17 Apr 2023 23:10:36 -0400
Conditions:
  Type                 Status  LastHeartbeatTime                 LastTransitionTime                Reason                       Message
  ----                 ------  -----------------                 ------------------                ------                       -------
  Ready                True    Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletReady                 kubelet is ready.
  OutOfDisk            False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasSufficientDisk     kubelet has sufficient disk space available
  MemoryPressure       False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasSufficientMemory   kubelet has sufficient memory available
  DiskPressure         False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasNoDiskPressure     kubelet has no disk pressure
  NetworkUnavailable   False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   RouteCreated                 RouteController created a route
Addresses:
  InternalIP:  10.224.0.119
Capacity:
  cpu:     10k
  memory:  4Ti
  pods:    5k
Allocatable:
  cpu:     10k
  memory:  4Ti
  pods:    5k
System Info:
  Machine ID:
  System UUID:
  Boot ID:
  Kernel Version:
  OS Image:
  Operating System:           Linux
  Architecture:               amd64
  Container Runtime Version:
  Kubelet Version:            v1.19.10-vk-azure-aci-1.4.8
  Kube-Proxy Version:
Non-terminated Pods:          (5 in total)
  Namespace                   Name                              CPU Requests  CPU Limits  Memory Requests  Memory Limits  Age
  ---------                   ----                              ------------  ----------  ---------------  -------------  ---
  default                     eclwatch-c565cc659-d45mr          0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
  default                     hthor-5968578967-59jjg            200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     4m3s
  default                     mydali-7d46c5769c-f4gdv           0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
  default                     myeclccserver-7f695ccd7c-gf2cl    200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     4m3s
  default                     spray-service-6977d9d46f-q4jd2    0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
Allocated resources:
  (Total limits may be over 100 percent, i.e., overcommitted.)
  Resource           Requests     Limits
  --------           --------     ------
  cpu                400m (0%)    400m (0%)
  memory             1600Mi (0%)  1600Mi (0%)
  ephemeral-storage  0 (0%)       0 (0%)
Events:              <none>
```
